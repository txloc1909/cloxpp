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
using ParseFn = std::function<void(SinglePassCompiler &, bool)>;

struct ParseRule {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};

struct PrattParser {
    Scanner scanner;
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
    std::unordered_map<TokenType, ParseRule> rules;

    PrattParser(const std::string &source);

    void errorAt(const Token &token, const char *message);
    void errorAtCurrent(const char *message);
    void error(const char *message);

    void advance();
    void consume(TokenType type, const char *message);
    bool check(TokenType type) const;
    bool match(TokenType type);
    void synchronize();

    void registerParseRule(TokenType type, ParseFn prefix, ParseFn infix,
                           Precedence prec);
    ParseRule getRule(TokenType type) const;

    void parsePrecedence(Precedence precedence, SinglePassCompiler &compiler);
};
using Parser = PrattParser;

class SinglePassCompiler {
public:
    SinglePassCompiler(const std::string &source);
    bool compile(Chunk *chunk);

    void variable(bool canAssign);
    void string(bool canAssign);
    void number(bool canAssign);
    void literal(bool canAssign);
    void grouping(bool canAssign);
    void unary(bool canAssign);
    void binary(bool canAssign);

private:
    Parser parser;
    Chunk *compilingChunk;

    Chunk *currentChunk() const;

    void declaration();
    void varDeclaration();
    void statement();
    void printStatement();
    void expressionStatement();
    void expression();

    uint8_t parseVariable(const char *errorMessage);
    uint8_t identifierConstant(const Token &name);
    void defineVariable(uint8_t global);
    void namedVariable(const Token &name, bool canAssign);

    uint8_t makeConstant(Value value);
    void emitByte(uint8_t byte);
    void emitBytes(uint8_t byte1, uint8_t byte2);
    void emitConstant(Value value);
    void emitReturn();
    void endCompiler();
};

} // namespace Clox

#endif // !CLOXPP_COMPILER_H
