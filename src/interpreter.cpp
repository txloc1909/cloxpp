#include "interpreter.hpp"
#include "runtime_error.hpp"
#include "token.hpp"
#include <iostream>
#include <variant>

static bool isTruthy(LoxValue value) {
    if (std::holds_alternative<std::monostate>(value))
        return false;

    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value);

    return true;
}

static bool isEqual(LoxValue val1, LoxValue val2) {
    if (std::holds_alternative<std::monostate>(val1) &&
        std::holds_alternative<std::monostate>(val2))
        return true;

    if (std::holds_alternative<std::monostate>(val1))
        return false;

    return val1.index() == val2.index() ? val1 == val2 : false;
}

static void checkNumberOperand(Token op, LoxValue operand) {
    if (std::holds_alternative<double>(operand))
        return;

    throw RuntimeError(op, "Operand must be a number.");
}

static void checkNumberOperands(Token op, LoxValue left, LoxValue right) {
    if (std::holds_alternative<double>(left) &&
        std::holds_alternative<double>(right))
        return;

    throw RuntimeError(op, "Operands must be a number.");
}

void Interpreter::interpret(Expr::ExprPtr expr) {
    try {
        std::cout << evaluate(*expr) << "\n";
    } catch (const RuntimeError &error) {
        handler_->runtimeError(error);
    }
}

LoxValue Interpreter::evaluate(const Expr::BaseExpr &expr) {
    return expr.accept(*this);
}

LoxValue Interpreter::visitBinary(const Expr::Binary &expr) {
    auto left = evaluate(*expr.left_);
    auto right = evaluate(*expr.right_);

    switch (expr.op_.type) {
    case TokenType::GREATER:
        checkNumberOperands(expr.op_, left, right);
        return std::get<double>(left) > std::get<double>(right);
    case TokenType::GREATER_EQUAL:
        checkNumberOperands(expr.op_, left, right);
        return std::get<double>(left) >= std::get<double>(right);
    case TokenType::LESS:
        checkNumberOperands(expr.op_, left, right);
        return std::get<double>(left) < std::get<double>(right);
    case TokenType::LESS_EQUAL:
        checkNumberOperands(expr.op_, left, right);
        return std::get<double>(left) <= std::get<double>(right);
    case TokenType::BANG_EQUAL:
        return !isEqual(left, right);
    case TokenType::EQUAL_EQUAL:
        return isEqual(left, right);
    case TokenType::MINUS:
        checkNumberOperands(expr.op_, left, right);
        return std::get<double>(left) - std::get<double>(right);
    case TokenType::PLUS:
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return std::get<double>(left) + std::get<double>(right);
        }
        if (std::holds_alternative<std::string>(left) &&
            std::holds_alternative<std::string>(right)) {
            return std::get<std::string>(left) + std::get<std::string>(right);
        }
        throw RuntimeError(expr.op_,
                           "Operands must be two numbers or two strings.");
    case TokenType::SLASH:
        return std::get<double>(left) / std::get<double>(right);
    case TokenType::STAR:
        return std::get<double>(left) * std::get<double>(right);
    default:
        return {}; // unreachable
    }
}

LoxValue Interpreter::visitGrouping(const Expr::Grouping &expr) {
    return evaluate(*expr.inner_);
}

LoxValue Interpreter::visitUnary(const Expr::Unary &expr) {
    auto right = evaluate(*expr.right_);

    switch (expr.op_.type) {
    case TokenType::BANG:
        return !isTruthy(right);
    case TokenType::MINUS:
        checkNumberOperand(expr.op_, right);
        return -std::get<double>(right);
    default:
        return {}; // unreachable
    }
}

LoxValue Interpreter::visitLiteral(const Expr::Literal &expr) {
    return expr.value_;
}
