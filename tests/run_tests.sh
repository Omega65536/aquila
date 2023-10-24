#!/bin/bash
cd $(dirname $0)

for name in *.aq
do
    ../src/aquila "./$name" > "$name.out"
    diff -s "${name%.aq}.ref" "$name.out"
    rm -f "$name.out"
done
