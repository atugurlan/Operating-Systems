#!bin/bash

#check for errors
if [ $# -ne 1 ]; then
    echo "wrong number of arguments"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "Error: $1 is not a file"
    exit 1
fi

#compile part
output=$(gcc -o compile -Wall "$1" 2>&1)

errors=$(echo "$output" | grep -c 'error' 1)
warnings=$(echo "$output" | grep -c 'warning' 1)

echo "$errors $warnings"
