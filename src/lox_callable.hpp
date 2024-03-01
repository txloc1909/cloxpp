#ifndef CLOXPP_LOX_CALLABLE_H
#define CLOXPP_LOX_CALLABLE_H

#include "interpreter.hpp"

namespace Jlox {

class LoxCallable {
public:
    virtual Value call(Interpreter &interpreter,
                       const std::vector<Value> &arguments) = 0;
    virtual std::size_t arity() const = 0;
    virtual std::string toString() const = 0;

    virtual ~LoxCallable() = default;
};

} // namespace Jlox
#endif // !CLOXPP_LOX_CALLABLE_H
