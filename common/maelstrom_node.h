#pragma once

#include <optional>
#include <string_view>

#include "message.h"

class MaelstromNode {
public:
  bool Initialize();
  std::optional<Message> Receive();
  void Send(Message msg);

  std::string_view Id() const;

private:
  int msg_id_ = 0;
  std::string id_;
  std::vector<std::string> nodes_;
};
