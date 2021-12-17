rm -f test_compile_time*

NUM_RUNS=20

cleanup() {
    make clean
}

# # Alleviate caching for beginning accesses
# for i in $(seq 1 5); do
#     cleanup
#     (time ./${TEST}_normal input1/${INPUT}) >> /dev/null 2>> /dev/null;
# done


# compile time
for i in $(seq 1 $NUM_RUNS); do
    cleanup
    echo "Compile # $i"
    (time make) >> /dev/null 2>> test_compile_time.txt;
done

cat test_compile_time.txt | grep user > test_compile_time_user.txt

# average runs
python3 average_runs.py