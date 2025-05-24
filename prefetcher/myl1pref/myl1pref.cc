#include "myl1pref.h"

#include "cache.h"

// 0:1 1:0 2:-1
// 3:-1 0:2
// 190:1

// ip st 0x1000 10 3
// 1000
// st 0x2000+4i 11 3
// 1000
// ip st 0x1020 10 3
// 2
// st 0x2020+4i 3
// 2
// ip st 0x1040 10

uint32_t myl1pref::prefetcher_cache_operate(champsim::address addr, champsim::address ip, uint8_t cache_hit, bool useful_prefetch, access_type type,
                                      uint32_t metadata_in)
{
  champsim::block_number cl_addr{addr};

  auto found = miss_related_table.check_hit((miss_related_table_entry){.ip=ip});
  
  if (!cache_hit) {

    // related miss with previous miss
    for (int i=0; i<MISS_RELATED_STRIDE_SIZE; ++i) {
      auto found_related = miss_related_table.check_hit((miss_related_table_entry){.ip=last_missed_pairs[i].ip});
      champsim::block_number cl_missed_addr{last_missed_pairs[i].addr};
      
      if (found_related.has_value()) {
        miss_related_table_entry update_entry = found_related.value();
        
        auto offset = champsim::offset(cl_missed_addr, cl_addr);

        int index = MISS_RELATED_STRIDE_SIZE-1;
        for (int i=0; i<MISS_RELATED_STRIDE_SIZE; ++i) {
          if (update_entry.stride_queue[i] == offset) {
            index = i;
            break;
          }
        }
        for (int i=index-1; i>=0; --i) {
          update_entry.stride_queue[i+1] = update_entry.stride_queue[i];
        }

        update_entry.stride_queue[0] = offset;
        miss_related_table.fill(update_entry);
      }
    }

    // update miss list
    int index = MISS_RELATED_STRIDE_SIZE-1;
    for (int i=0; i<MISS_RELATED_STRIDE_SIZE; ++i) {
      if (last_missed_pairs[i].ip == ip && last_missed_pairs[i].addr == addr) index = i;
    }
    for (int i=index-1; i>=0; --i)
      last_missed_pairs[i+1] = last_missed_pairs[i];
    last_missed_pairs[0] = {.ip=ip, .addr=addr};

  }
  
  if (!cache_hit || !ONLY_PREFETCH_ON_MISS) {

    if (found.has_value()) {
      miss_related_lookahead = found.value();
      miss_related_lookahead->ip=addr;
      miss_related_table.fill(found.value());
    } else if (!cache_hit) {
      miss_related_table.fill((miss_related_table_entry){.ip=ip});
    }
  }
    
  return metadata_in;
}

void myl1pref::prefetcher_cycle_operate()
{
  if (miss_related_lookahead.has_value()) {

    auto [addr, strides] = miss_related_lookahead.value();
    if (strides[0] != 0) {
      champsim::address pf_addr{champsim::block_number{addr} + strides[0]};
      bool success = true;
      if (champsim::page_number{pf_addr} == champsim::page_number{addr} || !myl1pref::ONLY_SAME_PAGE) {
        const bool mshr_under_light_load = intern_->get_mshr_occupancy_ratio() < myl1pref::OCCUPANCY_THRESHOLD;
        success = prefetch_line(pf_addr, mshr_under_light_load, 0);
      }

      if (success) {
        for (int i=0; i<MISS_RELATED_STRIDE_SIZE-1; ++i)
          miss_related_lookahead->stride_queue[i] = miss_related_lookahead->stride_queue[i+1];   
        miss_related_lookahead->stride_queue[MISS_RELATED_STRIDE_SIZE-1] = 0;
      }
    } else {
      miss_related_lookahead.reset();
    }
  }
}

uint32_t myl1pref::prefetcher_cache_fill(champsim::address addr, long set, long way, uint8_t prefetch, champsim::address evicted_addr, uint32_t metadata_in)
{ 
  return metadata_in;
}
