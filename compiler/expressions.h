#pragma once

#include <vector>
#include <memory>
#include <string>

enum ExpressionType {
    ET_Binary,
    ET_Number,
    ET_Identifier,
    ET_Call,
    ET_String,
    ET_Bool
};

struct Expression {
    ExpressionType type;
};

struct BinaryExpression : Expression {
    int op;
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
};

struct NumberExpression : Expression {
    double value;
};

struct IdentifierExpression : Expression {
    std::string name;
};

struct CallExpression : Expression {
    std::string name;
    std::vector<std::shared_ptr<Expression> > args;
};

struct StringExpression : Expression {
    std::string value;
};

struct BoolExpression : Expression {
    bool value;
};
