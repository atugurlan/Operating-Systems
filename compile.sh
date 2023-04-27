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
gcc -o compile -Wall "$1" > 1 2>&1

errors=$(grep -c 'error' 1)
warnings=$(grep -c 'warning' 1)
echo "Number of errors $errors"
echo "Number of warnings $warnings"
