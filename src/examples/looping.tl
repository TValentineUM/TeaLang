string OverUnder50(age: int){
    if(age<50){
        return "UnderFifty";
    }else{
        return "OverFifty";
    }
}

let z:int = 45;

while(z<50){
	print OverUnder50(z); //"UnderFifty"x5
	z=z+1;
}

print OverUnder50(z); //"OverFifty"

for(let loopvar:int = 0; loopvar<5; loopvar=loopvar+1){
	print loopvar;
}
