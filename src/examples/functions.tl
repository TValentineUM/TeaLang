float AverageOfThree (x:float, y:float, z:float){
    print "Float Average of three";
    let total:float = x+y+z;
    return total/3.0;
}

int AverageOfThree (x:int, y:int, z:int){
    print "Integer Average of three";
    let total:int = (x+y+z)/3;
    return total;
}

print AverageOfThree(10,20,45);

print AverageOfThree(12.5,7.5,30.5);
