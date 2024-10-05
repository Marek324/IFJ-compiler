# Define the directory variable
bin="../test"
directory="./input"

# Loop through all files in the directory
for file in $directory/*; do
    test_name=$(basename "$file")
    echo $test_name

    # expected_output="./exp_output/$test_name"
    # echo $expected_output
    # if [ -f "$expected_output" ]; then
    #     echo "$expected_output is a file"
    # else
    #     echo "$expected_output is not a file"
    # fi

    # actual_output="./output/$test_name"
    # echo $actual_output

    # diff "$file" "$expected_output"
done
