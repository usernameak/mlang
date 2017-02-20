Runtime.prototype.createNumberObject = function(val) {
	return {
		type: "number",
		val: val
	}
}
Runtime.prototype.simplifyBinaryExpression = function(left, right, op, scope) {
	if(left.type == "binary_expression") {
		left = this.simplifyBinaryExpression(left.left, left.right, left.op);
	}
	if(right.type == "binary_expression") {
		right = this.simplifyBinaryExpression(right.left, right.right, right.op);
	}
	if(left.type == "variable") {
		left = scope
	}
	switch(op) {
		case "+":
			if(left.type == "number" && right.type == "number") {
				return {
					type: "number",
					val: left.val + right.val
				}
			}/* else if(right.type == "binary_expression" && left.type == "binary_expression") {
				if(right.op == "*" && left.op == "*") {
					var vleft = left.right.type == "variable" ? left.right : left.left;
					var vright = right.right.type == "variable" ? right.right : right.left;
					if(vleft.name == vright.name) {
						var nleft = left.right.type == "number" ? left.right : left.left;
						var nright = right.right.type == "number" ? right.right : right.left;
						return {
							type: "binary_expression",
							left: this.createNumberObject(nleft.val + nright.val),
							right: vleft,
							op: "*"
						}
					}
					
				}
			}*/
		break;
		case "-":
			if(left.type == "number" && right.type == "number") {
				return {
					type: "number",
					val: left.val - right.val
				}
			}/* else if(right.type == "binary_expression" && left.type == "binary_expression") {
				if(right.op == "*" && left.op == "*") {
					var vleft = left.right.type == "variable" ? left.right : left.left;
					var vright = right.right.type == "variable" ? right.right : right.left;
					if(vleft.name == vright.name) {
						var nleft = left.right.type == "number" ? left.right : left.left;
						var nright = right.right.type == "number" ? right.right : right.left;
						return {
							type: "binary_expression",
							left: this.createNumberObject(nleft.val - nright.val),
							right: vleft,
							op: "*"
						}
					}
					
				}
			}*/
		break;
		case "*":
			if(left.type == "number" && right.type == "number") {
				return {
					type: "number",
					val: left.val * right.val
				}
			}/* else if(left.type == "binary_expression" && right.type == "number") {
				var vleft = left.right.type == "variable" ? left.right : left.left;
				var nleft = left.right.type == "number" ? left.right : left.left;
				return {
					type: "binary_expression",
					left: this.createNumberObject(nleft.val * right.val),
					right: vleft,
					op: "*"
				}
			}*/
		break;
		case "/":
			if(left.type == "number" && right.type == "number") {
				return {
					type: "number",
					val: left.val / right.val
				}
			}
		break;
	}
	return {
		type: "binary_expression",
		left: left,
		right: right,
		op: op
	}
}
/*Runtime.prototype.simplifyEquation = function(equation) {
	var right = equation.righthand;
	var left = equation.lefthand;
	switch(left.type) {
		case "binary_expression":
			left = this.simplifyBinaryExpression(left.left, left.right, left.op);
		break;
	}
	switch(right.type) {
		case "binary_expression":
			right = this.simplifyBinaryExpression(right.left, right.right, right.op);
		break;
	}
	return {
		type: "equation_statement",
		lefthand: left,
		righthand: right
	}
}*/