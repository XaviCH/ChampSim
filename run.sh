TEST_DIR=~/Descargas

PREF_TESTS=(437.leslie3d-134B.champsimtrace.xz 435.gromacs-111B.champsimtrace.xz)
BRANCH_TESTS=(400.perlbench-41B.champsimtrace.xz 401.bzip2-277B.champsimtrace.xz)

echo Testing Prefetcher...
echo Testing Branch...


GLOBAL_HISTORY_LENGTH_VALUES=(32 64 16)
IP_HISTORY_TABLE_SIZE_VALUES=(2048 1024 4096)
TENDENCY_BITS_VALUES=(2 3 4)
GS_HISTORY_TABLE_SIZES=(524288 262144 131072)

for GLOBAL_HISTORY_LENGTH_VALUE in ${GLOBAL_HISTORY_LENGTH_VALUES[@]}
do
    for IP_HISTORY_TABLE_SIZE_VALUE in ${IP_HISTORY_TABLE_SIZE_VALUES[@]}
    do
        for TENDENCY_BITS_VALUE in ${TENDENCY_BITS_VALUES[@]}
        do
            for GS_HISTORY_TABLE_SIZE in ${GS_HISTORY_TABLE_SIZES[@]}
            do
                ./config.sh mybranch_config.json
                make CXXFLAGS="-D'GLOBAL_HISTORY_LENGTH_VALUE=$GLOBAL_HISTORY_LENGTH_VALUE' -DIP_HISTORY_TABLE_SIZE_VALUE=$IP_HISTORY_TABLE_SIZE_VALUE -DTENDENCY_BITS_VALUE=$TENDENCY_BITS_VALUE -DGS_HISTORY_TABLE_SIZE=$GS_HISTORY_TABLE_SIZE"
                echo GLOBAL_HISTORY_LENGTH_VALUE=$GLOBAL_HISTORY_LENGTH_VALUE IP_HISTORY_TABLE_SIZE_VALUE=$IP_HISTORY_TABLE_SIZE_VALUE TENDENCY_BITS_VALUE=$TENDENCY_BITS_VALUE GS_HISTORY_TABLE_SIZE=$GS_HISTORY_TABLE_SIZE >> log.txt
                for TEST in ${BRANCH_TESTS[@]}
                do
                    echo $TEST >> log.txt
                    bin/champsim --warmup_instructions 20000000 --simulation_instructions 10000000 $TEST_DIR/$TEST | grep "CPU 0 Branch Prediction Accuracy:" >> log.txt
                done
            done
        done
    done
done

