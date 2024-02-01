#include <iostream>
#include <variant>

#include "interpreter.hpp"
#include "runtime_error.hpp"
#include "token.hpp"

static bool isTruthy(Value value) {
    if (std::holds_alternative<std::monostate>(value))
        return false;
    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value);

    return true;
}

static bool isEqual(Value val1, Value val2) {
    if (std::holds_alternative<std::monostate>(val1) &&
        std::holds_alternative<std::monostate>(val2))
        return true;

    if (std::holds_alternative<std::monostate>(val1))
        return false;

    return val1.index() == val2.index() ? val1 == val2 : false;
}

static void checkNumberOperand(Token op, Value operand) {
    if (std::holds_alternative<double>(operand))
        return;

    throw RuntimeError(op, "Operand must be a number.");
}

static void checkNumberOperands(Token op, Value left, Value right) {
    if (std::holds_alternative<double>(left) &&
        std::holds_alternative<double>(right))
        return;

    throw RuntimeError(op, "Operands must be numbers.");
}

Interpreter::Interpreter(ErrorHandler &handler) : handler(handler) {
    environment = new Environment();
}

Interpreter::~Interpreter() { delete environment; }

void Interpreter::interpret(const std::vector<Stmt::StmtPtr> &statements) {
    try {
        for (const auto &stmt : statements) {
            execute(*stmt);
        }
    } catch (const RuntimeError &error) {
        handler.runtimeError(error);
    }
}

void Interpreter::execute(const Stmt::BaseStmt &stmt) {
    return stmt.accept(*this);
}

void Interpreter::visit(const Stmt::Expr &stmt) {
    evaluate(*stmt.expr);
    return;
}

void Interpreter::visit(const Stmt::Print &stmt) {
    Value value = evaluate(*stmt.expr);
    std::cout << value << "\n";
}

void Interpreter::visit(const Stmt::Var &stmt) {
    Value value{};
    if (stmt.initializer) {
        value = evaluate(*stmt.initializer);
    }

    environment->define(stmt.name.lexeme, value);
}

void Interpreter::visit(const Stmt::Block &stmt) {
    auto new_env = std::make_unique<Environment>(this->environment);
    executeBlock(stmt.statements, new_env.get());
}

void Interpreter::visit(const Stmt::If &stmt) {
    if (isTruthy(evaluate(*stmt.condition))) {
        execute(*stmt.thenBranch);
    } else if (stmt.elseBranch) {
        execute(*stmt.elseBranch);
    }
}

void Interpreter::executeBlock(const std::vector<Stmt::StmtPtr> &statements,
                               Environment *environment) {
    auto new_scope = ScopeManager(*this, environment);
    for (const auto &stmt : statements) {
        execute(*stmt);
    }
}

Value Interpreter::evaluate(const Expr::BaseExpr &expr) {
    return expr.accept(*this);
}

Value Interpreter::visit(const Expr::Binary &expr) {
    auto left = evaluate(*expr.left);
    auto right = evaluate(*expr.right);

    switch (expr.op.type) {
    case TokenType::GREATER:
        checkNumberOperands(expr.op, left, right);
        return std::get<double>(left) > std::get<double>(right);
    case TokenType::GREATER_EQUAL:
        checkNumberOperands(expr.op, left, right);
        return std::get<double>(left) >= std::get<double>(right);
    case TokenType::LESS:
        checkNumberOperands(expr.op, left, right);
        return std::get<double>(left) < std::get<double>(right);
    case TokenType::LESS_EQUAL:
        checkNumberOperands(expr.op, left, right);
        return std::get<double>(left) <= std::get<double>(right);
    case TokenType::BANG_EQUAL:
        return !isEqual(left, right);
    case TokenType::EQUAL_EQUAL:
        return isEqual(left, right);
    case TokenType::MINUS:
        checkNumberOperands(expr.op, left, right);
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
        throw RuntimeError(expr.op,
                           "Operands must be two numbers or two strings.");
    case TokenType::SLASH:
        return std::get<double>(left) / std::get<double>(right);
    case TokenType::STAR:
        return std::get<double>(left) * std::get<double>(right);
    default:
        return {}; // unreachable
    }
}

Value Interpreter::visit(const Expr::Grouping &expr) {
    return evaluate(*expr.inner);
}

Value Interpreter::visit(const Expr::Unary &expr) {
    auto right = evaluate(*expr.right);

    switch (expr.op.type) {
    case TokenType::BANG:
        return !isTruthy(right);
    case TokenType::MINUS:
        checkNumberOperand(expr.op, right);
        return -std::get<double>(right);
    default:
        return {}; // unreachable
    }
}

Value Interpreter::visit(const Expr::Literal &expr) { return expr.value; }

Value Interpreter::visit(const Expr::Variable &expr) {
    return environment->get(expr.name);
}

Value Interpreter::visit(const Expr::Assign &expr) {
    auto value = evaluate(*expr.value);
    environment->assign(expr.name, value);
    return value;
};

ScopeManager::ScopeManager(Interpreter &interpreter, Environment *new_env)
    : interpreter(interpreter), new_env(new_env) {
    saved_env = interpreter.environment;
}

ScopeManager::~ScopeManager() { interpreter.environment = saved_env; }
