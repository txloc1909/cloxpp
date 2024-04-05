#include <cstdarg>
#include <cstdio>
#include <iostream>

#include "compiler.hpp"
#include "debug.hpp" // IWYU pragma: keep
#include "memory.hpp"
#include "vm.hpp"

namespace Clox {

VM::VM() { resetStack(); }

VM::~VM() { Allocator::cleanUp(); }

InterpretResult VM::interpret(const std::string &source) {
    Chunk newChunk{};
    if (!SinglePassCompiler::compile(source, &newChunk)) {
        return InterpretResult::COMPILE_ERROR;
    }

    chunk = &newChunk;
    ip = chunk->code;
    return run();
}

InterpretResult VM::run() {
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
#define READ_STRING() (READ_CONSTANT().asType<ObjString *>())
#define READ_SHORT() (ip += 2, static_cast<uint16_t>((ip[-2] << 8) | ip[-1]))
#define BINARY_OP(valueType, op)                                               \
    do {                                                                       \
        if (!peek(0).isType<Number>() || !peek(1).isType<Number>()) {          \
            runtimeError("Operands must be numbers.");                         \
        }                                                                      \
        auto b = pop().asType<Number>();                                       \
        auto a = pop().asType<Number>();                                       \
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
        case OP_POP: {
            pop();
            break;
        }
        case OP_GET_LOCAL: {
            uint8_t slot = READ_BYTE();
            push(stack[slot]);
            break;
        }
        case OP_SET_LOCAL: {
            uint8_t slot = READ_BYTE();
            stack[slot] = peek(0);
            break;
        }
        case OP_GET_GLOBAL: {
            ObjString *name = READ_STRING();
            auto maybeVal = globals.get(name);
            if (!maybeVal.has_value()) {
                runtimeError("Undefined variable '%s'", name->data());
                return InterpretResult::RUNTIME_ERROR;
            }
            push(maybeVal.value());
            break;
        }
        case OP_DEFINE_GLOBAL: {
            ObjString *name = READ_STRING();
            globals.set(name, peek(0));
            pop();
            break;
        }
        case OP_SET_GLOBAL: {
            ObjString *name = READ_STRING();
            if (globals.set(name, peek(0))) {
                globals.deleteKey(name);
                runtimeError("Undefined variable '%s'.", name->data());
                return InterpretResult::RUNTIME_ERROR;
            }

            break;
        }
        case OP_EQUAL: {
            Value b = pop();
            Value a = pop();
            push(a == b);
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
            if (peek(0).isType<ObjString *>() &&
                peek(1).isType<ObjString *>()) {
                ObjString *b = pop().asType<ObjString *>();
                ObjString *a = pop().asType<ObjString *>();
                ObjString *result = ObjString::concatenate(*a, *b);
                push(result);
            } else if (peek(0).isType<Number>() && peek(1).isType<Number>()) {
                auto b = pop().asType<Number>();
                auto a = pop().asType<Number>();
                push(a + b);
            } else {
                runtimeError("Operands must be two numbers or two strings.");
                return InterpretResult::RUNTIME_ERROR;
            }
            break;
        }
        case OP_SUBTRACT: {
            BINARY_OP(Number, -);
            break;
        }
        case OP_MULTIPLY: {
            BINARY_OP(Number, *);
            break;
        }
        case OP_DIVIDE: {
            BINARY_OP(Number, /);
            break;
        }
        case OP_NOT: {
            push(pop().isFalsey());
            break;
        }
        case OP_NEGATE: {
            if (!peek(0).isType<Number>()) {
                runtimeError("Operand must be a number.");
                return InterpretResult::RUNTIME_ERROR;
            }
            push(-pop().asType<Number>());
            break;
        }
        case OP_PRINT: {
            std::cout << pop() << "\n";
            break;
        }
        case OP_JUMP: {
            uint16_t offset = READ_SHORT();
            ip += offset;
            break;
        }
        case OP_JUMP_IF_FALSE: {
            uint64_t offset = READ_SHORT();
            if (peek(0).isFalsey())
                ip += offset;
            break;
        }
        case OP_RETURN: {
            return InterpretResult::OK;
        }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_STRING
#undef READ_SHORT
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
