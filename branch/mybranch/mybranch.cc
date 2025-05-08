#include "mybranch.h"

bool mybranch::predict_branch(champsim::address ip)
{
  auto found = ip_counter_table.check_hit({.ip=ip});

  if (found.has_value()) {
    auto [ip, max, count] = found.value();
    ip_counter_table.fill({.ip=ip, .max_count=max, .count=(uint16_t)(count < max ? count+1 : 0)});
    return count < max;
  } else {
    ip_counter_table.fill({.ip=ip, .max_count=mybranch::threshold, .count=0});
  }

  return 0;
}

void mybranch::last_branch_result(champsim::address ip, champsim::address branch_target, bool taken, uint8_t branch_type)
{
  auto found = ip_counter_table.check_hit({.ip=ip});
  if (found.has_value()) {
    auto [ip, max, count] = found.value();
    bool my_predict = count-1 < max;

    if (my_predict != taken) {
      if (taken) {
        max += 2;
      } else {
        max -= 1;
      }
      ip_counter_table.fill({.ip=ip, .max_count=max, .count=count});
    }

  }
}
