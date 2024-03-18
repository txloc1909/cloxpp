#include <cstdio>

#include "compiler.hpp"

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

    return !parser.hadError;
}

void SinglePassCompiler::expression() {}
void SinglePassCompiler::grouping() {}
void SinglePassCompiler::binary() {}
void SinglePassCompiler::unary() {}
void SinglePassCompiler::number() {}

Chunk *SinglePassCompiler::currentChunk() const { return compilingChunk; }

void SinglePassCompiler::emitByte(uint8_t byte) {
    currentChunk()->write(byte, parser.previous.line);
}

void SinglePassCompiler::emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

void SinglePassCompiler::emitReturn() { emitByte(OP_RETURN); }

void SinglePassCompiler::endCompiler() { emitReturn(); }

} // namespace Clox
