#include "mybranch.h"


std::size_t mybranch::ip_table_hash(champsim::address ip)
{
  std::size_t hash = ip.to<ulong>();

  return (hash >> 2) % IP_HISTORY_TABLE_SIZE;
}

std::size_t mybranch::gs_table_hash(champsim::address ip, std::bitset<GLOBAL_HISTORY_LENGTH> bh_vector, std::bitset<GLOBAL_HISTORY_LENGTH> mask)
{
  constexpr champsim::data::bits LOG2_HISTORY_TABLE_SIZE{champsim::lg2(GS_HISTORY_TABLE_SIZE)};
  constexpr champsim::data::bits LENGTH{GLOBAL_HISTORY_LENGTH};

  std::size_t hash = bh_vector.to_ullong() & mask.to_ullong();

  hash ^= ip.slice<LOG2_HISTORY_TABLE_SIZE, champsim::data::bits{}>().to<std::size_t>();
  hash ^= ip.slice<LOG2_HISTORY_TABLE_SIZE + LENGTH, LENGTH>().to<std::size_t>();
  hash ^= ip.slice<LOG2_HISTORY_TABLE_SIZE + 2 * LENGTH, 2 * LENGTH>().to<std::size_t>();

  return hash % GS_HISTORY_TABLE_SIZE;
}


bool mybranch::predict_branch(champsim::address ip)
{
  auto mask = ip_history_table[ip_table_hash(ip)].branch_mask;
  auto gs_hash = gs_table_hash(ip, branch_history_vector, mask);
  auto value = gs_history_table[gs_hash];
  return value.value() >= (value.maximum / 2);
}

void mybranch::last_branch_result(champsim::address ip, champsim::address branch_target, bool taken, uint8_t branch_type)
{
  // update gs history table
  auto mask = ip_history_table[ip_table_hash(ip)].branch_mask;
  auto gs_hash = gs_table_hash(ip, branch_history_vector, mask);
  gs_history_table[gs_hash] += taken ? 1 : -1;
  
  // update ip history table
  auto& tendency_table = ip_history_table[ip_table_hash(ip)].tendency_table;
  for(int bit=0; bit < GLOBAL_HISTORY_LENGTH; ++bit) {
    auto& entry = tendency_table[bit];
    // update counter
    auto& counter = branch_history_vector[bit] ? entry.one : entry.zero;
    counter += taken ? 1 : -1;
    // update mask
    ip_history_table[ip_table_hash(ip)].branch_mask[bit] = std::abs(entry.one.value() - entry.zero.value()) >= (entry.one.maximum / 2);
  }
  
  // update branch history vector
  branch_history_vector <<= 1;
  branch_history_vector[0] = taken;

  // debug
  /*
  if (call_counter && (call_counter % 2000000) == 0) {
    size_t ones = 0;
    for (auto ip_history : ip_history_table) {
      ones += ip_history.branch_mask.count();
    }
    printf("Tendency balance: %f.\n", (double) ones / (double) (GLOBAL_HISTORY_LENGTH*IP_HISTORY_TABLE_SIZE));
  }
  ++call_counter;
  */
}
