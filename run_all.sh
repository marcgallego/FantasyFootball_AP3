#!/bin/sh

for i in `seq 1 9`; do
    echo "\nJoc de proves $i"
    ./a.out benchs/data_base$i.txt benchs/query$i.txt benchs/solution$i.txt
    ./checker.MacOS benchs/data_base$i.txt benchs/query$i.txt benchs/solution$i.txt
    echo "_______________________________________________________________"
done
