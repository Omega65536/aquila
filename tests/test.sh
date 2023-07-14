#!/bin/bash
cd $(dirname $0)

arr=("test_print" "test_math" "test_variables" "test_comparison" "test_if" "test_while")

for name in "${arr[@]}"
do
    ../src/aquila "./$name.aq" > "$name.out"
    diff -s "$name.ref" "$name.out"
    rm -f "$name.out"
done
