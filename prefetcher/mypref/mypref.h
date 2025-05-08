#ifndef PREFETCHER_MYPREF_H
#define PREFETCHER_MYPREF_H

#include <cstdint>
#include <optional>
#include <bitset>

#include "address.h"
#include "champsim.h"
#include "modules.h"
#include "msl/lru_table.h"

class mypref : public champsim::modules::prefetcher
{

  constexpr static bool         ONLY_SAME_PAGE            = true;
  constexpr static double       OCCUPANCY_THRESHOLD       = 0.5;
  constexpr static std::size_t  MISS_RELATED_STRIDE_SIZE  = 4;
  constexpr static std::size_t  TABLE_SETS                = 256;
  constexpr static std::size_t  TABLE_WAYS                = 4;

  struct miss_related_table_entry {

    champsim::address ip{};
    champsim::block_number::difference_type stride_queue[MISS_RELATED_STRIDE_SIZE];

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

  struct miss_related_pair {

    champsim::address ip{}, addr{};

  };

  champsim::msl::lru_table<miss_related_table_entry> miss_related_table{TABLE_SETS, TABLE_WAYS};
  std::optional<miss_related_table_entry> miss_related_lookahead;
  miss_related_pair last_missed_pairs[MISS_RELATED_STRIDE_SIZE];

public:
  using prefetcher::prefetcher;

  // void prefetcher_initialize() {}
  // void prefetcher_branch_operate(champsim::address ip, uint8_t branch_type, champsim::address branch_target) {}
  uint32_t prefetcher_cache_operate(champsim::address addr, champsim::address ip, uint8_t cache_hit, bool useful_prefetch, access_type type,
                                    uint32_t metadata_in);
  uint32_t prefetcher_cache_fill(champsim::address addr, long set, long way, uint8_t prefetch, champsim::address evicted_addr, uint32_t metadata_in);
  void prefetcher_cycle_operate();
  // void prefetcher_final_stats() {}
};

#endif
