#include "broadcaster.h"

#include <algorithm>
#include <mutex>
#include <stop_token>
#include <string>
#include <thread>
#include <unordered_set>

#include "common/maelstrom_node.h"
#include "common/message.h"

namespace {

inline constexpr auto kBroadcastInterval = std::chrono::milliseconds(300);

}  // namespace

Broadcaster::Broadcaster(MaelstromNode& maelstrom_node, NodeId dest_node)
    : maelstrom_node_(maelstrom_node),
      dest_node_(dest_node),
      periodic_gossip_([this](std::stop_token stoken) {
        while (!stoken.stop_requested()) {
          DoGossip();
          std::this_thread::sleep_for(kBroadcastInterval);
        }
      }) {}

void Broadcaster::AddNumbers(const std::vector<int>& numbers) {
  std::unique_lock lck{mu_};
  numbers_to_gossip_.insert(numbers.begin(), numbers.end());
}

void Broadcaster::GossipReceived(MsgId in_reply_to) {
  std::unique_lock lck{mu_};
  if (!last_gossip_ || last_gossip_->first != in_reply_to) {
    return;
  }
  for (const auto number : last_gossip_->second.numbers) {
    numbers_to_gossip_.erase(number);
  }
  last_gossip_ = std::nullopt;
}

void Broadcaster::DoGossip() {
  std::unique_lock lck{mu_};
  if (numbers_to_gossip_.empty()) {
    return;
  }
  if (last_gossip_) {
    return;
  }
  Gossip gossip;
  std::copy(numbers_to_gossip_.begin(), numbers_to_gossip_.end(),
            std::back_inserter(gossip.numbers));
  const auto msg_id = maelstrom_node_.Send(
      Message{.src = maelstrom_node_.Id(), .dest = dest_node_, .body = gossip});
  last_gossip_ = {msg_id, std::move(gossip)};
}
