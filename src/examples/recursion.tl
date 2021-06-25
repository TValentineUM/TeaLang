int fib(a: int){
  if (a < 2) {
    return a;
  }
  return fib(a - 1) + fib(a - 2);
}

print fib(7);
print fib(20);
print fib(30);
