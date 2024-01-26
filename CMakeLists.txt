cmake_minimum_required(VERSION 3.27)
project(cloxpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

add_executable(jlox
    src/main.cpp
    src/treewalk_interpreter.cpp
    src/error_handler.cpp
    src/scanner.cpp
    src/token.cpp
    src/utils.cpp
    src/parser.cpp
    src/interpreter.cpp
)
target_compile_options(jlox PRIVATE -Wall -Wextra -fsanitize=undefined -fsanitize=address)
target_link_options(jlox PRIVATE -fsanitize=undefined -fsanitize=address)

add_executable(scanner
    src/main_scanner.cpp
    src/scanner.cpp
    src/error_handler.cpp
    src/token.cpp
    src/utils.cpp
)
target_compile_options(scanner PRIVATE -Wall -Wextra -fsanitize=undefined -fsanitize=address)
target_link_options(scanner PRIVATE -fsanitize=undefined -fsanitize=address)
