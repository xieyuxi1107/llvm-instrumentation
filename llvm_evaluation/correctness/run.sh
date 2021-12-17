PATH2LIB=~/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so        # Specify your build directory in the project
PASS=-fencePass                 # Choose either -fplicm-correctness or -fplicm-performance

# Delete outputs from previous run.
rm -f *.bc .*dot *.png ${1} ${1}.instrument

# Convert source code to bitcode (IR)
clang++ -emit-llvm -c ${1}.cpp -o ${1}.bc
# clang -emit-llvm -x c++ ${1}.cpp -o ${1}.bc
clang -emit-llvm -c print.c -o print.bc
llvm-link print.bc ${1}.bc -S -o=${1}.linked.bc


# # Canonicalize natural loops
# opt -loop-simplify ${1}.bc -o ${1}.ls.bc
# # Instrument profiler
# opt -pgo-instr-gen -instrprof ${1}.ls.bc -o ${1}.ls.prof.bc

# Generate binary executable
clang++ ${1}.bc -o ${1}

# Apply fence pass
opt-11 -load ${PATH2LIB} ${PASS} -o ${1}.instrument.bc < ${1}.linked.bc > /dev/null

# Generate binary executable after fence pass
# clang++ ${1}.instrument.bc -o ${1}.instrument

# Produce output from binary to check correctness
# lli ${1}.bc > no_instrument_output
# ./${1} > no_instrument_output
# lli ${1}.instrument.bc > instrument_output
# ./${1}.instrument > instrument_output

# Cleanup
# rm -f *.bc ${1} ${1}.instrument


# Instrument source code (*.cpp) without bitcode, create executatable test_fence
# clang -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so test_fence.cpp print.c -o test_fence
# ./test_fence > instrument_output