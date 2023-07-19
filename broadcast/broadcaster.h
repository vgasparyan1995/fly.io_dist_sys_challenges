#pragma once

#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "common/maelstrom_node.h"
#include "common/message.h"

// Broadcaster periodically broadcasts given numbers to the specified node.
// Broadcaster is thread-safe, bound to a single node, and blocks on destruction
// to complete all broadcasting.
class Broadcaster {
 public:
  Broadcaster(MaelstromNode& maelstrom_node, NodeId dest_node);

  void AddNumbers(const std::vector<int>& numbers);
  void GossipReceived(MsgId in_reply_to);

 private:
  void DoGossip();

  MaelstromNode& maelstrom_node_;
  const NodeId dest_node_;
  std::jthread periodic_gossip_;
  std::mutex mu_;
  std::optional<std::pair<MsgId, Gossip>> last_gossip_;  // Guarded by mu_
  std::set<int> numbers_to_gossip_;                      // Guarded by mu_
};
