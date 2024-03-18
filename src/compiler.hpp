#ifndef CLOXPP_COMPILER_H
#define CLOXPP_COMPILER_H

#include <string>

#include "chunk.hpp"
#include "scanner.hpp"

namespace Clox {

struct PrattParser {
    Scanner scanner;
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;

    PrattParser(const std::string &source)
        : scanner(Scanner(source)), current(scanner.scanOneToken()),
          previous(current), hadError(false), panicMode(false) {}

    void errorAt(const Token &token, const char *message);
    void errorAtCurrent(const char *message);
    void error(const char *message);

    void advance();
    void consume(TokenType type, const char *message);
    bool check(TokenType type);
    bool match(TokenType type);
};
using Parser = PrattParser;

class SinglePassCompiler {
public:
    SinglePassCompiler(const std::string &source) : parser(Parser(source)){};
    bool compile(Chunk *chunk);

private:
    Parser parser;
    Chunk *compilingChunk;

    Chunk *currentChunk() const;

    void expression();

    void emitByte(uint8_t byte);
    void emitBytes(uint8_t byte1, uint8_t byte2);
    void emitReturn();
    void endCompiler();
};

} // namespace Clox

#endif // !CLOXPP_COMPILER_H
