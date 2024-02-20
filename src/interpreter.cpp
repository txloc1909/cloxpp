#include <iostream>
#include <variant>

#include "expr.hpp"
#include "interpreter.hpp"
#include "lox_class.hpp"
#include "lox_function.hpp"
#include "return.hpp"
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

Interpreter::Interpreter(ErrorHandler &handler)
    : handler(handler), globals_(std::make_shared<Environment>()),
      currentEnvironment(globals_), locals({}) {
    globals_->define("clock", std::make_shared<NativeClock>());
}

Interpreter::~Interpreter() = default;

EnvironmentPtr Interpreter::globalEnvironment() const {
    return currentEnvironment;
};

void Interpreter::interpret(const std::vector<Stmt::StmtPtr> &statements) {
    try {
        for (const auto &stmt : statements) {
            execute(stmt);
        }
    } catch (const RuntimeError &error) {
        handler.runtimeError(error);
    }
}

void Interpreter::execute(const Stmt::StmtPtr stmt) {
    return stmt->accept(*this);
}

void Interpreter::visit(Stmt::ExprPtr stmt) {
    evaluate(stmt->expr);
    return;
}

void Interpreter::visit(Stmt::PrintPtr stmt) {
    Value value = evaluate(stmt->expr);
    std::cout << value << "\n";
}

void Interpreter::visit(Stmt::VarPtr stmt) {
    Value value{};
    if (stmt->initializer) {
        value = evaluate(stmt->initializer);
    }

    currentEnvironment->define(stmt->name.lexeme, value);
}

void Interpreter::visit(Stmt::BlockPtr stmt) {
    EnvironmentPtr new_env = std::make_shared<Environment>(currentEnvironment);
    executeBlock(stmt->statements, new_env);
}

void Interpreter::visit(Stmt::IfPtr stmt) {
    if (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->thenBranch);
    } else if (stmt->elseBranch) {
        execute(stmt->elseBranch);
    }
}

void Interpreter::visit(Stmt::WhilePtr stmt) {
    while (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->body);
    }
}

void Interpreter::visit(Stmt::FunctionPtr stmt) {
    Value function = std::make_shared<LoxFunction>(stmt->shared_from_this(),
                                                   currentEnvironment);
    currentEnvironment->define(stmt->name.lexeme, function);
}

void Interpreter::visit(Stmt::ReturnPtr stmt) {
    Value value{};
    if (stmt->value) {
        value = evaluate(stmt->value);
    }

    throw RuntimeReturn(value);
}

void Interpreter::visit(Stmt::ClassPtr stmt) {
    currentEnvironment->define(stmt->name.lexeme, {});

    auto methods = std::unordered_map<std::string, LoxFunctionPtr>();
    for (Stmt::FunctionPtr method : stmt->methods) {
        const bool isInitializer = method->name.lexeme == "init";
        methods[method->name.lexeme] = std::make_shared<LoxFunction>(
            method, currentEnvironment, isInitializer);
    }

    currentEnvironment->assign(
        stmt->name,
        std::dynamic_pointer_cast<LoxCallable>(
            std::make_shared<LoxClass>(stmt->name.lexeme, methods)));
}

void Interpreter::executeBlock(const std::vector<Stmt::StmtPtr> &statements,
                               EnvironmentPtr environment) {
    auto new_scope = ScopeManager(*this, environment);
    for (const auto &stmt : statements) {
        execute(stmt);
    }
}

void Interpreter::resolve(Expr::ExprPtr expr, int depth) {
    locals[expr] = depth;
}

Value Interpreter::lookUpVariable(const Token &name,
                                  const Expr::ExprPtr &expr) {
    if (locals.find(expr) != locals.end()) {
        int depth = locals.at(expr);
        return currentEnvironment->getAt(depth, name.lexeme);
    } else {
        return globals_->get(name);
    }
}

Value Interpreter::evaluate(const Expr::ExprPtr expr) {
    return expr->accept(*this);
}

