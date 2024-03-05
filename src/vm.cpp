#include "vm.hpp"
#include "debug.hpp" // IWYU pragma: keep

namespace Clox {

VM::VM() { resetStack(); }

VM::~VM() = default;

InterpretResult VM::interpret(Chunk *chunk) {
    this->chunk = chunk;
    this->ip = this->chunk->code;
    return run();
}

InterpretResult VM::run() {
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        disassembleInstruction(chunk, static_cast<int>(ip - chunk->code));
#endif // DEBUG_TRACE_EXECUTION

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
        case OP_CONSTANT: {
            Value constant = READ_CONSTANT();
            push(constant);
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
