#ifndef CLOXPP_COMPILER_H
#define CLOXPP_COMPILER_H

#include <functional>
#include <string>

#include "chunk.hpp"
#include "scanner.hpp"

namespace Clox {

enum class Precedence {
    NONE,
    ASSIGNMENT, // =
    OR,         // or
    AND,        // and
    EQUALITY,   // == !=
    COMPARISON, // < > <= >=
    TERM,       // + -
    FACTOR,     // * /
    UNARY,      // ! -
    CALL,       // . ()
    PRIMARY,
};

class SinglePassCompiler;
using ParseFn = std::function<void(SinglePassCompiler &)>;

struct ParseRule {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;

    ParseRule()
        : prefix(nullptr), infix(nullptr), precedence(Precedence::NONE) {}
    ParseRule(ParseFn prefix, ParseFn infix, Precedence prec)
        : prefix(prefix), infix(infix), precedence(prec) {}
};

struct PrattParser {
    Scanner scanner;
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
    std::unordered_map<TokenType, ParseRule> rules;

    PrattParser(const std::string &source)
        : scanner(Scanner(source)), current(scanner.scanOneToken()),
          previous(current), hadError(false), panicMode(false), rules({}){};

    void errorAt(const Token &token, const char *message);
    void errorAtCurrent(const char *message);
    void error(const char *message);

    void advance();
    void consume(TokenType type, const char *message);
    bool check(TokenType type) const;
    bool match(TokenType type);

    void registerParseRule(TokenType type, ParseFn prefix, ParseFn infix,
                           Precedence prec);
    ParseRule &getRule(TokenType type);

    void parsePrecedence(Precedence precedence, SinglePassCompiler &compiler);
};
using Parser = PrattParser;

class SinglePassCompiler {
public:
    SinglePassCompiler(const std::string &source);
    bool compile(Chunk *chunk);

    void declaration();
    void statement();
    void printStatement();
    void expressionStatement();
    void expression();
    void string();
    void number();
    void literal();
    void grouping();
    void unary();
    void binary();

private:
    Parser parser;
    Chunk *compilingChunk;

    Chunk *currentChunk() const;

    uint8_t makeConstant(Value value);
    void emitByte(uint8_t byte);
    void emitBytes(uint8_t byte1, uint8_t byte2);
    void emitConstant(Value value);
    void emitReturn();
    void endCompiler();
};

} // namespace Clox

#endif // !CLOXPP_COMPILER_H
