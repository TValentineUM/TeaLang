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
	let vTemp:Vector;
	vTemp.x = v1.x + v2.x;
	vTemp.y = v1.y + v2.y;
	vTemp.z = v1.z + v2.z;
	return vTemp;
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

let tempVar:int = v3.translate(1.0,1.0,1.0); // adds 1 to every element

print v3;

let v4:Vector = Add(v1,v3);

print v4;

tempVar = v1.scale(10.0);

print v1;
