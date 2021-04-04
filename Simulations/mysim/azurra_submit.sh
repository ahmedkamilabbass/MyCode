#!/bin/bash

cd build
nohup mpirun -np 64 ./dsim -e -n F -s 71463251
nohup mpirun -np 64 ./dsim -e -n G -s 65848557
nohup mpirun -np 64 ./dsim -e -n H -s 85858418



