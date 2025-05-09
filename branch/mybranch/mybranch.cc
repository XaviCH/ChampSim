#include "mybranch.h"

bool mybranch::predict_branch(champsim::address ip)
{
  /* 2 3 5 7 11 
  A 2 * 5  * 7   = 70
  B 2 * 3  * 11  = 66
  C 5 * 3  * 7   = 105

  A 3 * 11   33
  B 5 * 7    35
  C 2 * 13   26
  auto found = ip_counter_table.check_hit({.ip=ip});

  if (found.has_value()) {
    auto [ip, max, count] = found.value();
    ip_counter_table.fill({.ip=ip, .max_count=max, .count=(uint16_t)(count < max ? count+1 : 0)});
    return count < max;
  } else {
    ip_counter_table.fill({.ip=ip, .max_count=mybranch::threshold, .count=0});
  }

  return 1;
  */

  auto value = occurency_table[hash(ip.to<ulong>() ^ global_register.to_ulong())];

  if (value.has_value()) {
    auto [local, equal, counter] = value.value();
    bool repeated        = equal[63];        // 1
    bool prediction      = local[60];        // 0
    int fase = counter.value();
    std::bitset<4> last; 
    last[3] = local[60]; // 0
    last[2] = local[61]; // 1
    last[1] = local[62]; // 0
    last[0] = local[63]; // 1
    return prediction;

    if (fase == 0) return prediction;

    if (fase == 1) {
      return repeated ? last[2] : prediction;
    }

    if (fase == 2) {
      if (repeated) {
        return last[2] == last[1] ? last[2] : prediction;
      }
      return prediction;
    }

    if (fase == 3) {
      if (repeated) {
        return last[2] == last[1] && last[2] == last[0] ? last[2] : prediction;
      }
      return last[2] == last[1] && last[2] == last[0] ? !last[2] : prediction;
    }

    assert(fase <= 3);
  }

  return 0;
}

void mybranch::last_branch_result(champsim::address ip, champsim::address branch_target, bool taken, uint8_t branch_type)
{
  /*
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
  */
  auto& value = occurency_table[hash(ip.to<ulong>() ^ global_register.to_ulong())];
  global_register <<= 1;
  global_register[0] = taken;
  if (value.has_value()) {
    if (value->counter.value() == 3) {
      value->equal_history <<= 1;
      value->equal_history[0] = 
        value->local_history[63] == value->local_history[62] &&
        value->local_history[63] == value->local_history[61] &&
        value->local_history[63] == value->local_history[60];
      value->counter =  0;
    } else {
      value->counter += 1;
    }
    value->local_history    <<= 1;
    value->local_history[0]   = taken;
  } else {
    value = (occurency_entry){.local_history=taken, .equal_history=0};
  }
}
