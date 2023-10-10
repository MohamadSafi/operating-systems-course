#!/bin/bash

# Compile the C program
gcc -pthread ex3.c -o ex3

# Define the values of n and m
n=10000000
m_values=(1 2 4 10 100)

# Create or truncate the results file
echo "n,m,execution_time" > ex3_res.txt

# Run the program with different values of m and measure execution time
for m in "${m_values[@]}"; do
    echo "Running with m = $m..."
    execution_time=$(time -p ./ex3 $n $m 2>&1 | grep real | awk '{print $2}')
    echo "$n,$m,$execution_time" >> ex3_res.txt
done

# Clean up the compiled program
rm ex3

echo "Execution completed."
