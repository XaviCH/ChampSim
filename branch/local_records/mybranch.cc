#include "mybranch.h"

bool mybranch::predict(const mybranch::occurency_entry& entry) {
  assert(entry.counter <= RECORD_BLOCK_SIZE);

  bool prediction       = entry.history       [HISTORY_SIZE-RECORD_BLOCK_SIZE];
  bool last_prediction  = entry.history       [HISTORY_SIZE-RECORD_BLOCK_SIZE+1];
  bool repeated         = entry.equal_record  [RECORD_SIZE-1];
  bool is_last_in_block = (entry.counter == RECORD_BLOCK_SIZE-1);
  
  bool is_repeating     = entry.counter;
  for(int i=2; i<entry.counter; ++i) {
    is_repeating &= entry.history[HISTORY_SIZE-RECORD_BLOCK_SIZE+i];
  }

  if (is_repeating) {
    if (repeated) return last_prediction;
    else if (is_last_in_block) return !last_prediction;
  }

  return prediction;

}

bool mybranch::predict_branch(champsim::address ip)
{
  auto found = occurency_table[hash(ip)];

  if (found.has_value()) {
    return predict(found.value());
    /*
    auto [local, equal, counter] = found.value();
    bool repeated        = equal[63];        // 1
    bool prediction      = local[60];        // 0

    std::bitset<4> last; 
    last[3] = local[60]; // 0
    last[2] = local[61]; // 1
    last[1] = local[62]; // 0
    last[0] = local[63]; // 1
    return prediction;

    if (counter == 0) return prediction;

    if (counter == 1) {
      return repeated ? last[2] : prediction;
    }

    if (counter == 2) {
      if (repeated) {
        return last[2] == last[1] ? last[2] : prediction;
      }
      return prediction;
    }

    if (counter == 3) {
      if (repeated) {
        return last[2] == last[1] && last[2] == last[0] ? last[2] : prediction;
      }
      return last[2] == last[1] && last[2] == last[0] ? !last[2] : prediction;
    }

    assert(counter <= RECORD_BLOCK_SIZE);
    */
  }

  return 0;
}

void mybranch::last_branch_result(champsim::address ip, champsim::address branch_target, bool taken, uint8_t branch_type)
{

  auto& value = occurency_table[hash(ip)];
  if (value.has_value()) {
    value->counter += 1;
    if (value->counter == RECORD_BLOCK_SIZE) {

      bool is_repeating     = 1;
      for(int i=0; i<RECORD_BLOCK_SIZE-1; ++i) {
        is_repeating &= 
          value->equal_record[HISTORY_SIZE-1-i] == value->equal_record[HISTORY_SIZE-2-i];
      }

      value->counter          =   0;
      value->equal_record     <<= 1;
      value->equal_record[0]  =   is_repeating;
    }
    value->history    <<= 1;
    value->history[0]   = taken;
  } else {
    value = (occurency_entry){.history=taken, .equal_record=0};
  }
}
