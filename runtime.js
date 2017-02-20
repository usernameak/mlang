var Compiler = function(pcode) {
	this.pcode = pcode;
	this.vars = {};
	this.equations = {};
}
/*Compiler.prototype._simplifyEquationLeft = function(equation) {
	equation.lefthand.
}*/

Compiler.prototype.pushExpression = function(expr) {
	var out = "";
	switch(expr.type) {
		case "binary_expression":
			out += this.pushExpression(expr.left);
			out += this.pushExpression(expr.right);
			out += {
				"+": "add\n",
				"-": "sub\n",
				"*": "mul\n",
				"/": "div\n"
			}[expr.op];
		break;
		case "number":
			out += "push " + expr.val + "\n";
		break;
		case "identifier":
			out += "pushv " + expr.name + "\n";
		break;
	}
	return out;
};

Compiler.prototype.compile = function() {
	var out = "";
	for(var i = 0; i < this.pcode.length; i++) {
		var statement = this.pcode[i];
		if(statement.type == "assign_statement") {
			out += this.pushExpression(statement.value);
			out += "assn " + statement.assignee.name + "\n";
		} else if(statement.type == "Compiler_statement") {
			for(var i = 0; i < statement.args.length; i++) {
				out += this.pushExpression(statement.args[i]);
			}
			out += "rtcl "+statement.name.name+"\n";
		} else if(statement.type == "equation_statement") {
			console.log("??? FUCKING SHIT EQUATIONS NOW NOT SUPPORTED MOTHERFUCKER !!!");
			return;
		} else {

		}
	}
	return out;
}

module.exports = Compiler;