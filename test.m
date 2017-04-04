$ nativeload "./libmlang_stdlib.so";
b = 10;
a(a, b) = {
	return a+b+22;
};
c(s) = {
	return s+33;
};
a(b, 1);
$ nf_output a(b, 1);
$ nf_output c(b);