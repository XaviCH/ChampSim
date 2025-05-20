TEST_DIR=~/Descargas

PREF_TESTS=(437.leslie3d-134B.champsimtrace.xz 435.gromacs-111B.champsimtrace.xz)
BRANCH_TESTS=(400.perlbench-41B.champsimtrace.xz 401.bzip2-277B.champsimtrace.xz)

echo Testing Prefetcher...
echo Testing Branch...


GLOBAL_HISTORY_LENGTH_VALUES=(32 16)
IP_HISTORY_TABLE_SIZE_VALUES=(2048 1024)
TENDENCY_BITS_VALUES=(2 3)
GS_HISTORY_TABLE_SIZE_VALUES=(524288 262144)

for GLOBAL_HISTORY_LENGTH_VALUE in ${GLOBAL_HISTORY_LENGTH_VALUES[@]}
do
    for IP_HISTORY_TABLE_SIZE_VALUE in ${IP_HISTORY_TABLE_SIZE_VALUES[@]}
    do
        for TENDENCY_BITS_VALUE in ${TENDENCY_BITS_VALUES[@]}
        do
            for GS_HISTORY_TABLE_SIZE_VALUE in ${GS_HISTORY_TABLE_SIZE_VALUES[@]}
            do
                ./config.sh mybranch_config.json
                make CXXFLAGS="-D'GLOBAL_HISTORY_LENGTH_VALUE=$GLOBAL_HISTORY_LENGTH_VALUE' -D'IP_HISTORY_TABLE_SIZE_VALUE=$IP_HISTORY_TABLE_SIZE_VALUE' -D'TENDENCY_BITS_VALUE=$TENDENCY_BITS_VALUE' -D'GS_HISTORY_TABLE_SIZE_VALUE=$GS_HISTORY_TABLE_SIZE_VALUE'"
                for TEST in ${BRANCH_TESTS[@]}
                do
                    echo GLOBAL_HISTORY_LENGTH_VALUE=$GLOBAL_HISTORY_LENGTH_VALUE IP_HISTORY_TABLE_SIZE_VALUE=$IP_HISTORY_TABLE_SIZE_VALUE TENDENCY_BITS_VALUE=$TENDENCY_BITS_VALUE GS_HISTORY_TABLE_SIZE_VALUE=$GS_HISTORY_TABLE_SIZE_VALUE >> $TEST.log.txt
                    bin/champsim --warmup_instructions 20 --simulation_instructions 10 $TEST_DIR/$TEST | grep "CPU 0 Branch Prediction Accuracy:" >> $TEST.log.txt &
                done
            done
        done
    done
done

