#!/usr/bin/env bash

cd ./src

for i in *.dot;
do dot "$i" -Tpng -o ${i%.dot}.png;
done

for i in *.png;
do mv "$i" "../$i";
done
