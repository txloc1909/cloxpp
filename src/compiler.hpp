#ifndef CLOXPP_COMPILER_H
#define CLOXPP_COMPILER_H

#include <cstdint>
#include <functional>
#include <string>

#include "object.hpp"
#include "scanner.hpp"
#include "value.hpp"

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

class Compiler;
using ParseFn = std::function<void(Compiler &, bool)>;

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

    PrattParser(const std::string &source);

    void errorAt(const Token &token, const char *message);
    void errorAtCurrent(const char *message);
    void error(const char *message);

    void advance();
    void consume(TokenType type, const char *message);
    bool check(TokenType type) const;
    bool match(TokenType type);
    void synchronize();

    void parsePrecedence(Precedence precedence, Compiler &compiler);

    static std::unordered_map<TokenType, ParseRule> rules;
    static ParseRule getRule(TokenType type);
};
using Parser = PrattParser;

struct Local {
    Token name;
    int depth;
};

class Compiler {
public:
    Compiler(FunctionType type);

    void variable(bool canAssign);
    void string(bool canAssign);
    void number(bool canAssign);
    void literal(bool canAssign);
    void grouping(bool canAssign);
    void unary(bool canAssign);
    void binary(bool canAssign);
    void andOp(bool canAssign);
    void orOp(bool canAssign);

private:
    Chunk *currentChunk() const;

    void declaration();
    void varDeclaration();
    void block();
    void statement();
    void printStatement();
    void ifStatement();
    void whileStatement();
    void forStatement();
    void expressionStatement();
    void expression();

    void beginScope();
    void endScope();

    int resolveLocal(const Token &name);
    void addLocal(const Token &name);
    void markInitialized();

    uint8_t parseVariable(const char *errorMessage);
    uint8_t identifierConstant(const Token &name);
    void declareVariable();
    void defineVariable(uint8_t global);
    void namedVariable(const Token &name, bool canAssign);

    uint8_t makeConstant(Value value);
    void emitByte(uint8_t byte);
    void emitBytes(uint8_t byte1, uint8_t byte2);
    void emitConstant(Value value);
    int emitJump(uint8_t instruction);
    void patchJump(int offset);
    void emitLoop(int loopStart);
    void emitReturn();
    ObjFunction *endCompiler();

    ObjFunction *compilingFunction;
    FunctionType type;
    Local locals[UINT8_COUNT];
    int localCount;
    int scopeDepth;
    Parser *parser;

    friend class SinglePassCompiler;
};

class SinglePassCompiler {
public:
    static ObjFunction *compile(const std::string &source);

private:
    static std::unique_ptr<Parser> parser;
    static std::unique_ptr<Compiler> current;
};

} // namespace Clox

#endif // !CLOXPP_COMPILER_H
