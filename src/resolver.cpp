#include "resolver.hpp"
#include <cassert>

namespace Jlox {

void Resolver::resolve(const std::vector<Stmt::StmtPtr> &statements) {
    for (auto &stmt : statements) {
        resolve(stmt);
    }
}

void Resolver::resolve(const Expr::ExprPtr expr) { return expr->accept(*this); }

void Resolver::visit(Expr::BinaryPtr expr) {
    resolve(expr->left);
    resolve(expr->right);
}

void Resolver::visit(Expr::GroupingPtr expr) { resolve(expr->inner); }

void Resolver::visit(Expr::UnaryPtr expr) { resolve(expr->right); }

void Resolver::visit(Expr::LiteralPtr /*expr*/) {} // do nothing

void Resolver::visit(Expr::VariablePtr expr) {
    const auto &name = expr->name.getLexemeString();
    if (!scopes.empty()) {
        const auto &scope = scopes.back();
        if (scope.find(name) != scope.end()) {
            if (!scope.at(name)) {
                handler.error(
                    expr->name,
                    "Can't read local variable in its own initializer.");
            }
        }
    }

    resolveLocal(expr, expr->name);
}

void Resolver::visit(Expr::AssignPtr expr) {
    resolve(expr->value);
    resolveLocal(expr, expr->name);
}

void Resolver::visit(Expr::LogicalPtr expr) {
    resolve(expr->left);
    resolve(expr->right);
}

void Resolver::visit(Expr::CallPtr expr) {
    resolve(expr->callee);
    for (const auto &arg : expr->arguments) {
        resolve(arg);
    }
}

void Resolver::visit(Expr::GetPtr expr) {
    // Since properties are looked up dynamically, only resolve the object,
    // not the property
    resolve(expr->object);
}

void Resolver::visit(Expr::SetPtr expr) {
    resolve(expr->object);
    resolve(expr->value);
}

void Resolver::visit(Expr::ThisPtr expr) {
    if (currentClass == ClassType::NONE) {
        handler.error(expr->keyword, "Can't use 'this' outside of a class.");
        return;
    }
    resolveLocal(expr, expr->keyword);
}

void Resolver::visit(Expr::SuperPtr expr) {
    if (currentClass == ClassType::NONE) {
        handler.error(expr->keyword, "Can't use 'super' outside of a class.");
    } else if (currentClass != ClassType::SUBCLASS) {
        handler.error(expr->keyword,
                      "Can't use 'super' in a class with no superclass.");
    }

    resolveLocal(expr, expr->keyword);
}

void Resolver::resolve(const Stmt::StmtPtr stmt) { return stmt->accept(*this); }

void Resolver::visit(Stmt::ExprPtr stmt) { resolve(stmt->expr); }

void Resolver::visit(Stmt::PrintPtr stmt) { resolve(stmt->expr); }

void Resolver::visit(Stmt::VarPtr stmt) {
    declare(stmt->name);
    if (stmt->initializer) {
        resolve(stmt->initializer);
    }
    define(stmt->name);
}

void Resolver::visit(Stmt::BlockPtr stmt) {
    beginScope();
    resolve(stmt->statements);
    endScope();
}

void Resolver::visit(Stmt::IfPtr stmt) {
    resolve(stmt->condition);
    resolve(stmt->thenBranch);
    if (stmt->elseBranch) {
        resolve(stmt->elseBranch);
    }
}

void Resolver::visit(Stmt::WhilePtr stmt) {
    resolve(stmt->condition);
    resolve(stmt->body);
}

void Resolver::visit(Stmt::FunctionPtr stmt) {
    declare(stmt->name);
    define(stmt->name);
    resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visit(Stmt::ReturnPtr stmt) {
    if (currentFunction == FunctionType::NONE) {
        handler.error(stmt->keyword, "Can't return from top-level code.");
    }

    if (stmt->value) {
        if (currentFunction == FunctionType::INITIALIZER) {
            handler.error(stmt->keyword,
                          "Can't return a value from an initializer.");
        }
        resolve(stmt->value);
    }
}

void Resolver::visit(Stmt::ClassPtr stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    declare(stmt->name);
    define(stmt->name);

    const bool hasSuperClass = stmt->superclass != nullptr;
    const auto &className = stmt->name.getLexemeString();
    const auto &superClassName =
        hasSuperClass ? stmt->superclass->name.getLexemeString() : "";
    if (hasSuperClass && className == superClassName) {
        handler.error(stmt->superclass->name,
                      "A class can't inherit from itself.");
    }

    if (hasSuperClass) {
        currentClass = ClassType::SUBCLASS;
        resolve(stmt->superclass);
        beginScope();
        scopes.back()["super"] = true;
    }

    beginScope();
    scopes.back()["this"] = true;

    for (auto &method : stmt->methods) {
        resolveFunction(method, method->name.getLexemeString() == "init"
                                    ? FunctionType::INITIALIZER
                                    : FunctionType::METHOD);
    }

    endScope();
    if (hasSuperClass) {
        endScope();
    }
    currentClass = enclosingClass;
}

void Resolver::resolveLocal(Expr::ExprPtr expr, const Token &name) {
    const int numScopes = scopes.size();
    for (int i = numScopes - 1; i >= 0; i--) {
        const auto &scope = scopes.at(i);
        if (scope.find(name.getLexemeString()) != scope.end()) {
            interpreter.resolve(expr, numScopes - 1 - i);
            return;
        }
    }
}

void Resolver::resolveFunction(Stmt::FunctionPtr function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const auto &param : function->params) {
        declare(param);
        define(param);
    }
    resolve(function->body);
    endScope();

    currentFunction = enclosingFunction;
}

void Resolver::declare(const Token &name) {
    if (scopes.empty())
        return;

    auto &scope = scopes.back();
    auto target = name.getLexemeString();
    if (scope.find(target) != scope.end()) {
        handler.error(name, "Already a variable with this name in this scope.");
    }
    scope[target] = false;
}

void Resolver::define(const Token &name) {
    if (scopes.empty())
        return;

    auto &scope = scopes.back();
    auto target = name.getLexemeString();
    assert(scope.find(target) != scope.end() && !scope.at(target));
    scope.at(target) = true;
}

void Resolver::beginScope() { scopes.push_back({}); }

void Resolver::endScope() { scopes.pop_back(); }

} // namespace Jlox
