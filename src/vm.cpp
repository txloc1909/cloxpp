#include <iostream>

#include "compiler.hpp"
#include "debug.hpp" // IWYU pragma: keep
#include "utils.hpp"
#include "vm.hpp"

namespace Clox {

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
#define BINARY_OP(op)                                                          \
    do {                                                                       \
        double b = pop();                                                      \
        double a = pop();                                                      \
        push(a op b);                                                          \
    } while (false)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        std::printf("          ");
        for (Value *slot = stack; slot < stackTop; slot++) {
            std::printf("[ ");
            std::printf("%g", *slot); // temporarily
            std::printf(" ]");
        }
        std::printf("\n");
        disassembleInstruction(chunk, static_cast<int>(ip - chunk->code));
#endif // DEBUG_TRACE_EXECUTION

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
        case OP_CONSTANT: {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OP_ADD: {
            BINARY_OP(+);
            break;
        }
        case OP_SUBTRACT: {
            BINARY_OP(-);
            break;
        }
        case OP_MULTIPLY: {
            BINARY_OP(*);
            break;
        }
        case OP_DIVIDE: {
            BINARY_OP(/);
            break;
        }
        case OP_NEGATE: {
            push(-pop());
            break;
        }
        case OP_RETURN: {
            std::printf("%g\n", pop());
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

void VM::resetStack() { stackTop = stack; }

} // namespace Clox
