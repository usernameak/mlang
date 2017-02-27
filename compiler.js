var Compiler = function(pcode) {
	this.pcode = pcode;
	this.vars = {};
	this.equations = {};
	this.ccode = "";
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
		debugger;
		var statement = this.pcode[i];
		if(statement.type == "assign_statement") {
			out += this.pushExpression(statement.value);
			out += "assn " + statement.assignee.name + "\n";
		} else if(statement.type == "function_statement") {
			
			out += "fstart\n";
			for(var j = statement.args.length-1; j >= 0; j--) {
				out += "assn " + statement.args[j].name + "\n";
			}
			var fc = new Compiler(statement.block.statements);
			out += fc.compile();
			out += "pushf\n";
			out += "assn " + statement.name.name + "\n";
		} else if(statement.type == "return_statement") {
			out += this.pushExpression(statement.val);
			out += "ret\n";
		} else if(statement.type == "runtime_statement") {
			for(var j = 0; j < statement.args.length; j++) {
				out += this.pushExpression(statement.args[j]);
			}
			out += "rtcl "+statement.name.name+"\n";
		} else if(statement.type == "equation_statement") {
			// wtf???
			return;
		} else {

		}
	}
	this.ccode = out;
	return out;
}

Compiler.prototype.assemble = function() {
	var opcodes = {
		"add": 1,
		"sub": 2,
		"mul": 3,
		"div": 4,
		"push": 5,
		"pushv": 6,
		"assn": 7,
		"rtcl": 8,
		"fstart": 9,
		"ret": 10,
		"pushf": 11
	}
	var opfuncs = {
		push: function(bc, num) {
			var buf = Buffer.alloc(4);
			buf.writeInt32LE(+num);
			bc.push.apply(bc, buf.toJSON().data);
		},
		pushv: function(bc, name) {
			bc.push.apply(bc, Buffer.from(name).toJSON().data);
			bc.push(0);
		},
		assn: function(bc, name) {
			bc.push.apply(bc, Buffer.from(name).toJSON().data);
			bc.push(0);
		},
		rtcl: function(bc, name) {
			bc.push.apply(bc, Buffer.from(name).toJSON().data);
			bc.push(0);
		}
	}
	var lines = this.ccode.split("\n");
	var bytecode = [];
	for(var i = 0; i < lines.length; i++) {
		if(!lines[i].replace(/\s/g, '').length) {
			continue;
		}
		var elems = lines[i].split(" ");
		bytecode.push(opcodes[elems[0]]);
		if(elems[0] in opfuncs) {
			var args = [bytecode];
			for(var j = 1; j < elems.length; j++) {
				args.push(elems[j]);
			}
			opfuncs[elems[0]].apply(this, args);
		}
	}
	return bytecode;
}

module.exports = Compiler;