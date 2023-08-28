#include <iostream>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "scanner.hpp"


Scanner::Scanner(std::string source_) {
    this->source = std::make_unique<std::string>(source_);
}

std::vector<std::string> Scanner::scanTokens() {
    auto tokens = std::vector<std::string>();
    const char* delim = " ";
    char* c_src = (char*) malloc(this->source->size() * sizeof(char));
    std::strncpy(c_src, this->source->c_str(), this->source->size());
    char* token = std::strtok(c_src, delim);

    while (token) {
        tokens.push_back(token);
        token = std::strtok(nullptr, delim);
    }

    free(c_src);
    return tokens;
}
