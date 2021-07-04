auto fib(a: int){
  if (a < 2) {
    return a;
  }else{
    return fib(a - 1) + fib(a - 2);
  }
}

auto XGreaterThanY(x:float, y:float){
     return x > y;
}


let x:auto = 2.4;
let y:auto = 2.3;
print XGreaterThanY(x,y);     //true
print fib(12);