Value Interpreter::visit(Expr::BinaryPtr expr) {
    auto left = evaluate(expr->left);
    auto right = evaluate(expr->right);

    switch (expr->op.type) {
    case TokenType::GREATER:
        checkNumberOperands(expr->op, left, right);
        return std::get<double>(left) > std::get<double>(right);
    case TokenType::GREATER_EQUAL:
        checkNumberOperands(expr->op, left, right);
        return std::get<double>(left) >= std::get<double>(right);
    case TokenType::LESS:
        checkNumberOperands(expr->op, left, right);
        return std::get<double>(left) < std::get<double>(right);
    case TokenType::LESS_EQUAL:
        checkNumberOperands(expr->op, left, right);
        return std::get<double>(left) <= std::get<double>(right);
    case TokenType::BANG_EQUAL:
        return !isEqual(left, right);
    case TokenType::EQUAL_EQUAL:
        return isEqual(left, right);
    case TokenType::MINUS:
        checkNumberOperands(expr->op, left, right);
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
        throw RuntimeError(expr->op,
                           "Operands must be two numbers or two strings.");
    case TokenType::SLASH:
        return std::get<double>(left) / std::get<double>(right);
    case TokenType::STAR:
        return std::get<double>(left) * std::get<double>(right);
    default:
        return {}; // unreachable
    }
}

Value Interpreter::visit(Expr::GroupingPtr expr) {
    return evaluate(expr->inner);
}

Value Interpreter::visit(Expr::UnaryPtr expr) {
    auto right = evaluate(expr->right);

    switch (expr->op.type) {
    case TokenType::BANG:
        return !isTruthy(right);
    case TokenType::MINUS:
        checkNumberOperand(expr->op, right);
        return -std::get<double>(right);
    default:
        return {}; // unreachable
    }
}

Value Interpreter::visit(Expr::LiteralPtr expr) {
    auto value = Value{};

    // since Literal is a subset of Value, this std::visit call is trivial
    std::visit([&](auto &&arg) { value = arg; }, expr->value);

    return value;
}

Value Interpreter::visit(Expr::VariablePtr expr) {
    return lookUpVariable(expr->name, expr);
}

Value Interpreter::visit(Expr::AssignPtr expr) {
    Value value = evaluate(expr->value);

    if (locals.find(expr) != locals.end()) {
        int distance = locals.at(expr);
        currentEnvironment->assignAt(distance, expr->name, value);
    } else {
        globals_->assign(expr->name, value);
    }
    return value;
};

Value Interpreter::visit(Expr::LogicalPtr expr) {
    Value left = evaluate(expr->left);

    // short-circuit
    if (expr->op.type == TokenType::OR) {
        if (isTruthy(left))
            return left;
    } else { // expr->op.type == TokenType::AND
        if (!isTruthy(left))
            return left;
    }

    return evaluate(expr->right);
}

Value Interpreter::visit(Expr::CallPtr expr) {
    auto callee = evaluate(expr->callee);

    // Order of evaluating arguments is kept
    auto arguments = std::vector<Value>();
    for (const auto &arg : expr->arguments) {
        arguments.push_back(evaluate(arg));
    }

    if (!std::holds_alternative<LoxCallablePtr>(callee)) {
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }

    auto callable = std::get<LoxCallablePtr>(callee);
    if (arguments.size() != callable->arity()) {
        throw RuntimeError(expr->paren,
                           "Expected " + std::to_string(callable->arity()) +
                               " arguments but got " +
                               std::to_string(arguments.size()) + ".");
    }

    return callable->call(*this, arguments);
}

Value Interpreter::visit(Expr::GetPtr expr) {
    Value object = evaluate(expr->object);
    if (std::holds_alternative<LoxInstancePtr>(object)) {
        return std::get<LoxInstancePtr>(object)->get(expr->name);
    }

    throw RuntimeError(expr->name, "Only instances have properties.");
}

Value Interpreter::visit(Expr::SetPtr expr) {
    Value object = evaluate(expr->object);

    if (!std::holds_alternative<LoxInstancePtr>(object)) {
        throw RuntimeError(expr->name, "Only instances have fields.");
    }

    Value value = evaluate(expr->value);
    std::get<LoxInstancePtr>(object)->set(expr->name, value);
    return value;
}

Value Interpreter::visit(Expr::ThisPtr expr) {
    return lookUpVariable(expr->keyword, expr);
}

ScopeManager::ScopeManager(Interpreter &interpreter, EnvironmentPtr new_env)
    : interpreter(interpreter), new_env(new_env) {
    saved_env = interpreter.currentEnvironment;
    interpreter.currentEnvironment = new_env;
}

ScopeManager::~ScopeManager() { interpreter.currentEnvironment = saved_env; }
