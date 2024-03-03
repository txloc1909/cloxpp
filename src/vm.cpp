#include "vm.hpp"

namespace Clox {

VM::VM() = default;

VM::~VM() = default;

InterpreteResult VM::interpret(Chunk *chunk) {
    this->chunk = chunk;
    this->ip = this->chunk->code;
    return run();
}

InterpreteResult VM::run() {
#define READ_BYTE() (*this->ip++)
#define READ_CONSTANT() (this->chunk->constants.values[READ_BYTE()])

    for (;;) {
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
