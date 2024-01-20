#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "error_handler.hpp"
#include "scanner.hpp"
#include "token.hpp"

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"and", TokenType::AND},       {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
    {"for", TokenType::FOR},       {"fun", TokenType::FUN},
    {"if", TokenType::IF},         {"nil", TokenType::NIL},
    {"or", TokenType::OR},         {"print", TokenType::PRINT},
    {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS},     {"true", TokenType::TRUE},
    {"var", TokenType::VAR},       {"while", TokenType::WHILE},
};

static bool isdigit(char c) { return c >= '0' && c <= '9'; }

static bool isalpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static bool isalphanumeric(char c) { return isdigit(c) || isalpha(c); }

Scanner::Scanner(std::string source_, ErrorHandler *errorHandler_) {
    errorHandler = errorHandler_;
    source = std::make_unique<std::string>(source_);
    start = 0;
    current = 0;
    line = 1;
}

std::vector<Token> Scanner::scanTokens() {
    auto tokens = std::vector<Token>();
    while (!isAtEnd()) {
        start = current;
        auto tok = scanOneToken();
        if (tok) {
            tokens.push_back(tok.value());
        }
    }

    tokens.push_back(Token(TokenType::EOF_, "", {}, this->line));
    return tokens;
}

std::optional<Token> Scanner::scanOneToken() {
    char c = advance();
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

    case '/': {
        if (match('/')) {
            while (peek() != '\n' && !isAtEnd())
                advance();
        } else {
            return createToken(TokenType::SLASH);
        }
        break;
    }
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n': {
        line++;
        break;
    }

    case '"':
        return consumeString();

    default: {
        if (isdigit(c)) {
            return consumeNumber();
        } else if (isalpha(c)) {
            return consumeIdentifier();
        } else {
            errorHandler->error(line, "Unexpected character.");
            return std::nullopt;
        }
    }
    }

    return std::nullopt;
}

Token Scanner::createToken(TokenType type) { return createToken(type, {}); }

Token Scanner::createToken(TokenType type, Literal literal) {
    const auto lexeme = source->substr(start, current - start);
    return Token(type, lexeme, literal, line);
}

std::optional<Token> Scanner::consumeString() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
            line++;
        advance();
    }

    if (isAtEnd()) {
        errorHandler->error(line, "Unterminated string.");
        return std::nullopt;
    }

    advance(); // The closing quote

    auto string = source->substr(start + 1, current - start - 2);
    return createToken(TokenType::STRING, string);
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

    double value = std::stod(source->substr(start, current - start));
    return createToken(TokenType::NUMBER, value);
}

Token Scanner::consumeIdentifier() {
    while (isalphanumeric(peek()))
        advance();

    auto text = source->substr(start, current - start);

    TokenType type;
    if (KEYWORDS.find(text) == KEYWORDS.end()) {
        type = TokenType::IDENTIFIER;
    } else {
        type = KEYWORDS.at(text);
    }

    return createToken(type);
}

bool Scanner::isAtEnd() { return current >= source->size(); }

char Scanner::advance() { return source->at(current++); }

bool Scanner::match(char expected) {
    if (isAtEnd()) {
        return false;
    }
    if (source->at(current) != expected) {
        return false;
    }

    current++;
    return true;
}

char Scanner::peek() {
    if (isAtEnd())
        return '\0';
    return source->at(current);
}

char Scanner::peekNext() {
    if (current + 1 >= source->size())
        return '\0';
    return source->at(current + 1);
}
