TEST_DIR=~/Descargas
EXP1_DIR=myl1pref_exp1_logs
EXP2_DIR=myl1pref_exp2_logs
EXP3_DIR=myl1pref_exp3_logs

PREF_TESTS=(437.leslie3d-273B.champsimtrace.xz 435.gromacs-228B.champsimtrace.xz 410.bwaves-2097B.champsimtrace.xz 453.povray-887B.champsimtrace.xz)
BRANCH_TESTS=(400.perlbench-41B.champsimtrace.xz 401.bzip2-277B.champsimtrace.xz)

PREF_CONFIGS=(mypref_config.json ip_stride_config.json next_line_config.json spp_dev_config.json va_ampm_lite_config.json)

echo Testing Prefetcher...

ONLY_SAME_PAGE_VALUES=(0 1)
MISS_RELATED_STRIDE_SIZE_VALUES=(1 2 3 4)
TABLE_SETS_VALUES=(256)
TABLE_WAYS_VALUES=(16)
ONLY_PREFETCH_ON_MISS_VALUES=(0 1)

mkdir -p $EXP1_DIR

for ONLY_SAME_PAGE_VALUE in ${ONLY_SAME_PAGE_VALUES[@]}
do
    for ONLY_PREFETCH_ON_MISS_VALUE in ${ONLY_PREFETCH_ON_MISS_VALUES[@]}
    do
        make clean
        ./config.sh mypref_config.json
        make CPPFLAGS="-D'ONLY_SAME_PAGE_VALUE=$ONLY_SAME_PAGE_VALUE' -D'ONLY_PREFETCH_ON_MISS_VALUE=$ONLY_PREFETCH_ON_MISS_VALUE'"
        for TEST in ${PREF_TESTS[@]}
        do
            bin/champsim --warmup_instructions 200000000 --simulation_instructions 100000000 $TEST_DIR/$TEST | grep -E "cpu0->cpu0_L1D PREFETCH REQUESTED|CPU 0 cumulative IPC" >> $EXP1_DIR/$TEST.$ONLY_SAME_PAGE_VALUE.$ONLY_PREFETCH_ON_MISS_VALUE.log &
        done
    done
done

mkdir -p $EXP2_DIR

for MISS_RELATED_STRIDE_SIZE_VALUE in ${MISS_RELATED_STRIDE_SIZE_VALUES[@]}
do
    make clean
    ./config.sh mypref_config.json
    make CPPFLAGS="-D'MISS_RELATED_STRIDE_SIZE_VALUE=$MISS_RELATED_STRIDE_SIZE_VALUE'"
    for TEST in ${PREF_TESTS[@]}
    do
        bin/champsim --warmup_instructions 200000000 --simulation_instructions 100000000 $TEST_DIR/$TEST | grep -E "cpu0->cpu0_L1D PREFETCH REQUESTED|CPU 0 cumulative IPC" >> $EXP2_DIR/$TEST.$MISS_RELATED_STRIDE_SIZE_VALUE.log &
    done
done

mkdir -p $EXP3_DIR
make clean

for PREF_CONFIG in ${PREF_CONFIGS[@]}
do
    ./config.sh $PREF_CONFIG
    make
    for TEST in ${PREF_TESTS[@]}
    do
        bin/champsim --warmup_instructions 200000000 --simulation_instructions 100000000 $TEST_DIR/$TEST | grep -E "cpu0->cpu0_L1D PREFETCH REQUESTED|CPU 0 cumulative IPC" >> $EXP3_DIR/$TEST.$PREF_CONFIG.log &
    done
done

