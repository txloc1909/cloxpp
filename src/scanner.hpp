#ifndef CLOXPP_SCANNER_H
#define CLOXPP_SCANNER_H

#include <memory>
#include <vector>

class Scanner {
    private:
        std::unique_ptr<std::string> source;

    public:
        Scanner(std::string source_);
        std::vector<std::string> scanTokens();
};

#endif // !CLOXPP_SCANNER_H
