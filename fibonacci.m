import "io";
fib(n) = {
    a = 1;
    b = 1;
    for(i = 3; i <= n; i = i + 1) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}
for(i = 0; i < 1000; i = i + 1) {
    print(fib(i))
}
