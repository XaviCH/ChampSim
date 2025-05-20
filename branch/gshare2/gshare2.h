#ifndef BRANCH_GSHARE_H
#define BRANCH_GSHARE_H

#ifndef GLOBAL_HISTORY_LENGTH_VALUE
#define GLOBAL_HISTORY_LENGTH_VALUE 32 
#endif

#ifndef COUNTER_BITS_VALUE
#define COUNTER_BITS_VALUE 2
#endif

#ifndef GS_HISTORY_TABLE_SIZE_VALUE
#define GS_HISTORY_TABLE_SIZE_VALUE 524288
#endif

#ifndef IP_HISTORY_TABLE_SIZE_VALUE
#define IP_HISTORY_TABLE_SIZE_VALUE 2048
#endif

#ifndef TENDENCY_BITS_VALUE
#define TENDENCY_BITS_VALUE 3
#endif

#include <array>
#include <bitset>

#include "modules.h"
#include "msl/fwcounter.h"

/*
  size = COUNTER_BITS*GS_HISTORY_TABLE_SIZE + GLOBAL_HISTORY_LENGTH * (1 + (TENDENCY_BITS*2 + 1) * IP_HISTORY_TABLE_SIZE)
  192 kB
  57352 Bytes
*/
struct gshare2 : champsim::modules::branch_predictor {
  static constexpr std::size_t GLOBAL_HISTORY_LENGTH = GLOBAL_HISTORY_LENGTH_VALUE;
  static constexpr std::size_t COUNTER_BITS = COUNTER_BITS_VALUE;
  static constexpr std::size_t GS_HISTORY_TABLE_SIZE = GS_HISTORY_TABLE_SIZE_VALUE;
  
  static constexpr std::size_t IP_HISTORY_TABLE_SIZE = IP_HISTORY_TABLE_SIZE_VALUE;
  static constexpr std::size_t TENDENCY_BITS = TENDENCY_BITS_VALUE;

  struct ip_history_entry { 
    
    struct bit_counter {
      champsim::msl::fwcounter<TENDENCY_BITS> zero, one;
    };
    
    std::bitset<GLOBAL_HISTORY_LENGTH> branch_mask;
    std::array<bit_counter, GLOBAL_HISTORY_LENGTH> tendency_table;
  };
  size_t call_counter = 0;
  
  std::bitset<GLOBAL_HISTORY_LENGTH> branch_history_vector;
  std::array<champsim::msl::fwcounter<COUNTER_BITS>, GS_HISTORY_TABLE_SIZE> gs_history_table;

  std::array<ip_history_entry, IP_HISTORY_TABLE_SIZE> ip_history_table;

  using branch_predictor::branch_predictor;

  static std::size_t ip_table_hash(champsim::address ip);
  std::size_t gs_table_hash(champsim::address ip, std::bitset<GLOBAL_HISTORY_LENGTH> bh_vector, std::bitset<GLOBAL_HISTORY_LENGTH> mask);

  bool predict_branch(champsim::address ip);
  void last_branch_result(champsim::address ip, champsim::address branch_target, bool taken, uint8_t branch_type);
};

#endif
