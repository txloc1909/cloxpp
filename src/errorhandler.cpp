#include <iostream>

#include "errorhandler.hpp"


void ErrorHandler::report(int line, const char* where, const char* message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
    hadError = true;
}

void ErrorHandler::error(int line, const char* message) {
    report(line, "", message);
}
