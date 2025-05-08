#ifndef BRANCH_MYBRANCH_H
#define BRANCH_MYBRANCH_H

#include <array>

#include "address.h"
#include "modules.h"
#include "champsim.h"
#include "modules.h"
#include "msl/lru_table.h"

/*

if max < threshold
ip -> max, count 
count 


*/

class mybranch : champsim::modules::branch_predictor
{

  static constexpr int threshold = 15;
  constexpr static std::size_t  TABLE_SETS                = 256;
  constexpr static std::size_t  TABLE_WAYS                = 4;

  [[nodiscard]] static constexpr auto hash(champsim::address ip) { return ip.to<unsigned long>() % PRIME; }

  static constexpr std::size_t TABLE_SIZE = 16384;
  static constexpr std::size_t PRIME = 16381;
  static constexpr std::size_t BITS = 2;

  struct ip_counter_entry {
    champsim::address ip;
    uint16_t max_count;
    uint16_t count;

    auto index() const
    {
      using namespace champsim::data::data_literals;

      return ip.slice_upper<2_b>();
    }
    auto tag() const
    {
      using namespace champsim::data::data_literals;
      
      return ip.slice_upper<2_b>();
    }
  };

  champsim::msl::lru_table<ip_counter_entry> ip_counter_table{TABLE_SETS, TABLE_WAYS};


public:
  using branch_predictor::branch_predictor;

  // void initialize_branch_predictor();
  bool predict_branch(champsim::address ip);
  void last_branch_result(champsim::address ip, champsim::address branch_target, bool taken, uint8_t branch_type);
};

#endif
