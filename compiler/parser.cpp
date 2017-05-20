#include "parser.h"

#include "scanner.h"
#include <iostream>
#include "expressions.h"
#include "statements.h"
#include "util.h"
#include <cstring>

Parser::Parser(Scanner &sc) : scanner(sc) {}

void Parser::Parse() {
    std::shared_ptr<Statement> stmt;
    while((stmt = ParseStatement()) != nullptr) {
        statements.push_back(stmt);
        scanner.CheckToken(';');
    }
}

std::shared_ptr<Statement> Parser::ParseStatement() {
    if(!scanner.GetString()) return nullptr;
    if(scanner.TokenType == TK_Import) {
        return ParseImport();
    } else if(scanner.TokenType == TK_Export) {
        return ParseExport();
    } else if(scanner.TokenType == '$') {
        return ParseRuntime();
    } else if(scanner.TokenType == TK_Identifier) {
        std::string name = scanner.String;
        scanner.GetString();
        if(scanner.Compare("=")) {
            return ParseAssignment(name);
        } else if(scanner.Compare("(")) {
            std::shared_ptr<Statement> stmt;
            Scanner::SavedPos pos = scanner.SavePos();
            if((stmt = ParseFunction(name)) == nullptr) {
                scanner.RestorePos(pos);
                stmt = ParseCall(name);
            }
            return stmt;
        } else {
            scanner.UnknownTokenError();
            exit(2);
            return nullptr;
        }
    } else if(scanner.TokenType == TK_Return) {
        return ParseReturn();
    } else if(scanner.TokenType == TK_If) {
        return ParseIf();
    } else if(scanner.TokenType == TK_For) {
        return ParseFor();
    } else {
        scanner.UnknownTokenError();
        exit(2);
        return nullptr;
    }
}

std::shared_ptr<RuntimeStatement> Parser::ParseRuntime() {
    std::shared_ptr<RuntimeStatement> stmt = std::make_shared<RuntimeStatement>();
    stmt->type = ST_Runtime;
    scanner.MustGetToken(TK_Identifier);
    stmt->name = scanner.String;
    do {
        std::shared_ptr<Expression> arg = ParseExpression(false);
        if(arg == nullptr) break;
        stmt->args.push_back(arg);
    } while(scanner.CheckToken(','));
    return stmt;
}

std::shared_ptr<IfStatement> Parser::ParseIf() {
    std::shared_ptr<IfStatement> stmt = std::make_shared<IfStatement>();
    stmt->type = ST_Import;
    scanner.MustGetToken('(');
    stmt->condition = ParseExpression();
    scanner.MustGetToken(')');
    stmt->thenblock = ParseBlock();
    if(scanner.CheckToken(TK_Else)) {
        stmt->elseblock = ParseBlock();
    }
    return stmt;
}

std::shared_ptr<ForStatement> Parser::ParseFor() {
    std::shared_ptr<ForStatement> stmt = std::make_shared<ForStatement>();
    stmt->type = ST_For;
    scanner.MustGetToken('(');
    stmt->initial = ParseStatement();
    scanner.MustGetToken(';');
    stmt->condition = ParseExpression();
    scanner.MustGetToken(';');
    stmt->increment = ParseStatement();
    scanner.MustGetToken(')');
    stmt->block = ParseBlock();
    return stmt;
}

std::shared_ptr<ReturnStatement> Parser::ParseReturn() {
    std::shared_ptr<ReturnStatement> stmt = std::make_shared<ReturnStatement>();
    stmt->type = ST_Return;
    stmt->value = ParseExpression();
    return stmt;
}

std::shared_ptr<ExportStatement> Parser::ParseExport() {
    std::shared_ptr<ExportStatement> stmt = std::make_shared<ExportStatement>();
    stmt->type = ST_Export;
    do {
        scanner.MustGetToken(TK_Identifier);
        stmt->exports.push_back(scanner.String);
    } while(scanner.CheckToken(','));
    return stmt;
}

std::shared_ptr<ImportStatement> Parser::ParseImport() {
    scanner.MustGetToken(TK_StringConst);

    std::shared_ptr<ImportStatement> stmt = std::make_shared<ImportStatement>();
    stmt->type = ST_Import;
    stmt->what = scanner.String;
    return stmt;
}

