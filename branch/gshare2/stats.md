# BENCHMARK
## Config Options
- GLOBAL_HISTORY_LENGTH = 14;
- COUNTER_BITS = 2;
- GS_HISTORY_TABLE_SIZE = 16384;
- IP_HISTORY_TABLE_SIZE = 1024;
- TENDENCY_BITS = 3;
## Execution Data
bin/champsim --warmup_instructions 200000000 --simulation_instructions 100000000 ~/Descargas/400.perlbench-41B.champsimtrace.xz
CPU 0 cumulative IPC: 1.304 instructions: 100000002 cycles: 76677203
CPU 0 Branch Prediction Accuracy: 96.23% MPKI: 7.773 Average ROB Occupancy at Mispredict: 72.96
Branch type MPKI
BRANCH_DIRECT_JUMP: 0.00133
BRANCH_INDIRECT: 0.3474
BRANCH_CONDITIONAL: 7.411
BRANCH_DIRECT_CALL: 0.00328
BRANCH_INDIRECT_CALL: 0.00805
BRANCH_RETURN: 0.00201

bin/champsim --warmup_instructions 200000000 --simulation_instructions 100000000 ~/Descargas/459.GemsFDTD-765B.champsimtrace.xz
CPU 0 cumulative IPC: 0.6294 instructions: 100000004 cycles: 158871101
CPU 0 Branch Prediction Accuracy: 99.48% MPKI: 0.0256 Average ROB Occupancy at Mispredict: 247.8
Branch type MPKI
BRANCH_DIRECT_JUMP: 2e-05
BRANCH_INDIRECT: 0
BRANCH_CONDITIONAL: 0.02558
BRANCH_DIRECT_CALL: 0
BRANCH_INDIRECT_CALL: 0
BRANCH_RETURN: 0

##
CPU 0 runs /home/xavier/Descargas/602.gcc_s-2375B.champsimtrace.xz

Region of Interest Statistics

CPU 0 cumulative IPC: 0.8834 instructions: 500000003 cycles: 565988508
CPU 0 Branch Prediction Accuracy: 88.56% MPKI: 24.51 Average ROB Occupancy at Mispredict: 19.61
Branch type MPKI
BRANCH_DIRECT_JUMP: 0.6668
BRANCH_INDIRECT: 0.6643
BRANCH_CONDITIONAL: 22.61
BRANCH_DIRECT_CALL: 0.2989
BRANCH_INDIRECT_CALL: 0.08021
BRANCH_RETURN: 0.1912

Last Tendency Balance
Tendency balance: 0.239716.
Tendency balance: 0.274567.
Tendency balance: 0.242035.
Tendency balance: 0.265274.
Tendency balance: 0.261185.
Tendency balance: 0.294037.
Tendency balance: 0.244781.
Tendency balance: 0.293289.
Tendency balance: 0.267380.
Tendency balance: 0.214294.