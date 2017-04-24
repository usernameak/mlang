module.exports = (function() {
	var crypto = require("crypto");
	var namegen = function () {
		var buf = crypto.randomBytes(16);
		return '_' + buf.toString("hex");
	};
	var Compiler = function(pcode) {
		this.pcode = pcode;
		this.vars = {};
		this.equations = {};
		this.frames = [];
		this.curlabel = 0;
		this.names = {};
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
					"/": "div\n",
					"<=": "leq\n",
					">": "grt\n",
					">=": "geq\n",
					"<": "less\n",
					"==": "eq\n",
					"!=": "neq\n",
					"<<": "lsh\n",
					">>": "rsh\n",
					"&": "and\n",
					"|": "or\n"
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
				out += "pushv " + expr.name.name + "\n";
				out += "call\n";
			break;
			case "string":
				out += "pushs " + expr.val + "\n";
			break;
			case "boolean":
				out += "pushb " + expr.val + "\n";
			break;
		}
		return out;
	};

	Compiler.prototype.compile = function() {
		this.frames.push(this.compileFrame("main", [], this.pcode));
		return this.frames;
	}

	Compiler.prototype.compileStatement = function(statement) {
		var out = "";
		var subframes = [];
		if(statement.type == "assign_statement") {
			out += this.pushExpression(statement.value);
			out += "assn " + statement.assignee.name + "\n";
		} else if(statement.type == "function_statement") {
			this.names[statement.name.name] = namegen();
			var ff = this.compileFrame(this.names[statement.name.name], statement.args.map(a=>a.name), statement.block.statements);
			this.frames.push(ff);
			out += "pushf " + this.names[statement.name.name] + "\n";
			out += "assn " + statement.name.name + "\n";
		} else if(statement.type == "return_statement") {
			out += this.pushExpression(statement.val);
			out += "ret\n";
		} else if(statement.type == "export_statement") {
			out += "pushmap\n";
			for(var j = 0; j < statement.exports.length; j++) {
				out += this.pushExpression(statement.exports[j]);
				out += this.pushExpression({
					type: "string",
					val: statement.exports[j].name
				});
				out += "massn\n"
			}
			out += "ret\n";
		} else if (statement.type == "import_statement") {
			out += this.pushExpression(statement.libname);
			out += "ecall\n";
			out += "unmap\n";
		} else if(statement.type == "runtime_statement") {
			for(var j = 0; j < statement.args.length; j++) {
				out += this.pushExpression(statement.args[j]);
			}
			out += "rtcl "+statement.name.name+"\n";
		} else if(statement.type == "call_statement") {
			for(var j = 0; j < statement.args.length; j++) {
				out += this.pushExpression(statement.args[j]);
			}
			out += "pushv " + statement.name.name + "\n";
			out += "call\npop\n";
		} else if(statement.type == "if_statement") {
			var lab1 = this.curlabel;
			var lab2 = this.curlabel + 1;
			this.curlabel += 2;
			out += this.pushExpression(statement.condition);
			var strt = out.split("\n").length-2
			out += "jn L" + lab1 + "\n";
			var ifframe = this.compileFrame("L"+this.curlabel, [], statement.thenBlock.statements, true);
			out += ifframe.code;
			subframes.push.apply(subframes, ifframe.subframes);
			if(statement.elseBlock) {
				out += "jmp L" + lab2 + "\n";
			}
			out += ":L" + lab1 + "\n";
			if(statement.elseBlock) {
				var elseframe = this.compileFrame("L"+lab2, [], statement.elseBlock.statements, true);
				out += elseframe.code;
				subframes.push.apply(subframes, elseframe.subframes);
				out += ":L" + lab2 + "\n";
			}

		} else if(statement.type == "for_statement") {
			var lab1 = this.curlabel;
			var lab2 = this.curlabel + 1;
			this.curlabel += 2;
			var initcstat = this.compileStatement(statement.initial);
			out += initcstat.out;
			subframes.push.apply(subframes, initcstat.subframes);
			out += ":L" + lab1 + "\n";
			out += this.pushExpression(statement.condition);
			out += "jn L" + lab2 + "\n";
			var blockframe = this.compileFrame("L"+lab1, [], statement.block.statements, true);
			out += blockframe.code;
			subframes.push.apply(subframes, blockframe.subframes);
			var inccstat = this.compileStatement(statement.increment);
			out += inccstat.out;
			subframes.push.apply(subframes, inccstat.subframes);
			out += "jmp L" + lab1 + "\n";
			out += ":L" + lab2 + "\n";
		} else {

		}
		return {
			out: out,
			subframes: subframes
		};
	}

	Compiler.prototype.compileFrame = function(name, args, code, dontUseReturnStub) {
		var subframes = [];
		var out = "";
		for(var i = args.length-1; i >= 0; i--) {
			out += "assn " + args[i] + "\n";
		}
		for(var i = 0; i < code.length; i++) {
			var statement = this.compileStatement(code[i]);
			out += statement.out;
			subframes.push.apply(subframes, statement.subframes);
		}
		if(!dontUseReturnStub) out += "push 0\nret\n";
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
		var lines = frame.code.replace(/^\s*$/g, '').split("\n");
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
			"pop": 13,
			"pushb": 14,
			"leq": 15,
			"grt": 16,
			"geq": 17,
			"less": 18,
			"eq": 19,
			"neq": 20,
			"lsh": 21,
			"rsh": 22,
			"and": 23,
			"or": 24,
			"jn": 25,
			"jmp": 26,
			"unmap": 27,
			"pushmap": 28,
			"massn": 29,
			"ecall": 30
		}
		var opfuncs = {
			push: function(bc, num) {
				var buf = Buffer.alloc(8);
				buf.writeDoubleLE(+num);
				bc.push.apply(bc, buf.toJSON().data);
			},
			pushb: function(bc, val) {
				bc.push(val == "true" ? 1 : 0);
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
			},
			jn: function(bc, label) {
				var idx = 0;
				for(var i = 0; i < lines.length; i++) {
					var elems = lines[i].split(" ");
					if(elems[0].charAt(0) == ":") {
						if(elems[0].slice(1) == label) {
							var buf = Buffer.alloc(4);
							buf.writeInt32LE(idx);
							bc.push.apply(bc, buf.toJSON().data);
							return;
						}
					} else idx++;
				}
				console.log("Error assembling frame \"" + frame.name + "\": label \"" + label + "\" not found")
			},
			jmp: function(bc, label) {
				var idx = 0;
				for(var i = 0; i < lines.length; i++) {
					var elems = lines[i].split(" ");
					if(elems[0].charAt(0) == ":") {
						if(elems[0].slice(1) == label) {
							var buf = Buffer.alloc(4);
							buf.writeInt32LE(idx);
							bc.push.apply(bc, buf.toJSON().data);
							return;
						}
					} else idx++;
				}
				console.log("Error assembling frame \"" + frame.name + "\": label \"" + label + "\" not found")
			}
		}

		bytecode.push.apply(bytecode, Buffer.from(frame.name).toJSON().data);
		bytecode.push(0);

		for(var i = 0; i < lines.length; i++) {
			if(!lines[i].replace(/\s/g, '').length) {
				continue;
			}
			var elems = lines[i].split(" ");
			if(elems[0].charAt(0) == ":") {
				continue;
			}
			bytecode.push(opcodes[elems[0]]);
			if(elems[0] in opfuncs) {
				var args = [bytecode, elems.slice(1).join(" ")];
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
