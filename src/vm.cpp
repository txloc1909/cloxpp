#include "vm.hpp"
#include "debug.hpp" // IWYU pragma: keep

namespace Clox {

VM::VM() = default;

VM::~VM() = default;

InterpreteResult VM::interpret(Chunk *chunk) {
    this->chunk = chunk;
    this->ip = this->chunk->code;
    return run();
}

InterpreteResult VM::run() {
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
            std::printf("%g\n", constant); // temporarily
            break;
        }
        case OP_RETURN: {
            return InterpreteResult::OK;
        }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}

} // namespace Clox
