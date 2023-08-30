cmake_minimum_required(VERSION 3.27)
project(cloxpp)

add_executable(jlox
    src/main.cpp
    src/treewalk_interpreter.cpp
    src/error_handler.cpp
    src/scanner.cpp
    src/token.cpp
    src/utils.cpp
)
target_compile_options(jlox PRIVATE -Wall -Wextra)
target_compile_features(jlox PRIVATE cxx_std_17)
