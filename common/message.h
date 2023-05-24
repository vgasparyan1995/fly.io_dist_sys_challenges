#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "graph_utils.h"

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
  size_t id = 0;
};

struct Broadcast {
  int message = 0;
};
struct BroadcastOk {};

struct Read {};
struct ReadOk {
  std::vector<int> messages;
};

struct Topology {
  AdjacencyList<std::string> topology;
};
struct TopologyOk {};

using Body =
    std::variant<Init, InitOk, Echo, EchoOk, Generate, GenerateOk, Broadcast,
                 BroadcastOk, Read, ReadOk, Topology, TopologyOk>;

struct Message {
  std::string src;
  std::string dest;
  std::optional<int> msg_id;
  std::optional<int> in_reply_to;
  Body body;
};
