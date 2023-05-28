#pragma once

#include <mutex>
#include <optional>
#include <string_view>

#include "message.h"

// Thread-safe.
class MaelstromNode {
 public:
  // The node waits for an "init" message to identify itself and the other
  // nodes. MaelstromNode should only be used after Initialize succeeds.
  bool Initialize();

  // Sends out a message. If `msg.dest` is the current node, `msg.src`,
  // `msg.dest` and `in_reply_to` fields will be set appropriately. Returns the
  // msg_id of the sent message.
  int Send(Message msg);

  // Blocks on STDIN to read a message.
  std::optional<Message> Receive();

  const std::string& Id() const { return id_; }

 private:
  std::mutex mu_send_;
  int msg_id_ = 0;  // Guarded by mu_send_
  std::string id_;
  std::vector<std::string> nodes_;
};
