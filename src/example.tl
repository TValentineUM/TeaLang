float Square(x:float){
      return x*x;
}

bool XGreaterThanY(x: float, y:float) {
     let ans:bool = true;
     if (y>x) { ans = false; }
     return ans;
}

!

bool XGreaterThanYv2(x:float, y:float){
     return x > y;
}


float AverageOfThree (x:float, y:float, z:float){
      let total:float = x+y+z;
      return total/3;
}

string JoinStr(s1:string, s2:string){
    let s3 = s1 + s2;
    return s3;
}

let x:float = 2.4;
let y:float = Square(2.5);
print y;                        //6.25
print XGreaterThanY(x,2.3);     //true
print XGreaterThanYv2(Square(1.5),y); // false
print AverageOfThree(x,y,1.2);        //3.28
print JoinStr("Hello", " World");     //Hello World
