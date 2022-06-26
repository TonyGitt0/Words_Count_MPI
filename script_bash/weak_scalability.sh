#!/bin/bash


rm -rf results/weak_scalability.csv

mpicc -c -g main.c utils.c
mpicc -o word_count main.o utils.o

echo "TIME,VCPU,FILES,WORDS" >> results/weak_scalability.csv


for ((p=1; p<=24; p=p+1))
do
    mpirun --allow-run-as-root -np $p --hostfile hostfile word_count $p >> results/weak_scalability.csv
done

cat results/weak_scalability.csv