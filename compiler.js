module.exports = (function() {
	var Compiler = function(pcode) {
		this.pcode = pcode;
		this.vars = {};
		this.equations = {};
		this.frames = [];
	}
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
			case "call_expression":
				for(var j = 0; j < expr.args.length; j++) {
					out += this.pushExpression(expr.args[j]);
				}
				out += "pushf " + expr.name.name + "\n";
				out += "call\n";
			break;
		}
		return out;
	};

	Compiler.prototype.compile = function() {
		this.frames.push(this.compileFrame("main", [], this.pcode));
		return this.frames;
	}

	Compiler.prototype.compileFrame = function(name, args, code) {
		var subframes = [];
		var out = "";
		for(var i = args.length-1; i >= 0; i--) {
			out += "assn " + args[i] + "\n";
		}
		for(var i = 0; i < code.length; i++) {
			debugger;
			var statement = code[i];
			if(statement.type == "assign_statement") {
				out += this.pushExpression(statement.value);
				out += "assn " + statement.assignee.name + "\n";
			} else if(statement.type == "function_statement") {
				var ff = this.compileFrame(statement.name.name, statement.args.map(a=>a.name), statement.block.statements);
				subframes.push(ff);
			} else if(statement.type == "return_statement") {
				out += this.pushExpression(statement.val);
				out += "ret\n";
			} else if(statement.type == "runtime_statement") {
				for(var j = 0; j < statement.args.length; j++) {
					out += this.pushExpression(statement.args[j]);
				}
				out += "rtcl "+statement.name.name+"\n";
			} else if(statement.type == "call_statement") {
				for(var j = 0; j < statement.args.length; j++) {
					out += this.pushExpression(statement.args[j]);
				}
				out += "pushf " + statement.name.name + "\n";
				out += "call\npop\n";
			} else {

			}
		}
		return {
			name: name,
			code: out,
			subframes: subframes
		};
	}

	Compiler.prototype.assemble = function() {
		var bytecode = [];
		for(var i = 0; i < this.frames.length; i++) {
			bytecode.push.apply(bytecode, this.assembleFrame(this.frames[i]));
		}
		return bytecode;
	}

	Compiler.prototype.assembleFrame = function(frame) {
		var bytecode = [];
		var opcodes = {
			"add": 1,
			"sub": 2,
			"mul": 3,
			"div": 4,
			"push": 5,
			"pushv": 6,
			"assn": 7,
			"rtcl": 8,
			"pushf": 9,
			"ret": 10,
			"pushs": 11,
			"call": 12,
			"pop": 13
		}
		var opfuncs = {
			push: function(bc, num) {
				var buf = Buffer.alloc(8);
				buf.writeDoubleLE(+num);
				bc.push.apply(bc, buf.toJSON().data);
			},
			pushv: function(bc, name) {
				bc.push.apply(bc, Buffer.from(name).toJSON().data);
				bc.push(0);
			},
			pushs: function(bc, name) {
				bc.push.apply(bc, Buffer.from(name).toJSON().data);
				bc.push(0);
			},
			pushf: function(bc, name) {
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
		
		bytecode.push.apply(bytecode, Buffer.from(frame.name).toJSON().data);
		bytecode.push(0);

		var lines = frame.code.split("\n");
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

		bytecode.push(0);

		for(var i = 0; i < frame.subframes.length; i++) {
			bytecode.push.apply(bytecode, this.assembleFrame(frame.subframes[i]));
		}

		bytecode.push(0);

		return bytecode;
	}

	return Compiler;
})();