#!/bin/bash

# Paths
binary="./test" 
input_dir="./tests/input"

# Counter for test cases
total_tests=0
passed_tests=0
failed_tests=0

for input_file in $input_dir/*; do
    test_name=$(basename "$input_file")
    
    actual_output="./tests/output/$test_name"
    expected_output="./tests/exp_output/$test_name"
    
    "$binary" "$input_file" "$actual_output"
    
    exit_code=$?
    
    total_tests=$((total_tests + 1))
    
    if diff -q "$actual_output" "$expected_output" >/dev/null; then
        echo "Test $test_name passed."
        passed_tests=$((passed_tests + 1))
    else
        echo "Test $test_name failed."
        diff "$actual_output" "$expected_output"
        failed_tests=$((failed_tests + 1))
    fi
done

# Summary of the test results
echo "======================================="
echo "Total tests run: $total_tests"
echo "Passed: $passed_tests"
echo "Failed: $failed_tests"
echo "======================================="
