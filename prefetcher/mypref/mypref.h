/*
Axiom
- A cache line may be related with a set of cache lines.

Consequences
- A cache line may be related with sequencials cache lines.
- A cache line may not be related with any cache line.

Idea:

Strong table
  - Unified table that shares space between sequential and random cache blocks
  - Represents the tendency of a program
  - LRU
Weak sequential table
  
Weak random table

Miss LRU table
hash(ip0) -> stride_lru_miss_table[]
hash(ip1) -> stride_lru_miss_table[]
hash(ip2) -> stride_lru_miss_table[]
hash(ip3) -> stride_lru_miss_table[]

ip(p0) != ip(p0)
[A3,A2] -> [A2,A1] -> [A0,A1] 
if next to add is already in the queue
write in the lru table
*/


#ifndef PREFETCHER_MYPREF_H
#define PREFETCHER_MYPREF_H

#include <cstdint>
#include <optional>
#include <bitset>

#include "address.h"
#include "champsim.h"
#include "modules.h"
#include "msl/lru_table.h"
/* MAIN IDEAS */
// Es importante no saturar el bus??
// Prefech depending on jump in loops         // 16 kb
// ip -> addr
//

// Table address to address LRU   // 32 kb
// 
//
//

// Prefetch degree variable ( hit rate )

// 1 hit -> 2,3,4
//                | 2,3,4
// Next line if no override data

// miss -> addr_miss, last_addr_miss -> (last_addr_m, addr_m), last_addr_miss := addr_m
//
//
/*
  Objects:
    IPtable:    stores ip and strides info for the sequence
    Rtable[N]:  stores pair of address (addr1, b(addr2))
    Otable:     store correlated pair of address

    empty state:
      I(ip, b(addr), 0, 3)
      R(addr, _)
    
    next:
      R(addr, b(addr2), degree) if (p(addr) == p(addr2))
      R(addr2, _)

      addr
      R(addr, b(addr2), degree) if (p(addr) == p(addr2))


    Rtable      Rtable      Rtable   | Otable
    (x,y)       (x,y)                | (x,y)

  tag: ip & addr

    LRU is ip?
      then:      LRU ip table  -> ip, b(addr), found.b - b(addr), (b(addr) - fond.b  == found.stride ? found.degree+1 : found.degree)
      otherwise: 
        LRU ip table  -> ip, b(addr), 0, 1
        if lookahead.degree > 0 then LRU 
    LRU recent addr -> addr & b(addr)

  tag: ip & addr


    LRU is addr?



*/
// M[prev_miss] <- addr
// prev_miss <- addr


/*
miss table
[ ip1 -> ip2, ip3, ip4 ]
[ ip2 -> ip3, ip4 ]
[ ip4 -> ]
*/
template <typename T, unsigned Sz>
class queue {

  T _data[Sz];
  int num;
  public:
  queue();

  void clear() { num=0; }
  bool empty() { return num == 0;}
  bool full() { return num == Sz;}

  T& last() { _data[num-1]; }

  std::optional<T> find(T& elem) {
    for(int i=0; i<num; ++i) {
      if (_data[i].tag() == elem.tag()) {
        return std::optional(_data[i]);
      }
    }
    return std::optional<T>();
  }  

  void insert(T& elem) {
    for(int i=0; i < num-1; ++i) {
      _data[i+1] = _data[i];
    }
    _data[0] = elem;
    if (num != Sz) ++num;
  }
};

class mypref : public champsim::modules::prefetcher
{

  constexpr static std::size_t MISS_RELATED_STRIDE_SIZE = 4;

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

  struct random_entry {

    champsim::address addr{};
    champsim::block_number::difference_type stride{};

    auto index() const
    {
      using namespace champsim::data::data_literals;

      return addr.slice_upper<2_b>();
    }
    auto tag() const
    {
      using namespace champsim::data::data_literals;
      
      return addr.slice_upper<2_b>();
    }
  };

  struct tracker_entry {

    enum entry_type {
      SEQUENTIAL, RANDOM
    };
    
    struct sequential_entry {
      champsim::block_number last_cl_addr{};                 // the last address accessed by this IP
      champsim::block_number::difference_type last_stride{}; // the stride between the last two addresses accessed by this IP
      uint8_t degree;
    };

    struct random_entry {
      champsim::block_number::difference_type stride{};
      uint8_t trust;   // trust on the decisition
    };
    
    // tag
    champsim::address addr{};                              // the IP we're tracking
    // data
    entry_type type;                                      


    auto index() const
    {
      using namespace champsim::data::data_literals;

      return addr.slice_upper<2_b>();
    }
    auto tag() const
    {
      using namespace champsim::data::data_literals;
      
      return addr.slice_upper<2_b>();
    }
  };

  struct lookahead_entry {
    champsim::address ip{};
    champsim::address address{};
    champsim::address::difference_type stride{};
    uint8_t degree = 0; // degree remaining
  };

  constexpr static std::size_t TRACKER_SETS = 256;
  constexpr static std::size_t TRACKER_WAYS = 4;
  constexpr static std::size_t RTABLE_NUM =  4;

  // std::optional<lookahead_entry> active_lookahead;
  // std::optional<random_entry> object_lookahead;
  // champsim::msl::lru_table<tracker_entry> table{TRACKER_SETS, TRACKER_WAYS};
  // champsim::msl::lru_table<random_entry> Otable{TRACKER_SETS, TRACKER_WAYS};
  // queue<random_entry, TRACKER_SETS> R1buffer;
  // queue<random_entry, TRACKER_SETS> R2buffer;
  // queue<random_entry, TRACKER_SETS> R3buffer;

  // random_entry R1_entry, R2_entry, R3_entry;

  /*
  miss related table
  [ip1 -> off[2,3,4]]
  [ip2 -> off[3,4]]
  [ip3 -> off[4]]
  [ip4 -> off[]]

  miss related lookahead
  (addr, off)

  last missed addr
  (ip1, addr1), ip2, ip3, ip4

  */
  champsim::msl::lru_table<miss_related_table_entry> miss_related_table{TRACKER_SETS, TRACKER_WAYS};
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
