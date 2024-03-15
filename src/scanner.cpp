#include <cstring>

#include "scanner.hpp"
#include "token.hpp"

static bool isdigit(char c) { return c >= '0' && c <= '9'; }

static bool isalpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static bool isalphanumeric(char c) { return isdigit(c) || isalpha(c); }

Scanner::Scanner(const std::string &source)
    : source(source), start(source.data()), current(start), line(1) {}

Token Scanner::scanOneToken() {
    skipWhitespace();
    start = current;

    char c = advance();
    if (isalpha(c))
        return consumeIdentifier();
    if (isdigit(c))
        return consumeNumber();

    switch (c) {
    case '(':
        return createToken(TokenType::LEFT_PAREN);
    case ')':
        return createToken(TokenType::RIGHT_PAREN);
    case '{':
        return createToken(TokenType::LEFT_BRACE);
    case '}':
        return createToken(TokenType::RIGHT_BRACE);
    case ',':
        return createToken(TokenType::COMMA);
    case '.':
        return createToken(TokenType::DOT);
    case '-':
        return createToken(TokenType::MINUS);
    case '+':
        return createToken(TokenType::PLUS);
    case ';':
        return createToken(TokenType::SEMICOLON);
    case '*':
        return createToken(TokenType::STAR);
    case '/':
        return createToken(TokenType::SLASH);

    case '!':
        return createToken(match('=') ? TokenType::BANG_EQUAL
                                      : TokenType::BANG);
    case '=':
        return createToken(match('=') ? TokenType::EQUAL_EQUAL
                                      : TokenType::EQUAL);
    case '>':
        return createToken(match('=') ? TokenType::GREATER_EQUAL
                                      : TokenType::GREATER);
    case '<':
        return createToken(match('=') ? TokenType::LESS_EQUAL
                                      : TokenType::LESS);
    case '"':
        return consumeString();
    }

    if (isAtEnd())
        return createToken(TokenType::EOF_);

    return createErrorToken("Unexpected character.");
}

Token Scanner::createToken(TokenType type) {
    const std::string_view lexeme{start,
                                  static_cast<std::size_t>(current - start)};
    return Token(type, lexeme, line);
}

Token Scanner::createToken(TokenType type, std::string_view lexeme) {
    return Token(type, lexeme, line);
}

Token Scanner::createErrorToken(const char *message) {
    return Token(TokenType::ERROR, message, line);
}

void Scanner::skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
        case ' ':
        case '\r':
        case '\t': {
            advance();
            break;
        }
        case '\n': {
            line++;
            advance();
            break;
        }
        case '/': { // handle comment
            if (peekNext() == '/') {
                while (peek() != '\n' && !isAtEnd())
                    advance();
            } else {
                return;
            }
            break;
        }
        default:
            return;
        }
    }
}

Token Scanner::consumeString() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
            line++;
        advance();
    }

    if (isAtEnd()) {
        return createErrorToken("Unterminated string.");
    }

    advance(); // The closing quote
    auto lexeme = std::string_view{
        start + 1, static_cast<std::size_t>(current - start) - 2};
    return createToken(TokenType::STRING, lexeme);
}

Token Scanner::consumeNumber() {
    while (isdigit(peek()))
        advance();

    // Look for fractional part
    if (peek() == '.' && isdigit(peekNext())) {
        advance(); // consume the dot '.'
        while (isdigit(peek()))
            advance();
    }
    return createToken(TokenType::NUMBER);
}

TokenType Scanner::checkKeyword(std::size_t start, std::size_t length,
                                const char *rest, TokenType type) {
    if (static_cast<std::size_t>(this->current - this->start) ==
            start + length &&
        std::memcmp(this->start + start, rest, length) == 0) {
        return type;
    }

    return TokenType::IDENTIFIER;
}

TokenType Scanner::identifierType() {
    switch (*start) {
    case 'a':
        return checkKeyword(1, 2, "nd", TokenType::AND);
    case 'c':
        return checkKeyword(1, 4, "lass", TokenType::CLASS);
    case 'e':
        return checkKeyword(1, 3, "lse", TokenType::ELSE);
    case 'f': {
        if (static_cast<std::size_t>(current - start) > 1) {
            switch (*(start + 1)) {
            case 'a':
                return checkKeyword(2, 3, "lse", TokenType::FALSE);
            case 'o':
                return checkKeyword(2, 1, "r", TokenType::FOR);
            case 'u':
                return checkKeyword(2, 1, "n", TokenType::FUN);
            }
        }
        break;
    }
    case 'i':
        return checkKeyword(1, 1, "f", TokenType::IF);
    case 'n':
        return checkKeyword(1, 2, "il", TokenType::NIL);
    case 'o':
        return checkKeyword(1, 1, "r", TokenType::OR);
    case 'p':
        return checkKeyword(1, 4, "rint", TokenType::PRINT);
    case 'r':
        return checkKeyword(1, 5, "eturn", TokenType::RETURN);
    case 's':
        return checkKeyword(1, 4, "uper", TokenType::SUPER);
    case 't': {
        if (static_cast<std::size_t>(current - start) > 1) {
            switch (*(start + 1)) {
            case 'h':
                return checkKeyword(2, 2, "is", TokenType::THIS);
            case 'r':
                return checkKeyword(2, 2, "ue", TokenType::TRUE);
            }
        }
        break;
    }
    case 'v':
        return checkKeyword(1, 2, "ar", TokenType::VAR);
    case 'w':
        return checkKeyword(1, 4, "hile", TokenType::WHILE);
    }

    return TokenType::IDENTIFIER;
}

Token Scanner::consumeIdentifier() {
    while (isalphanumeric(peek()))
        advance();

    return createToken(identifierType());
}

bool Scanner::isAtEnd() { return *current == '\0'; }

char Scanner::advance() {
    if (isAtEnd()) {
        // must have this if clause,
        // else scanner cannot reach EOF and create EOF token
        // I have no idea why...
        return '\0';
    }
    current++;
    return *(current - 1);
}

bool Scanner::match(char expected) {
    if (isAtEnd()) {
        return false;
    }
    if (*current != expected) {
        return false;
    }

    current++;
    return true;
}

char Scanner::peek() {
    if (isAtEnd())
        return '\0';
    return *current;
}

char Scanner::peekNext() {
    if (isAtEnd())
        return '\0';

    return *(current + 1);
}
