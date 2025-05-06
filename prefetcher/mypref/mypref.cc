#include "mypref.h"

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

uint32_t mypref::prefetcher_cache_operate(champsim::address addr, champsim::address ip, uint8_t cache_hit, bool useful_prefetch, access_type type,
                                      uint32_t metadata_in)
{
  champsim::block_number cl_addr{addr};
  champsim::block_number::difference_type stride = 0;
  uint8_t degree = 1;

  auto foundR1 = R1buffer.find((random_entry){.addr=addr});
  auto foundR2 = R2buffer.find((random_entry){.addr=addr});
  auto foundR3 = R3buffer.find((random_entry){.addr=addr});
  auto foundO  = Otable.check_hit((random_entry){.addr=addr});

  // check if addr has correlated block on object table
  if (foundO.has_value()) {
    object_lookahead = {addr, foundO->stride};
  }

  int strideR1 = -1;
  int strideR2 = -2;
  int strideR3 = -3;
  if (foundR1.has_value()) strideR1 = foundR1->stride;
  if (foundR2.has_value()) strideR2 = foundR2->stride;
  if (foundR3.has_value()) strideR3 = foundR3->stride;

  int strideO = 0;
  if(strideR1 == strideR2) strideO = strideR1;
  if(strideR1 == strideR3) strideO = strideR1;
  if(strideR2 == strideR3) strideO = strideR2;

  if (strideO != 0) {
    Otable.fill(
      (random_entry){.addr=addr, .stride=strideO}
    );
  }

  if (R3_entry.stride != 0)
    R3buffer.insert(R3_entry);

  if (R2_entry.stride != 0) {
    if (R2buffer.full())
      R3_entry = R2buffer.last();
    R2buffer.insert(R2_entry);
  }

  if (R1_entry.addr != addr) {
    if (R1buffer.full())
      R2_entry = R1buffer.last();
    R1buffer.insert((random_entry){.addr=addr, .stride=champsim::offset(addr, R1_entry.addr)});
  }

  R1_entry.addr = addr;

  auto found = table.check_hit((tracker_entry){.addr=ip}); 

  // if we found a matching entry
  if (found.has_value()) {
    if (found->type==tracker_entry::entry_type::SEQUENTIAL) {

      stride = champsim::offset(found->sequential.last_cl_addr, cl_addr);
      degree = found->sequential.degree;
      
      if (active_lookahead.has_value()) {
        auto [pf_ip, pf_address, pf_stride, pf_degree] = active_lookahead.value();
        if (pf_ip == ip) {
          if (pf_address == addr && pf_degree == 0) {
            degree +=1;
            active_lookahead = {pf_ip, pf_address, pf_stride, 1};
          }
        } else {
          if (pf_degree != 0) {
            // update last entry of prefetch
            auto last_prefetch = table.check_hit(
              (tracker_entry){
                .type=tracker_entry::entry_type::SEQUENTIAL,
                .addr=pf_ip, 
                //.sequential={cl_addr, stride, degree}
              }
            );
            table.fill(
              (tracker_entry){
                .type=tracker_entry::entry_type::SEQUENTIAL,
                .addr=last_prefetch->addr, 
              .sequential={last_prefetch->sequential.last_cl_addr, last_prefetch->sequential.last_stride, last_prefetch->sequential.degree-1}
            }
            );
          }
        }
      } 
        // Initialize prefetch state unless we somehow saw the same address twice in
        // a row or if this is the first time we've seen this stride
        else if (stride != 0 && stride == found->sequential.last_stride)
        active_lookahead = {ip, champsim::address{cl_addr}, stride, degree}; // PREFETCH_DEGREE};
    } else {

    }
  }

  // update tracking set
  table.fill(
    (tracker_entry){
      .type=tracker_entry::entry_type::SEQUENTIAL,
      .addr=ip, 
      .sequential={cl_addr, stride, degree}
    }
  );

  return metadata_in;
}

void mypref::prefetcher_cycle_operate()
{
  // If a lookahead is active
  if (active_lookahead.has_value()) {
    auto [ip, old_pf_address, stride, degree] = active_lookahead.value();
    // assert(degree > 0);
    
    champsim::address pf_address{champsim::block_number{old_pf_address} + stride};

    // If the next step would exceed the degree or run off the page, stop
    if (intern_->virtual_prefetch || champsim::page_number{pf_address} == champsim::page_number{old_pf_address}) {
      // check the MSHR occupancy to decide if we're going to prefetch to this level or not
      const bool mshr_under_light_load = intern_->get_mshr_occupancy_ratio() < 0.5;
      const bool success = prefetch_line(pf_address, mshr_under_light_load, 0);
      if (success)
        active_lookahead = {ip, pf_address, stride, degree - 1};
      // If we fail, try again next cycle

      // if (active_lookahead->degree == 0) {
      //   active_lookahead.reset();
      // }
    } else {
      active_lookahead.reset();
    }
  }
}

uint32_t mypref::prefetcher_cache_fill(champsim::address addr, long set, long way, uint8_t prefetch, champsim::address evicted_addr, uint32_t metadata_in)
{
  // leer table
  // 0: addr1 <-- 1, 
  // 1: addr2 <-- 2, 4 
  // 2: addr3 <-- 3, 0
  // addr1 <-- addr2 M[addr1] <- addr2
  // addr2 <-- 
  
  return metadata_in;
}
