# BENCHMARK
## Config Options
- GLOBAL_HISTORY_LENGTH = 14;
- COUNTER_BITS = 2;
- GS_HISTORY_TABLE_SIZE = 16384;
- IP_HISTORY_TABLE_SIZE = 1024;
- TENDENCY_BITS = 3;
## Execution Data
bin/champsim --warmup_instructions 200000000 --simulation_instructions 100000000 ~/Descargas/400.perlbench-41B.champsimtrace.xz
CPU 0 cumulative IPC: 1.321 instructions: 100000002 cycles: 75677784
CPU 0 Branch Prediction Accuracy: 96.49% MPKI: 7.22 Average ROB Occupancy at Mispredict: 78.94
Branch type MPKI
BRANCH_DIRECT_JUMP: 0.00133
BRANCH_INDIRECT: 0.3474
BRANCH_CONDITIONAL: 6.858
BRANCH_DIRECT_CALL: 0.00328
BRANCH_INDIRECT_CALL: 0.00805
BRANCH_RETURN: 0.00201

bin/champsim --warmup_instructions 200000000 --simulation_instructions 100000000 ~/Descargas/459.GemsFDTD-765B.champsimtrace.xz
CPU 0 cumulative IPC: 0.6294 instructions: 100000004 cycles: 158883101
CPU 0 Branch Prediction Accuracy: 99.48% MPKI: 0.02582 Average ROB Occupancy at Mispredict: 247.3
Branch type MPKI
BRANCH_DIRECT_JUMP: 2e-05
BRANCH_INDIRECT: 0
BRANCH_CONDITIONAL: 0.0258
BRANCH_DIRECT_CALL: 0
BRANCH_INDIRECT_CALL: 0
BRANCH_RETURN: 0