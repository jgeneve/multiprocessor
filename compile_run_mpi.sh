#!/bin/bash
mpicc -o hn blockwise_test.c
mpirun -n 4 ./hn
