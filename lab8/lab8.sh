#!/bin/bash

g++ -c lab8_s.cpp
g++ -o lab8_s lab8_s.o -lpthread

g++ -c lab8_c.cpp
g++ -o lab8_c lab8_c.o -lpthread
