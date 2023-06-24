#pragma once

#include <optional>
#include <variant>
#include <vector>

#include "graph_utils.h"
#include "ids.h"

struct Init {
  NodeId node_id;
  std::vector<NodeId> node_ids;
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
  int number = 0;
};
struct BroadcastOk {};

struct Gossip {
  std::vector<int> numbers;
};
struct GossipOk {};

struct Read {};
struct ReadOk {
  std::vector<int> numbers;
};

struct Topology {
  AdjacencyList<NodeId> topology;
};
struct TopologyOk {};

using Body = std::variant<Init, InitOk, Echo, EchoOk, Generate, GenerateOk,
                          Broadcast, BroadcastOk, Gossip, GossipOk, Read,
                          ReadOk, Topology, TopologyOk>;

struct Message {
  NodeId src;
  NodeId dest;
  std::optional<MsgId> msg_id;
  std::optional<MsgId> in_reply_to;
  Body body;
};
