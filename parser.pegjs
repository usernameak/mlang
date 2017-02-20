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

code = statement*
statement = _n s:(assign_statement / runtime_statement) _n ";" {
	return s;
}
assign_statement = assignee:identifier _ "=" _ value:expression {
	return {
    	type: "assign_statement",
        assignee: assignee,
        value: value
    }
}
expression "expression" = add_expression / number / identifier
runtime_statement = name:identifier args:(__ arg:expression {return arg})* {
	return {
    	type: "runtime_statement",
        name: name,
        args: args
    }
}
add_expression = left:mul_expression right:(_ op:("+" / "-") _ expr:mul_expression {return {op:op, expr:expr}})+ {
	return buildBinaryExpr(left, right);
} / mul_expression
mul_expression = left:prim_expression right:(_ op:("/" / "*") _ expr:prim_expression {return {op:op, expr:expr}})+ {
	return buildBinaryExpr(left, right);
}  / prim_expression
prim_expression = "(" _ expr:add_expression _ ")" {return expr} / number / identifier
number "number" = n:($("-"?([1-9][0-9]*) / "0")) {return {
	type: "number",
	val: +n
}}
identifier "identifier" = name:$([a-zA-Z_][a-zA-Z0-9_]*) {return {
	type: "identifier",
	name: name
}}
_ "whitespace" = [ \t]*
__ "whitespace" = [ \t]+
_n "newline" = [ \t\n]*