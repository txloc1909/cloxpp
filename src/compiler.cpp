#include <cstdio>
#include <string>

#include "compiler.hpp"
#include "debug.hpp" // IWYU pragma: keep

namespace Clox {

// clang-format off
std::unordered_map<TokenType, ParseRule> PrattParser::rules = {
    { TokenType::LEFT_PAREN,    { &SinglePassCompiler::grouping, nullptr, Precedence::NONE }},
    { TokenType::MINUS,         { &SinglePassCompiler::unary,   &SinglePassCompiler::binary, Precedence::TERM }},
    { TokenType::PLUS,          { nullptr, &SinglePassCompiler::binary, Precedence::TERM }},
    { TokenType::SLASH,         { nullptr, &SinglePassCompiler::binary, Precedence::FACTOR }},
    { TokenType::STAR,          { nullptr, &SinglePassCompiler::binary, Precedence::FACTOR }},
    { TokenType::NUMBER,        { &SinglePassCompiler::number, nullptr, Precedence::NONE }},
    { TokenType::STRING,        { &SinglePassCompiler::string, nullptr, Precedence::NONE }},
    { TokenType::NIL,           { &SinglePassCompiler::literal, nullptr, Precedence::NONE }},
    { TokenType::TRUE,          { &SinglePassCompiler::literal, nullptr, Precedence::NONE }},
    { TokenType::FALSE,         { &SinglePassCompiler::literal, nullptr, Precedence::NONE }},
    { TokenType::BANG,          { &SinglePassCompiler::unary, nullptr, Precedence::NONE }},
    { TokenType::BANG_EQUAL,    { nullptr, &SinglePassCompiler::binary, Precedence::EQUALITY }},
    { TokenType::EQUAL_EQUAL,   { nullptr, &SinglePassCompiler::binary, Precedence::EQUALITY }},
    { TokenType::GREATER,       { nullptr, &SinglePassCompiler::binary, Precedence::COMPARISON }},
    { TokenType::GREATER_EQUAL, { nullptr, &SinglePassCompiler::binary, Precedence::COMPARISON }},
    { TokenType::LESS,          { nullptr, &SinglePassCompiler::binary, Precedence::COMPARISON }},
    { TokenType::LESS_EQUAL,    { nullptr, &SinglePassCompiler::binary, Precedence::COMPARISON }},
    { TokenType::IDENTIFIER,    { &SinglePassCompiler::variable, nullptr, Precedence::NONE }},
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

void Parser::parsePrecedence(Precedence precedence,
                             SinglePassCompiler &compiler) {
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

SinglePassCompiler::SinglePassCompiler(const std::string &source)
    : parser(Parser(source)) {}

bool SinglePassCompiler::compile(Chunk *chunk) {
    compilingChunk = chunk;

    while (!parser.match(TokenType::EOF_)) {
        declaration();
    }
    endCompiler();
    return !parser.hadError;
}

void SinglePassCompiler::declaration() {
    if (parser.match(TokenType::VAR)) {
        varDeclaration();
    } else {
        statement();
    }
    if (parser.panicMode)
        parser.synchronize();
}

void SinglePassCompiler::varDeclaration() {
    uint8_t global = parseVariable("Expect variable name.");

    if (parser.match(TokenType::EQUAL)) {
        expression();
    } else {
        emitByte(OP_NIL);
    }

    parser.consume(TokenType::SEMICOLON,
                   "Expect ';' after variable declaration.");
    defineVariable(global);
}

void SinglePassCompiler::variable(bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

void SinglePassCompiler::statement() {
    if (parser.match(TokenType::PRINT)) {
        printStatement();
    } else {
        expressionStatement();
    }
}

void SinglePassCompiler::expressionStatement() {
    expression();
    parser.consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

void SinglePassCompiler::printStatement() {
    expression();
    parser.consume(TokenType::SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

void SinglePassCompiler::expression() {
    parser.parsePrecedence(Precedence::ASSIGNMENT, *this);
}

void SinglePassCompiler::grouping(bool /*canAssign*/) {
    expression();
    parser.consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

void SinglePassCompiler::unary(bool /*canAssign*/) {
    TokenType operatorType = parser.previous.type;
    parser.parsePrecedence(Precedence::UNARY, *this);
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

void SinglePassCompiler::binary(bool /*canAssign*/) {
    TokenType operatorType = parser.previous.type;
    ParseRule rule = parser.getRule(operatorType);
    auto nextPrec = static_cast<int>(rule.precedence) + 1;
    parser.parsePrecedence(static_cast<Precedence>(nextPrec), *this);

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

void SinglePassCompiler::number(bool /*canAssign*/) {
    Number value = std::strtod(parser.previous.lexeme.data(), nullptr);
    emitConstant(value);
}

void SinglePassCompiler::string(bool /*canAssign*/) {
    emitConstant(ObjString::copy(parser.previous.lexeme));
}

void SinglePassCompiler::literal(bool /*canAssign*/) {
    switch (parser.previous.type) {
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

Chunk *SinglePassCompiler::currentChunk() const { return compilingChunk; }

uint8_t SinglePassCompiler::parseVariable(const char *errorMessage) {
    parser.consume(TokenType::IDENTIFIER, errorMessage);
    return identifierConstant(parser.previous);
}

uint8_t SinglePassCompiler::identifierConstant(const Token &name) {
    return makeConstant(ObjString::copy(name.lexeme));
}

void SinglePassCompiler::defineVariable(uint8_t global) {
    emitBytes(OP_DEFINE_GLOBAL, global);
}

void SinglePassCompiler::namedVariable(const Token &name, bool canAssign) {
    uint8_t arg = identifierConstant(name);
    if (parser.match(TokenType::EQUAL) && canAssign) {
        expression();
        emitBytes(OP_SET_GLOBAL, arg);
    } else {
        emitBytes(OP_GET_GLOBAL, arg);
    }
}

uint8_t SinglePassCompiler::makeConstant(Value value) {
    int constant = currentChunk()->addConstant(value);
    if (constant > UINT8_MAX) {
        parser.error("Too many constants in one chunk.");
        return 0;
    }

    return static_cast<uint8_t>(constant);
}

void SinglePassCompiler::emitByte(uint8_t byte) {
    currentChunk()->write(byte, parser.previous.line);
}

void SinglePassCompiler::emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

void SinglePassCompiler::emitConstant(Value value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

void SinglePassCompiler::emitReturn() { emitByte(OP_RETURN); }

void SinglePassCompiler::endCompiler() {
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

} // namespace Clox
