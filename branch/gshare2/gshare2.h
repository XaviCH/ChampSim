#ifndef BRANCH_GSHARE_H
#define BRANCH_GSHARE_H

#include <array>
#include <bitset>

#include "modules.h"
#include "msl/fwcounter.h"

/*
  size = COUNTER_BITS*GS_HISTORY_TABLE_SIZE + GLOBAL_HISTORY_LENGTH * (1 + (TENDENCY_BITS*2 + 1) * IP_HISTORY_TABLE_SIZE)
  192 kB
  57352 Bytes

  gh & mask(ip) 
  if mask == 0 then gh no influye
  if mask == 1 then gh influyer

  11111
  0: 0->? 1->?
  por cada bit
    0-15, 0-15 para cuando bit gh ith es 1 o 0
    one - zero > 2 hay divergencia
    
    

*/
struct gshare2 : champsim::modules::branch_predictor {
  static constexpr std::size_t GLOBAL_HISTORY_LENGTH = 32;
  static constexpr std::size_t COUNTER_BITS = 2;
  static constexpr std::size_t GS_HISTORY_TABLE_SIZE = 524288;
  
  static constexpr std::size_t IP_HISTORY_TABLE_SIZE = 2048;
  static constexpr std::size_t TENDENCY_BITS = 3;

  /*
    size_ihe = TENDENCY_BITS*2*GLOBAL_HISTORY_LENGTH + GLOBAL_HISTORY_LENGTH
    ip
      gh[0]: [1] [0] ? taken ? 1 : -1 one or zero
      1:
      2:

  */
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
