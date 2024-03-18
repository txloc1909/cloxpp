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
    double value = std::strtod(parser.previous.lexeme.data(), nullptr);
    emitConstant(value);
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
