#pragma once

#include <optional>
#include <string_view>

#include "message.h"

class MaelstromNode {
 public:
  // The node waits for an "init" message to identify itself and the other
  // nodes.
  bool Initialize();

  // Sends out a message. If `msg.dest` is the current node, `msg.src`,
  // `msg.dest` and `in_reply_to` fields will be set appropriately.
  void Send(Message msg);

  // Waits for a message.
  std::optional<Message> Receive();

  // Node's ID.
  std::string_view Id() const;

 private:
  int msg_id_ = 0;
  std::string id_;
  std::vector<std::string> nodes_;
};