std::shared_ptr<FunctionStatement> Parser::ParseFunction(std::string name) {
    std::shared_ptr<FunctionStatement> stmt = std::make_shared<FunctionStatement>();
    stmt->type = ST_Function;
    stmt->name = name;
    do {
        if(scanner.CheckToken(TK_Identifier)) {
            stmt->args.push_back(scanner.String);
        } else {
            return nullptr;
        }
    } while(scanner.CheckToken(','));
    if(!scanner.CheckToken(')')) {
        return nullptr;
    }
    if(!scanner.CheckToken('=')) {
        return nullptr;
    }
    stmt->block = ParseBlock();
    return stmt;
}

std::vector<std::shared_ptr<Statement> > Parser::ParseBlock() {
    scanner.MustGetToken('{');
    std::vector<std::shared_ptr<Statement> > ret;
    while(!scanner.CheckToken('}')) {
        ret.push_back(ParseStatement());
        scanner.CheckToken(';');
    }
    return ret;
}

std::shared_ptr<AssignStatement> Parser::ParseAssignment(std::string assignee) {
    std::shared_ptr<AssignStatement> stmt = std::make_shared<AssignStatement>();
    stmt->type = ST_Assign;
    stmt->assignee = assignee;
    stmt->value = ParseExpression();
    return stmt;
}

std::shared_ptr<CallStatement> Parser::ParseCall(std::string name) {
    std::shared_ptr<CallStatement> stmt = std::make_shared<CallStatement>();
    stmt->type = ST_Call;
    stmt->name = name;
    do {
        std::shared_ptr<Expression> arg = ParseExpression(false);
        if(arg == nullptr) break;
        stmt->args.push_back(arg);
    } while(scanner.CheckToken(','));
    scanner.MustGetToken(')');
    return stmt;
}

std::shared_ptr<Expression> Parser::ParseExpression(bool strict) {
    std::shared_ptr<Expression> ret;
    scanner.GetToken();
    if(scanner.TokenType == TK_FloatConst) {
        std::shared_ptr<NumberExpression> expr = std::make_shared<NumberExpression>();
        expr->type = ET_Number;
        expr->value = scanner.Float;
        ret = expr;
    } else if(scanner.TokenType == TK_StringConst) {
        std::shared_ptr<StringExpression> expr = std::make_shared<StringExpression>();
        expr->type = ET_String;
        expr->value = scanner.String;
        ret = expr;
    } else if(scanner.TokenType == TK_Identifier) {
        std::string name = scanner.String;
        if(name.compare("true") == 0 || name.compare("false") == 0) {
            std::shared_ptr<BoolExpression> expr = std::make_shared<BoolExpression>();
            expr->type = ET_Bool;
            expr->value = name.compare("true") == 0;
            ret = expr;
        }
        if(scanner.CheckToken('(')) {
            std::shared_ptr<CallExpression> expr = std::make_shared<CallExpression>();
            expr->type = ET_Call;
            expr->name = name;
            do {
                std::shared_ptr<Expression> arg = ParseExpression(false);
                if(arg == nullptr) break;
                expr->args.push_back(arg);
            } while(scanner.CheckToken(','));
            scanner.MustGetToken(')');
            ret = expr;
        }
        std::shared_ptr<IdentifierExpression> expr = std::make_shared<IdentifierExpression>();
        expr->type = ET_Identifier;
        expr->name = name;
        ret = expr;
    } else {
        scanner.UnknownTokenError();
        exit(2);
        ret = nullptr;
    }
    if(scanner.GetToken()) {
        switch(scanner.TokenType) {
            case '>':
            case '<':
            case TK_Equals:
            case TK_NotEquals:
            case TK_LeftShift:
            case TK_RightShift:
            case TK_Leq:
            case TK_Geq:
            case '+':
            case '-':
            case '*':
            case '/':
            case '&':
            case '|':{
                std::shared_ptr<BinaryExpression> expr = std::make_shared<BinaryExpression>();
                expr->type = ET_Binary;
                expr->left = ret;
                expr->right = ParseExpression();
                expr->op = scanner.TokenType;
                ret = expr;
            }break;
            default:
                scanner.UnGet();
            break;
        }
    }
    return ret;
}
