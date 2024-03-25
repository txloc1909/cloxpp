#include <cstdio>
#include <string>

#include "compiler.hpp"
#include "debug.hpp" // IWYU pragma: keep

namespace Clox {

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

void Parser::registerParseRule(TokenType type, ParseFn prefix, ParseFn infix,
                               Precedence prec) {
    rules[type] = {prefix, infix, prec};
}

ParseRule &Parser::getRule(TokenType type) { return rules[type]; }

void Parser::parsePrecedence(Precedence precedence,
                             SinglePassCompiler &compiler) {
    advance();
    ParseFn prefixRule = getRule(previous.type).prefix;
    if (!prefixRule) {
        error("Expect expression.");
        return;
    }
    prefixRule(compiler);
    while (precedence <= getRule(current.type).precedence) {
        advance();
        ParseFn infixRule = getRule(previous.type).infix;
        infixRule(compiler);
    }
}

SinglePassCompiler::SinglePassCompiler(const std::string &source)
    : parser(Parser(source)) {
    // register parse rule here:
    parser.registerParseRule(TokenType::LEFT_PAREN,
                             &SinglePassCompiler::grouping, nullptr,
                             Precedence::NONE);
    parser.registerParseRule(TokenType::MINUS, &SinglePassCompiler::unary,
                             &SinglePassCompiler::binary, Precedence::TERM);
    parser.registerParseRule(TokenType::PLUS, nullptr,
                             &SinglePassCompiler::binary, Precedence::TERM);
    parser.registerParseRule(TokenType::SLASH, nullptr,
                             &SinglePassCompiler::binary, Precedence::FACTOR);
    parser.registerParseRule(TokenType::STAR, nullptr,
                             &SinglePassCompiler::binary, Precedence::FACTOR);
    parser.registerParseRule(TokenType::NUMBER, &SinglePassCompiler::number,
                             nullptr, Precedence::NONE);
    parser.registerParseRule(TokenType::STRING, &SinglePassCompiler::string,
                             nullptr, Precedence::NONE);
    parser.registerParseRule(TokenType::NIL, &SinglePassCompiler::literal,
                             nullptr, Precedence::NONE);
    parser.registerParseRule(TokenType::TRUE, &SinglePassCompiler::literal,
                             nullptr, Precedence::NONE);
    parser.registerParseRule(TokenType::FALSE, &SinglePassCompiler::literal,
                             nullptr, Precedence::NONE);
    parser.registerParseRule(TokenType::BANG, &SinglePassCompiler::unary,
                             nullptr, Precedence::NONE);
    parser.registerParseRule(TokenType::BANG_EQUAL, nullptr,
                             &SinglePassCompiler::binary, Precedence::EQUALITY);
    parser.registerParseRule(TokenType::EQUAL_EQUAL, nullptr,
                             &SinglePassCompiler::binary, Precedence::EQUALITY);
    parser.registerParseRule(TokenType::GREATER, nullptr,
                             &SinglePassCompiler::binary,
                             Precedence::COMPARISON);
    parser.registerParseRule(TokenType::GREATER_EQUAL, nullptr,
                             &SinglePassCompiler::binary,
                             Precedence::COMPARISON);
    parser.registerParseRule(TokenType::LESS, nullptr,
                             &SinglePassCompiler::binary,
                             Precedence::COMPARISON);
    parser.registerParseRule(TokenType::LESS_EQUAL, nullptr,
                             &SinglePassCompiler::binary,
                             Precedence::COMPARISON);
}

bool SinglePassCompiler::compile(Chunk *chunk) {
    compilingChunk = chunk;

    expression();
    parser.consume(TokenType::EOF_, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}

void SinglePassCompiler::expression() {
    parser.parsePrecedence(Precedence::ASSIGNMENT, *this);
}

void SinglePassCompiler::grouping() {
    expression();
    parser.consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

void SinglePassCompiler::unary() {
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

void SinglePassCompiler::binary() {
    TokenType operatorType = parser.previous.type;
    ParseRule &rule = parser.getRule(operatorType);
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

void SinglePassCompiler::number() {
    Number value = std::strtod(parser.previous.lexeme.data(), nullptr);
    emitConstant(value);
}

void SinglePassCompiler::string() {
    emitConstant(new ObjString(parser.previous.lexeme)); // currently leaking
}

void SinglePassCompiler::literal() {
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
