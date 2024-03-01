#ifndef CLOXPP_CONCATENATOR_H
#define CLOXPP_CONCATENATOR_H

#include <variant>

template <typename T, typename... Args>
struct concatenator;

template <typename... Args0, typename... Args1>
struct concatenator<std::variant<Args0...>, Args1...> {
    using type = std::variant<Args0..., Args1...>;
};

#endif // !CLOXPP_CONCATENATOR_H
