#pragma once

#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "common/maelstrom_node.h"
#include "common/message.h"

// Broadcaster periodically broadcasts given messages to the specified node.
// Broadcaster is thread-safe, bound to a single node, and blocks on destruction
// to complete all broadcasting.
class Broadcaster {
 public:
  Broadcaster(MaelstromNode& maelstrom_node, std::string dest_node);
  ~Broadcaster();

  void AddMessages(const std::vector<int>& messages);
  void BroadcastReceived(int recv_msg_id);

 private:
  void Start();
  void Stop();

  MaelstromNode& maelstrom_node_;
  const std::string dest_node_;
  std::thread periodic_broadcaster_;
  std::mutex mu_buffer_;
  std::unordered_set<int> buffer_;  // Guarded by mu_buffer_
  std::queue<std::pair<int, std::unordered_set<int>>>
      bulk_broadcast_history_;  // Guarded by mu_buffer_
  bool stopped_ = false;        // Guarded by mu_buffer_
};
