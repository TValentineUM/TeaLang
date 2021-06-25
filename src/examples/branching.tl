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

let temp:float = Pow(2.1,10);
print temp; //printstoconsole 1667.988
