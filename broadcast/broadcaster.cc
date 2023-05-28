#include "broadcaster.h"

#include <algorithm>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>

#include "common/maelstrom_node.h"
#include "common/message.h"

inline constexpr auto kBroadcastInterval = std::chrono::milliseconds(1000);

Broadcaster::Broadcaster(MaelstromNode& maelstrom_node, std::string dest_node)
    : maelstrom_node_(maelstrom_node), dest_node_(std::move(dest_node)) {
  Start();
}

Broadcaster::~Broadcaster() { Stop(); }

void Broadcaster::Start() {
  periodic_broadcaster_ = std::thread([this] {
    while (true) {
      std::this_thread::sleep_for(kBroadcastInterval);
      std::scoped_lock l{mu_buffer_};
      if (stopped_) {
        break;
      }
      if (buffer_.empty()) {
        continue;
      }
      const auto msg_id = maelstrom_node_.Send(
          Message{.src = maelstrom_node_.Id(),
                  .dest = dest_node_,
                  .body = BulkBroadcast{.messages = std::vector<int>(
                                            buffer_.begin(), buffer_.end())}});
      bulk_broadcast_history_.push({msg_id, buffer_});
    }
  });
}

void Broadcaster::Stop() {
  {
    std::scoped_lock l{mu_buffer_};
    stopped_ = true;
  }
  if (periodic_broadcaster_.joinable()) {
    periodic_broadcaster_.join();
  }
}

void Broadcaster::AddMessages(const std::vector<int>& messages) {
  std::scoped_lock l{mu_buffer_};
  buffer_.insert(messages.begin(), messages.end());
}

void Broadcaster::BroadcastReceived(int recv_msg_id) {
  std::scoped_lock l{mu_buffer_};
  while (!bulk_broadcast_history_.empty()) {
    const auto& [msg_id, buffer] = bulk_broadcast_history_.front();
    if (msg_id == recv_msg_id) {
      for (const auto msg : buffer) {
        buffer_.erase(msg);
      }
    }
    if (msg_id <= recv_msg_id) {
      bulk_broadcast_history_.pop();
      continue;
    }
    return;
  }
}
