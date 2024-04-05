#include <cstdio>
#include <string>

#include "compiler.hpp"
#include "debug.hpp" // IWYU pragma: keep

namespace Clox {

std::unique_ptr<Parser> SinglePassCompiler::parser = nullptr;
std::unique_ptr<Compiler> SinglePassCompiler::current = nullptr;

// clang-format off
std::unordered_map<TokenType, ParseRule> PrattParser::rules = {
    { TokenType::LEFT_PAREN,    { &Compiler::grouping, nullptr, Precedence::NONE }},
    { TokenType::MINUS,         { &Compiler::unary,   &Compiler::binary, Precedence::TERM }},
    { TokenType::PLUS,          { nullptr, &Compiler::binary, Precedence::TERM }},
    { TokenType::SLASH,         { nullptr, &Compiler::binary, Precedence::FACTOR }},
    { TokenType::STAR,          { nullptr, &Compiler::binary, Precedence::FACTOR }},
    { TokenType::NUMBER,        { &Compiler::number, nullptr, Precedence::NONE }},
    { TokenType::STRING,        { &Compiler::string, nullptr, Precedence::NONE }},
    { TokenType::NIL,           { &Compiler::literal, nullptr, Precedence::NONE }},
    { TokenType::TRUE,          { &Compiler::literal, nullptr, Precedence::NONE }},
    { TokenType::FALSE,         { &Compiler::literal, nullptr, Precedence::NONE }},
    { TokenType::BANG,          { &Compiler::unary, nullptr, Precedence::NONE }},
    { TokenType::BANG_EQUAL,    { nullptr, &Compiler::binary, Precedence::EQUALITY }},
    { TokenType::EQUAL_EQUAL,   { nullptr, &Compiler::binary, Precedence::EQUALITY }},
    { TokenType::GREATER,       { nullptr, &Compiler::binary, Precedence::COMPARISON }},
    { TokenType::GREATER_EQUAL, { nullptr, &Compiler::binary, Precedence::COMPARISON }},
    { TokenType::LESS,          { nullptr, &Compiler::binary, Precedence::COMPARISON }},
    { TokenType::LESS_EQUAL,    { nullptr, &Compiler::binary, Precedence::COMPARISON }},
    { TokenType::IDENTIFIER,    { &Compiler::variable, nullptr, Precedence::NONE }},
    { TokenType::AND,           { nullptr, &Compiler::andOp, Precedence::AND}},
    { TokenType::OR,            { nullptr, &Compiler::orOp, Precedence::OR }},
};
// clang-format on

ParseRule Parser::getRule(TokenType type) { return rules[type]; }

PrattParser::PrattParser(const std::string &source)
    : scanner(Scanner(source)), current(scanner.scanOneToken()),
      previous(current), hadError(false), panicMode(false){};

void Parser::errorAt(const Token &token, const char *message) {
    if (panicMode)
        return;
    panicMode = true;

    std::fprintf(stderr, "[line %d] Error", token.line);
    if (token.type == TokenType::EOF_) {
        std::fprintf(stderr, " at end");
    } else if (token.type == TokenType::ERROR) {
        // do nothing
    } else {
        std::fprintf(stderr, ": at '%.*s'",
                     static_cast<int>(token.lexeme.length()),
                     token.lexeme.data());
    }
    std::fprintf(stderr, ": %s\n", message);

    hadError = true;
}

void Parser::error(const char *message) { errorAt(previous, message); }

void Parser::errorAtCurrent(const char *message) { errorAt(current, message); }

void Parser::advance() {
    previous = current;

    for (;;) {
        current = scanner.scanOneToken();
        if (current.type != TokenType::ERROR)
            break;

        errorAtCurrent(current.lexeme.data());
    }
}

void Parser::consume(TokenType type, const char *message) {
    if (current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

bool Parser::check(TokenType type) const { return current.type == type; }

bool Parser::match(TokenType type) {
    if (!check(type))
        return false;
    advance();
    return true;
}

void Parser::synchronize() {
    panicMode = false;
    while (current.type != TokenType::EOF_) {
        if (previous.type == TokenType::SEMICOLON)
            return;

        switch (current.type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        default:; // do nothing
        }

        advance();
    }
}

void Parser::parsePrecedence(Precedence precedence, Compiler &compiler) {
    advance();
    ParseFn prefixRule = getRule(previous.type).prefix;
    if (!prefixRule) {
        error("Expect expression.");
        return;
    }
    bool canAssign = precedence <= Precedence::ASSIGNMENT;
    prefixRule(compiler, canAssign);
    while (precedence <= getRule(current.type).precedence) {
        advance();
        ParseFn infixRule = getRule(previous.type).infix;
        infixRule(compiler, canAssign);
    }

    if (canAssign && match(TokenType::EQUAL)) {
        error("Invalid assignment target.");
    }
}

bool SinglePassCompiler::compile(const std::string &source, Chunk *chunk) {
    parser = std::make_unique<Parser>(source);
    current = std::make_unique<Compiler>();
    current->compilingChunk = chunk;
    current->parser = parser.get();

    while (!parser->match(TokenType::EOF_)) {
        current->declaration();
    }

    current->endCompiler();
    return !parser->hadError;
}

Chunk *SinglePassCompiler::currentChunk() const {
    return current->compilingChunk;
}

Compiler::Compiler() : localCount(0), scopeDepth(0) {}

void Compiler::declaration() {
    if (parser->match(TokenType::VAR)) {
        varDeclaration();
    } else {
        statement();
    }
    if (parser->panicMode)
        parser->synchronize();
}

void Compiler::varDeclaration() {
    uint8_t global = parseVariable("Expect variable name.");

    if (parser->match(TokenType::EQUAL)) {
        expression();
    } else {
        emitByte(OP_NIL);
    }

    parser->consume(TokenType::SEMICOLON,
                    "Expect ';' after variable declaration.");
    defineVariable(global);
}

void Compiler::variable(bool canAssign) {
    namedVariable(parser->previous, canAssign);
}

void Compiler::statement() {
    if (parser->match(TokenType::PRINT)) {
        printStatement();
    } else if (parser->match(TokenType::IF)) {
        ifStatement();
    } else if (parser->match(TokenType::LEFT_BRACE)) {
        beginScope();
        block();
        endScope();
    } else {
        expressionStatement();
    }
}

void Compiler::block() {
    while (!parser->check(TokenType::RIGHT_BRACE) &&
           !parser->check(TokenType::EOF_)) {
        declaration();
    }

    parser->consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
}

void Compiler::expressionStatement() {
    expression();
    parser->consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

void Compiler::printStatement() {
    expression();
    parser->consume(TokenType::SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

void Compiler::ifStatement() {
    parser->consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    expression();
    parser->consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

    int thenJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();

    int elseJump = emitJump(OP_JUMP);
    patchJump(thenJump);
    emitByte(OP_POP);

    if (parser->match(TokenType::ELSE))
        statement();

    patchJump(elseJump);
}

void Compiler::expression() {
    parser->parsePrecedence(Precedence::ASSIGNMENT, *this);
}

void Compiler::grouping(bool /*canAssign*/) {
    expression();
    parser->consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary(bool /*canAssign*/) {
    TokenType operatorType = parser->previous.type;
    parser->parsePrecedence(Precedence::UNARY, *this);
    switch (operatorType) {
    case TokenType::BANG:
        emitByte(OP_NOT);
        break;
    case TokenType::MINUS:
        emitByte(OP_NEGATE);
        break;
    default:
        return;
    }
}

void Compiler::binary(bool /*canAssign*/) {
    TokenType operatorType = parser->previous.type;
    ParseRule rule = parser->getRule(operatorType);
    auto nextPrec = static_cast<int>(rule.precedence) + 1;
    parser->parsePrecedence(static_cast<Precedence>(nextPrec), *this);

    switch (operatorType) {
    case TokenType::BANG_EQUAL:
        emitBytes(OP_EQUAL, OP_NOT);
        break;
    case TokenType::EQUAL_EQUAL:
        emitByte(OP_EQUAL);
        break;
    case TokenType::GREATER:
        emitByte(OP_GREATER);
        break;
    case TokenType::GREATER_EQUAL:
        emitBytes(OP_LESS, OP_NOT);
        break;
    case TokenType::LESS:
        emitByte(OP_LESS);
        break;
    case TokenType::LESS_EQUAL:
        emitBytes(OP_GREATER, OP_NOT);
        break;
    case TokenType::PLUS:
        emitByte(OP_ADD);
        break;
    case TokenType::MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TokenType::STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TokenType::SLASH:
        emitByte(OP_DIVIDE);
        break;
    default:
        return;
    }
}

void Compiler::andOp(bool /*canAssign*/) {
    int endJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    parser->parsePrecedence(Precedence::AND, *this);
    patchJump(endJump);
}

void Compiler::orOp(bool /*canAssign*/) {
    int elseJump = emitJump(OP_JUMP_IF_FALSE);
    int endJump = emitJump(OP_JUMP);

    patchJump(elseJump);
    emitByte(OP_POP);

    parser->parsePrecedence(Precedence::OR, *this);
    patchJump(endJump);
}

void Compiler::number(bool /*canAssign*/) {
    Number value = std::strtod(parser->previous.lexeme.data(), nullptr);
    emitConstant(value);
}

void Compiler::string(bool /*canAssign*/) {
    emitConstant(ObjString::copy(parser->previous.lexeme));
}

void Compiler::literal(bool /*canAssign*/) {
    switch (parser->previous.type) {
    case TokenType::FALSE:
        emitByte(OP_FALSE);
        break;
    case TokenType::TRUE:
        emitByte(OP_TRUE);
        break;
    case TokenType::NIL:
        emitByte(OP_NIL);
        break;
    default:
        return;
    }
}

void Compiler::beginScope() { scopeDepth++; }

void Compiler::endScope() {
    scopeDepth--;

    while (localCount > 0 && locals[localCount - 1].depth > scopeDepth) {
        emitByte(OP_POP);
        localCount--;
    }
}

int Compiler::resolveLocal(const Token &name) {
    for (int i = localCount - 1; i >= 0; i--) {
        Local *local = &locals[i];
        if (name.lexeme == local->name.lexeme) {
            if (local->depth == -1) {
                parser->error(
                    "Can't read local variable in its own initializer.");
            }

            return i;
        }
    }
    return -1;
}

void Compiler::addLocal(const Token &name) {
    if (localCount == UINT8_COUNT) {
        parser->error("Too many local variables in function.");
        return;
    }

    Local *local = &locals[localCount++];
    *local = {name, -1};
}

void Compiler::markInitialized() { locals[localCount - 1].depth = scopeDepth; }

uint8_t Compiler::parseVariable(const char *errorMessage) {
    parser->consume(TokenType::IDENTIFIER, errorMessage);

    declareVariable();
    if (scopeDepth > 0)
        return 0;

    return identifierConstant(parser->previous);
}

uint8_t Compiler::identifierConstant(const Token &name) {
    return makeConstant(ObjString::copy(name.lexeme));
}

void Compiler::declareVariable() {
    if (scopeDepth == 0)
        return;

    const Token &name = parser->previous;
    for (int i = localCount - 1; i >= 0; i--) {
        Local *local = &locals[i];
        if (local->depth != -1 && local->depth < scopeDepth) {
            break;
        }
        if (name.lexeme == local->name.lexeme) {
            parser->error("Already a variable with this name in this scope.");
        }
    }

    addLocal(name);
}

void Compiler::defineVariable(uint8_t global) {
    if (scopeDepth > 0) {
        markInitialized();
        return;
    }

    emitBytes(OP_DEFINE_GLOBAL, global);
}

void Compiler::namedVariable(const Token &name, bool canAssign) {
    uint8_t getOp, setOp;
    int arg = resolveLocal(name);
    if (arg != -1) {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    } else {
        arg = identifierConstant(name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (parser->match(TokenType::EQUAL) && canAssign) {
        expression();
        emitBytes(setOp, static_cast<uint8_t>(arg));
    } else {
        emitBytes(getOp, static_cast<uint8_t>(arg));
    }
}

uint8_t Compiler::makeConstant(Value value) {
    int constant = compilingChunk->addConstant(value);
    if (constant > UINT8_MAX) {
        parser->error("Too many constants in one chunk.");
        return 0;
    }

    return static_cast<uint8_t>(constant);
}

void Compiler::emitByte(uint8_t byte) {
    compilingChunk->write(byte, parser->previous.line);
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

void Compiler::emitConstant(Value value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

int Compiler::emitJump(uint8_t instruction) {
    emitByte(instruction);
    emitBytes(0xff, 0xff);
    return compilingChunk->count - 2;
}

void Compiler::patchJump(int offset) {
    int jump = compilingChunk->count - offset - 2;

    if (jump > UINT16_MAX) {
        parser->error("Too many code to jump over.");
    }

    compilingChunk->code[offset] = (jump >> 8) & 0xff;
    compilingChunk->code[offset + 1] = jump & 0xff;
}

void Compiler::emitReturn() { emitByte(OP_RETURN); }

void Compiler::endCompiler() {
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser->hadError) {
        disassembleChunk(compilingChunk, "code");
    }
#endif
}

} // namespace Clox
