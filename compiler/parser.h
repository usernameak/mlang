#pragma once

#include "scanner.h"
#include "expressions.h"
#include "statements.h"

class Parser {
public:
    Parser(Scanner&);
    void Parse();
    std::shared_ptr<Statement> ParseStatement();
    std::shared_ptr<RuntimeStatement> ParseRuntime();
    std::shared_ptr<IfStatement> ParseIf();
    std::shared_ptr<ForStatement> ParseFor();
    std::shared_ptr<ReturnStatement> ParseReturn();
    std::shared_ptr<ExportStatement> ParseExport();
    std::shared_ptr<ImportStatement> ParseImport();
    std::shared_ptr<FunctionStatement> ParseFunction(std::string);
    std::shared_ptr<AssignStatement> ParseAssignment(std::string);
    std::shared_ptr<CallStatement> ParseCall(std::string);
    std::vector<std::shared_ptr<Statement>> ParseBlock();
    std::shared_ptr<Expression> ParseExpression(bool strict = true);
protected:
    Scanner &scanner;
    std::vector<std::shared_ptr<Statement>> statements;
    bool saving;
};
