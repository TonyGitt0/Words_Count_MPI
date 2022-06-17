#!/bin/bash


rm -rf results/strong_scalability.csv

mpicc -c -g main.c utils.c
mpicc -o word_count main.o utils.o

echo "TIME,VCPU,FILES,WORDS" >> results/strong_scalability.csv


for ((p=1; p<=24; p=p+1))
do
    mpirun --allow-run-as-root -np $p --hostfile hostfile word_count $p >> results/strong_scalability.csv
done

cat results/strong_scalability.csv