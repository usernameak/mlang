import "io";
b = 10;
a(a, b) = {
	return a+b+22;
};
c(s) = {
	return s+33;
};
a(b, 1);

if(1 == 1) {
	$ nf_output "yahooo";
} else {
	$ nf_output "fail";
};

$ nf_output a(b, 1);
$ nf_output c(b);
$ nf_output true;
$ nf_output false | true;
$ nf_output true & false;
$ nf_output "Hello, " + "world!";
print("HW2");
