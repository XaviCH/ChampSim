#ifndef PREFETCHER_MYPREF_H
#define PREFETCHER_MYPREF_H

#include <cstdint>
#include <optional>
#include <bitset>

#include "address.h"
#include "champsim.h"
#include "modules.h"
#include "msl/lru_table.h"

#ifndef ONLY_SAME_PAGE_VALUE
#define ONLY_SAME_PAGE_VALUE 1 // 1 show a reduce use of the prefetcher
#endif

#ifndef MISS_RELATED_STRIDE_SIZE_VALUE
#define MISS_RELATED_STRIDE_SIZE_VALUE 2 // 2 or 3 display similar performance
#endif

#ifndef TABLE_SETS_VALUE
#define TABLE_SETS_VALUE 256 // low relevance 
#endif

#ifndef TABLE_WAYS_VALUE
#define TABLE_WAYS_VALUE 16 // low relevance
#endif

#ifndef ONLY_PREFETCH_ON_MISS_VALUE
#define ONLY_PREFETCH_ON_MISS_VALUE 1 // 1 show a reduce use of the prefetcher
#endif

// size = (8 + (page_size_in_bytes - block_size_in_bytes)*MISS_RELATED_STRIDE_SIZE)*(TABLE_SETS+TABLE_WAYS+1)
//         + MISS_RELATED_STRIDE_SIZE*16

class myl1pref : public champsim::modules::prefetcher
{

  constexpr static bool         ONLY_SAME_PAGE            = ONLY_SAME_PAGE_VALUE;
  constexpr static bool         ONLY_PREFETCH_ON_MISS     = ONLY_PREFETCH_ON_MISS_VALUE;
  constexpr static double       OCCUPANCY_THRESHOLD       = 0.5;
  constexpr static std::size_t  MISS_RELATED_STRIDE_SIZE  = MISS_RELATED_STRIDE_SIZE_VALUE;
  constexpr static std::size_t  TABLE_SETS                = TABLE_SETS_VALUE;
  constexpr static std::size_t  TABLE_WAYS                = TABLE_WAYS_VALUE;

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
