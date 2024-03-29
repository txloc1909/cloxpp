cmake_minimum_required(VERSION 3.27)
project(cloxpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

add_executable(jlox
    src/main_jlox.cpp
    src/treewalk_interpreter.cpp
    src/error_handler.cpp
    src/value.cpp
    src/scanner.cpp
    src/token.cpp
    src/utils.cpp
    src/parser.cpp
    src/interpreter.cpp
    src/environment.cpp
    src/lox_function.cpp
    src/resolver.cpp
    src/lox_class.cpp

    # TODO: unnecessary dependencies; refactor this
    src/memory.cpp
    src/object.cpp
)
target_compile_options(jlox PRIVATE -Wall -Wextra)
target_link_options(jlox PRIVATE)

add_executable(scanner
    src/main_scanner.cpp
    src/scanner.cpp
    src/error_handler.cpp
    src/token.cpp
    src/utils.cpp
)
target_compile_options(scanner PRIVATE -Wall -Wextra -fsanitize=undefined -fsanitize=address)
target_link_options(scanner PRIVATE -fsanitize=undefined -fsanitize=address)

add_executable(clox
    src/main_clox.cpp
    src/stack_vm.cpp
    src/chunk.cpp
    src/memory.cpp
    src/debug.cpp
    src/value.cpp
    src/utils.cpp
    src/vm.cpp
    src/token.cpp
    src/scanner.cpp
    src/compiler.cpp
    src/object.cpp
    src/table.cpp

    # TODO: unnecessary dependencies; refactor this
    src/lox_class.cpp
    src/lox_function.cpp
    src/environment.cpp
    src/interpreter.cpp
    src/error_handler.cpp
)
target_compile_options(clox PRIVATE -DDEBUG_TRACE_EXECUTION -DDEBUG_PRINT_CODE -Wall -Wextra
    -fsanitize=undefined -fsanitize=address)
target_link_options(clox PRIVATE -fsanitize=undefined -fsanitize=address)
