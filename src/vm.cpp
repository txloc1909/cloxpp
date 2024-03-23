#include <cstdarg>
#include <cstdio>
#include <iostream>

#include "compiler.hpp"
#include "debug.hpp" // IWYU pragma: keep
#include "utils.hpp"
#include "vm.hpp"

namespace Clox {

static bool isFalsey(Value value) {
    return std::holds_alternative<Nil>(value) ||
           (std::holds_alternative<bool>(value) && !std::get<bool>(value));
}

struct ValueEquality {
    bool operator()(Nil, Nil) const { return true; }

    template <typename T>
    bool operator()(T a, T b) const {
        return a == b;
    }

    template <typename T, typename U,
              typename = std::enable_if_t<!std::is_same_v<T, U>>>
    bool operator()(T /*a*/, U /*b*/) const {
        return false;
    }
};

static bool valuesEqual(Value a, Value b) {
    return std::visit(ValueEquality(), a, b);
}

VM::VM() { resetStack(); }

VM::~VM() = default;

void VM::runFile(const char *path) {
    auto result = interpret(readFile(path));

    if (result == InterpretResult::COMPILE_ERROR)
        std::exit(65);
    if (result == InterpretResult::RUNTIME_ERROR)
        std::exit(70);
}

void VM::repl() {
    std::string line;
    while (std::cin) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        interpret(line);
    }
}

InterpretResult VM::interpret(const std::string &source) {
    Chunk newChunk{};
    if (!SinglePassCompiler(source).compile(&newChunk)) {
        return InterpretResult::COMPILE_ERROR;
    }

    chunk = &newChunk;
    ip = chunk->code;
    return run();
}

InterpretResult VM::run() {
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op)                                               \
    do {                                                                       \
        if (!std::holds_alternative<double>(peek(0)) ||                        \
            !std::holds_alternative<double>(peek(1))) {                        \
            runtimeError("Operands must be numbers.");                         \
        }                                                                      \
        double b = std::get<double>(pop());                                    \
        double a = std::get<double>(pop());                                    \
        push(static_cast<valueType>(a op b));                                  \
    } while (false)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        std::printf("          ");
        for (Value *slot = stack; slot < stackTop; slot++) {
            std::cout << "[ " << *slot << " ]";
        }
        std::cout << "\n";
        disassembleInstruction(chunk, static_cast<int>(ip - chunk->code));
#endif // DEBUG_TRACE_EXECUTION

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
        case OP_CONSTANT: {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OP_NIL: {
            push(Nil{});
            break;
        }
        case OP_TRUE: {
            push(true);
            break;
        }
        case OP_FALSE: {
            push(false);
            break;
        }
        case OP_EQUAL: {
            Value b = pop();
            Value a = pop();
            push(valuesEqual(a, b));
            break;
        }
        case OP_GREATER: {
            BINARY_OP(bool, >);
            break;
        }
        case OP_LESS: {
            BINARY_OP(bool, <);
            break;
        }
        case OP_ADD: {
            if (std::holds_alternative<ObjString *>(peek(0)) &&
                std::holds_alternative<ObjString *>(peek(1))) {
                ObjString *b = std::get<ObjString *>(pop());
                ObjString *a = std::get<ObjString *>(pop());
                ObjString *result =
                    new ObjString(ObjString::concatenate(*a, *b)); // leak
                push(result);
            } else if (std::holds_alternative<double>(peek(0)) &&
                       std::holds_alternative<double>(peek(1))) {
                double b = std::get<double>(pop());
                double a = std::get<double>(pop());
                push(a + b);
            } else {
                runtimeError("Operands must be two numbers or two strings.");
                return InterpretResult::RUNTIME_ERROR;
            }
            break;
        }
        case OP_SUBTRACT: {
            BINARY_OP(double, -);
            break;
        }
        case OP_MULTIPLY: {
            BINARY_OP(double, *);
            break;
        }
        case OP_DIVIDE: {
            BINARY_OP(double, /);
            break;
        }
        case OP_NOT: {
            push(isFalsey(pop()));
            break;
        }
        case OP_NEGATE: {
            if (!std::holds_alternative<double>(peek(0))) {
                runtimeError("Operand must be a number.");
                return InterpretResult::RUNTIME_ERROR;
            }
            push(-std::get<double>(pop()));
            break;
        }
        case OP_RETURN: {
            std::cout << pop() << "\n";
            return InterpretResult::OK;
        }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

void VM::push(Value value) {
    *stackTop = value;
    stackTop++;
}

Value VM::pop() {
    stackTop--;
    return *stackTop;
}

Value VM::peek(int distance) { return *(stackTop - 1 - distance); }

void VM::resetStack() { stackTop = stack; }

void VM::runtimeError(const char *format, ...) {
    std::va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);
    std::fputs("\n", stderr);

    std::size_t instruction = ip - chunk->code - 1;
    int line = chunk->lines[instruction];
    std::fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

} // namespace Clox
