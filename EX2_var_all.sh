#!/bin/bash

rm -rf data
rm -f log
echo "clean done"
mkdir data

echo "start to run 1"
./EX2_var1.sh

echo "start to run 2"
./EX2_var2.sh

echo "start to run 3"
./EX2_var3.sh

echo "all run task done, please check the result @data/"

