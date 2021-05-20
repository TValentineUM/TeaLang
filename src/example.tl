float Square(x:float){
      return x*x;
}

bool XGreaterThanY(x: float, y:float) {
     let ans:bool = true;
     if (y>x) { ans = false; }
     return ans;
}

bool XGreaterThanYv2(x:float, y:float){
     return x > y;
}

float AverageOfThree (x:float, y:float, z:float){
    let total:float = x+y+z;
    return total/3.0;
}

string JoinStr(s1:string, s2:string){
    let s3:string = s1 + s2;
    return s3;
}

int fib(a: int){
  if (a < 2) {
    return a;
  }
  return fib(a - 1) + fib(a - 2);
}

let x:float = 2.4;
let y:float = Square(2.5);
print y;                        //6.25
print XGreaterThanY(x,2.3);     //true
print XGreaterThanYv2(Square(1.5),y); // false
print AverageOfThree(x,y,1.2);        //3.28
print JoinStr("Hello", " World");     //Hello World

print fib(7);

string OverUnder50(age: int){
    if(age<50){
        return "UnderFifty";
   }else{
        return "OverFifty";    }
}


for(let loopvar:int = 0; loopvar<10; loopvar=loopvar+1){
	print loopvar;
}



let z:int=45;

while(z<50){
	print OverUnder50(z); //"UnderFifty"x5
	z=z+1;
}

float Sum(toAverage[]:float, count:int){
    let total: float = 0.0;
    for (let i:int = 0; i<count; i = i+1){
          total = total + toAverage[i];
    }
    return total;
}



print OverUnder50(z); //"OverFifty"



float Pow(x:float, n:int){
    let y:float = 1.0;
    if(n>0){
        for(;n>0;n=n-1){
            y = y * x;
        }
    }else{
        for(;n<0;n=n-1){
            y = y/x;
        }
    }
    return y;
}



let temp1:char = 'x';



let temp2[4]:int = {1,2,3,4};


print y;                        //6.25

temp2[2+1] = 2;


print x; //printstoconsole 1667.988
