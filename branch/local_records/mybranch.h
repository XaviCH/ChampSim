#ifndef BRANCH_MYBRANCH_H
#define BRANCH_MYBRANCH_H

#include <array>

#include "address.h"
#include "modules.h"
#include "champsim.h"
#include "modules.h"
#include <utility>
#include <bitset>
#include "msl/fwcounter.h"
#include "msl/lru_table.h"

/*
Common cases prediction:
  - loops
    - static:     last
    - tendency:   last - tendency
    - random:     always return 1
  - conditional blocks
Idea:
  - Detect wether we are in a loop case or conditional block
    - 
  - For loops try to know de avg len (maybe it is not need)
  

multiple biased tendency
  - bimodal holds for recent bias
  - add more layers to discuss the tendency
  0 - 3
  01 no take 23 take
  
  1111 - 1111
  
  1110 - 1111
  1100 - 1110
  1001 - 1110
  1011 - 1101

if max < threshold
ip -> max, count 
count 

ocurrency tables
    base       table: 64 bit of decisitions [111101111101111101111[0111][1110][0000]]
    equal      table: 64 bit of if 4 last are equal [10]
    prediction table: 2 1-0  
    // prediction table: 64 bit [11111101010101101111111101101]

*/

class mybranch : champsim::modules::branch_predictor
{

  [[nodiscard]] static constexpr auto hash(champsim::address ip) { return ip.to<unsigned long>() % PRIME; }

  static constexpr std::size_t TABLE_SIZE = 16384;
  static constexpr std::size_t PRIME = 16381;

  static constexpr std::size_t HISTORY_SIZE = 64;
  static constexpr std::size_t RECORD_SIZE = 64;
  static constexpr std::size_t RECORD_BLOCK_SIZE = 4;
  // static constexpr std::size_t BITS = 2;

  struct occurency_entry {
    std::bitset<HISTORY_SIZE>   history;
    std::bitset<RECORD_SIZE>    equal_record;
    int counter;
    // champsim::msl::fwcounter<2> counter;
  };

  std::array<std::optional<occurency_entry>, TABLE_SIZE> occurency_table;

  bool predict(const occurency_entry& entry);

public:
  using branch_predictor::branch_predictor;

  // void initialize_branch_predictor();
  bool predict_branch(champsim::address ip);
  void last_branch_result(champsim::address ip, champsim::address branch_target, bool taken, uint8_t branch_type);
};

#endif
