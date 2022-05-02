#!/bin/bash

touch myfile

g++ -c lab6_write.cpp
g++ -o lab6_write lab6_write.o -lpthread

g++ -c lab6_read.cpp
g++ -o lab6_read lab6_read.o -lpthread

