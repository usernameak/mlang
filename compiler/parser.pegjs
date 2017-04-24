{
    function buildBinaryExpr(left, right) {
        var expr = {
            type: "binary_expression",
            op: right[0].op,
            left: left,
            right: right[0].expr
        };
        for(var i = 1; i < right.length; i++) {
            expr = {
                type: "binary_expression",
                op: right[i].op,
                left: expr,
                right: right[i].expr
            };
        }
        return expr;
    }
}

code = _n s:statement* _n {return s}
statement = _n s:(if_statement / assign_statement / import_statement / export_statement / return_statement / runtime_statement / function_statement / call_statement) _n ";" {
    return s;
}
assign_statement = assignee:identifier _ "=" _ value:expression {
    return {
        type: "assign_statement",
        assignee: assignee,
        value: value
    }
}
function_statement = name:identifier _ "(" _ args:(arg1:identifier args2:(_ "," _ arg:identifier _ {return arg})* {return [arg1].concat(args2)}) _ ")" _ "=" _ block:block {
    return {
        type: "function_statement",
        name: name,
        args: args,
        block: block
    }
}
return_statement = "return" __ val:expression {
    return {
        type: "return_statement",
        val: val
    }
}
import_statement = "import" __ libname:string {
    return {
        type: "import_statement",
        libname: libname
    }
}
export_statement = "export" __ args:(arg1:identifier args2:(_ "," _ arg:identifier _ {return arg})* {return [arg1].concat(args2)}) {
    return {
        type: "export_statement",
        exports: args
    }
}
runtime_statement = "$" __ name:identifier args:(__ arg:expression {return arg})* {
    return {
        type: "runtime_statement",
        name: name,
        args: args
    }
}
call_statement = expr:call_expression {
    expr.type = "call_statement";
    return expr;
}
if_statement = "if" _ "(" _ cond:expression _ ")" _ thenBlock:block elseBlock:( _ "else" _ b:block {return b})? {
    return elseBlock ? {
        type: "if_statement",
        condition: cond,
        thenBlock: thenBlock,
        elseBlock: elseBlock
    } : {
        type: "if_statement",
        condition: cond,
        thenBlock: thenBlock
    }
}

expression "expression" = bwise_expression / noadd_expression
noadd_expression "expression" = call_expression / string / number / boolean / identifier
call_expression = name:identifier _ "(" _ args:(arg1:expression args2:(_ "," _ arg:expression _ {return arg})* {return [arg1].concat(args2)}) _ ")" {
    return {
        type: "call_expression",
        name: name,
        args: args
    }
}
bwise_expression = left:cmp_expression right:(_ op:("<<" / ">>" / "&" / "|") _ expr:cmp_expression {return {op:op, expr:expr}})+ {
    return buildBinaryExpr(left, right);
} / cmp_expression
cmp_expression = left:add_expression right:(_ op:("<" / ">" / ">=" / "<=" / "!=" / "==") _ expr:add_expression {return {op:op, expr:expr}})+ {
    return buildBinaryExpr(left, right);
} / add_expression
add_expression = left:mul_expression right:(_ op:("+" / "-") _ expr:mul_expression {return {op:op, expr:expr}})+ {
    return buildBinaryExpr(left, right);
} / mul_expression
mul_expression = left:prim_expression right:(_ op:("/" / "*") _ expr:prim_expression {return {op:op, expr:expr}})+ {
    return buildBinaryExpr(left, right);
}  / prim_expression
prim_expression = noadd_expression / "(" _ expr:expression _ ")" {return expr} / string / number / boolean / identifier

block = "{" _n sm:statement* _n "}" {
    return {
        type: "block_expression",
        statements: sm
       }
}
string "string" = "\"" s:("\\\"" {return "\""} / "\\n" {return "\n"} / "\\t" {return "\t"} / "\\v" {return "\v"} / "\\r" {return "\r"} / ! "\"" ch:. {return ch})* "\"" {return {
    type: "string",
    val: s.join("")
}}
number "number" = n:($("-"?([1-9][0-9]*) / "0")) {return {
    type: "number",
    val: +n
}}
boolean "boolean" = val:("true"/"false") {return {
    type: "boolean",
    val: val == "true" ? true : false
}}
identifier "identifier" = name:$([a-zA-Z_][a-zA-Z0-9_]*) {return {
    type: "identifier",
    name: name
}}

_ "whitespace" = [ \t]*
__ "whitespace" = [ \t]+
_n "newline" = [ \t\n]*
