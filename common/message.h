#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Init {
  std::string node_id;
  std::vector<std::string> node_ids;
};
struct InitOk {};

struct Echo {
  std::string echo;
};
struct EchoOk {
  std::string echo;
};

struct Generate {};
struct GenerateOk {
  size_t id;
};

using Body = std::variant<Init, InitOk, Echo, EchoOk, Generate, GenerateOk>;

struct Message {
  std::string src;
  std::string dest;
  std::optional<int> msg_id;
  std::optional<int> in_reply_to;
  Body body;
};
