var fs = require("fs");
var parser = require("./parser");
var Compiler = require("./compiler");

if(process.argv.length != 4) {
	console.log("usage: node index.js <source> <destination>");
	process.exit(1);
}

var code = fs.readFileSync(process.argv[2], "utf8");

var pcode = parser.parse(code);

var compiler = new Compiler(pcode);

compiler.compile();

fs.writeFileSync(process.argv[3], Buffer.from(compiler.assemble()));

//console.log(pcode);