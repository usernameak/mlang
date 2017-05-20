#pragma once

#include "expressions.h"
#include <memory>

enum StatementType {
    ST_Assign,
    ST_Return,
    ST_Function,
    ST_Export,
    ST_Import,
    ST_Runtime,
    ST_Call,
    ST_If,
    ST_For
};

struct Statement {
    StatementType type;
};

struct AssignStatement : Statement {
    std::string assignee;
    std::shared_ptr<Expression> value;
};

struct ReturnStatement : Statement {
    std::shared_ptr<Expression> value;
};

struct FunctionStatement : Statement {
    std::string name;
    std::vector<std::string> args;
    std::vector<std::shared_ptr<Statement> > block;
};

struct ExportStatement : Statement {
    std::vector<std::string> exports;
};

struct ImportStatement : Statement {
    std::string what;
};

struct RuntimeStatement : Statement {
    std::string name;
    std::vector<std::shared_ptr<Expression>> args;
};

struct CallStatement : Statement {
    std::string name;
    std::vector<std::shared_ptr<Expression>> args;
};

struct IfStatement : Statement {
    std::shared_ptr<Expression> condition;
    std::vector<std::shared_ptr<Statement> > thenblock;
    std::vector<std::shared_ptr<Statement> > elseblock;
};

struct ForStatement : Statement {
    std::shared_ptr<Statement> initial;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> increment;
    std::vector<std::shared_ptr<Statement> > block;
};
