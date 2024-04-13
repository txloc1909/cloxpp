#ifndef CLOXPP_VM_H
#define CLOXPP_VM_H

#include "object.hpp"
#include "table.hpp"

namespace Clox {

enum class InterpretResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR,
};

struct CallFrame {
    ObjClosure *closure;
    uint8_t *ip;
    Value *slots;
};

struct Caller;
class VM {
public:
    VM();
    ~VM();
    InterpretResult interpret(const std::string &source);

private:
    static constexpr std::size_t FRAMES_MAX = 64;
    static constexpr std::size_t STACK_MAX = FRAMES_MAX * UINT8_COUNT;
    CallFrame frames[FRAMES_MAX];
    int frameCount;
    Value stack[STACK_MAX];
    Value *stackTop;
    Table globals;

    InterpretResult run();

    bool callValue(Value callee, int argCount);
    bool call(ObjClosure *closure, int argCount);

    void push(Value value);
    Value pop();
    Value peek(int distance);

    void resetStack();
    void runtimeError(const char *format, ...);

    void defineNative(std::string_view name, NativeFn function);

    friend struct Caller;
};

} // namespace Clox

#endif // !CLOXPP_VM_H
