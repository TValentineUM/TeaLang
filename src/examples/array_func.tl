int Sum(arr[]:int, count:int){
    let total: int = 0;
    for (let i:int = 0; i<count; i = i+1){
          total = total + arr[i];
    }
    return total;
}


let temp[4]:auto = {1,2,3,4};

print Sum(temp, 4);
