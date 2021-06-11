tlstruct Vector {
	let x:float = 0.0;
	let y:float = 0.0;
	let z:float = 0.0;

	int scale(s:float){
		x = x * s;
		y = y * s;
		z = z * s;
		return 0;
	}

	int translate(tx:float, ty:float, tz:float){
		x = x + tx;
		y = y + tz;
		z = z + tz;
		return 0;
	}

}

auto Add(v1:Vector, v2:Vector){
	let v3:Vector;
	v3.x = v1.x + v2.x;
	v3.y = v1.y + v2.y;
	v3.z = v1.z + v2.z;
	return v3;
}

let v1:Vector;
v1.x = 1.0;
v1.y = 2.0;
v1.z = 2.0;

let v2:Vector;
v2.x = 2.0;
v2.y = 1.2;
v2.z = 0.0;

let v3:Vector = Add(v1,v2);
print v3.x; // 3
print v3.y; // 3.2
print v3.z; // 2

let tempVar:int = v3.translate(1.0,1.0,1.0);

let v4:Vector = Add(v1,v3);
print v4.x; // 3+1=     4
print v4.y; // 3.2+2=   5.2
print v4.z; // 2+2=     4

print v4;

tempVar = v1.scale(10.0);

print v1;

float Square(x:float){
      return x*x;
}

bool XGreaterThanY(x: float, y:float) {
     let ans:bool = true;
     if (y>x) { ans = false; }
     return ans;
}

auto XGreaterThanYv2(x:float, y:float){
     return x > y;
}

float AverageOfThree (x:float, y:float, z:float){
    let total:float = x+y+z;
    return total/3.0;
}

int AverageOfThree (x:int, y:int, z:int){
    let total:int = (x+y+z)/3;
    return total;
}

string JoinStr(s1:string, s2:string){
    let s3:string = s1 + s2;
    return s3;
}

auto fib(a: int){
  if (a < 2) {
    return a;
  }
  return fib(a - 1) + fib(a - 2);
}

let x:float = 2.4;
let y:float = Square(2.5);
print x;                        //2.4
print y;                        //6.25
print XGreaterThanY(x,2.3);     //true
print fib(10);

print XGreaterThanYv2(Square(1.5),y); // false
print AverageOfThree(x,y,1.2);        //3.28
print AverageOfThree(4,8,12);        //8
print JoinStr("Hello", " World");     //Hello World


auto OverUnder50(age: int){
    if(age<50){
        return "UnderFifty\"";
   }else{
        return "OverFifty";    }
}


for(let loopvar:int = 0; loopvar<10; loopvar=loopvar+1){
	print loopvar;
}



let z:auto=45;

while(z<50){
	print OverUnder50(z); //"UnderFifty"x5
	z=z+1;
}

int Sum(toAverage[]:int, count:int){
    let total: int = 0;
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



let temp2[4]:auto = {1,2,3,4};


print y;                        //6.25

temp2[2+1] = 7;


print temp1; //printstoconsole 1667.988

print temp2;

print Sum(temp2, 4);
