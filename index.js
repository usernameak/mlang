var fs = require("fs");
var parser = require("./parser");
var Compiler = require("./compiler");

if(process.argv.length != 4) {
	process.stderr.write("usage: node index.js <source> <destination>\n");
	process.exit(1);
}

var code = fs.readFileSync(process.argv[2], "utf8");

try {
	var pcode = parser.parse(code);
} catch(e) {
	process.stderr.write("Line " + e.location.start.line + ", column " + e.location.start.column + ": " + e.message + "\n");
	process.exit(2);
}

var compiler = new Compiler(pcode);

console.log(compiler.compile());

fs.writeFileSync(process.argv[3], Buffer.from(compiler.assemble()));

//console.log(pcode);