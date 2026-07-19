#!/bin/bash

# Usage: ./generate_numbers.sh <start_number> <num_lines> [output_file]

if [ $# -lt 2 ]; then
    echo "Usage: $0 <start_number> <num_lines> [output_file]"
    exit 1
fi

start_number=$1
num_lines=$2
output_file=${3:-numbers.txt}

current=$start_number

> "$output_file"  # Truncate/create the output file

for ((i=0; i<num_lines; i++)); do
    echo "$current" >> "$output_file"
    current=$((current + 2))
done

echo "Generated $num_lines numbers in '$output_file'."
