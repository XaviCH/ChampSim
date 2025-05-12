#ifndef BRANCH_GSHARE_H
#define BRANCH_GSHARE_H

#include <array>
#include <bitset>

#include "modules.h"
#include "msl/fwcounter.h"

struct gshare2 : champsim::modules::branch_predictor {
  static constexpr std::size_t GLOBAL_HISTORY_LENGTH = 14;
  static constexpr std::size_t COUNTER_BITS = 2;
  static constexpr std::size_t GS_HISTORY_TABLE_SIZE = 16384;
  static constexpr std::size_t IP_HISTORY_TABLE_SIZE = 1024;
  static constexpr std::size_t TENDENCY_BITS = 3;

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
