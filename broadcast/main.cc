#include <iostream>
#include <set>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <variant>

#include "broadcaster.h"
#include "common/graph_utils.h"
#include "common/maelstrom_node.h"
#include "common/message.h"

AdjacencyList<std::string> ConstructTopology(MaelstromNode& maelstrom_node) {
  auto topology_msg = maelstrom_node.Receive();
  if (!topology_msg) {
    return {};
  }
  auto* topology = std::get_if<Topology>(&topology_msg->body);
  if (!topology) {
    return {};
  }
  std::set<Edge<std::string>> edges;
  for (const auto& [v, us] : topology->topology) {
    for (const auto& u : us) {
      if (v > u) {
        continue;
      }
      edges.insert({v, u});
    }
  }
  topology_msg->body = TopologyOk{};
  maelstrom_node.Send(*topology_msg);
  return MinimumSpanningTree(std::move(edges));
}

int main() {
  MaelstromNode node;
  if (!node.Initialize()) {
    std::cerr << "Failed initializing the node.\n";
    return -1;
  }

  auto node_graph = ConstructTopology(node);
  if (node_graph.empty()) {
    std::cerr << "Failed constructing topology.\n";
    return -1;
  }

  std::unordered_set<int> messages;
  std::unordered_map<std::string, Broadcaster> broadcasters;
  for (const auto& neighbor : node_graph[node.Id()]) {
    broadcasters.emplace(std::piecewise_construct,
                         std::forward_as_tuple(neighbor),
                         std::forward_as_tuple(node, neighbor));
  }

  while (true) {
    auto msg = node.Receive();
    if (!msg) {
      continue;
    }
    if (auto* broadcast = std::get_if<Broadcast>(&msg->body)) {
      messages.insert(broadcast->message);
      for (auto& [_, broadcaster] : broadcasters) {
        std::vector<int> messages = {broadcast->message};
        broadcaster.AddMessages(messages);
      }
      msg->body = BroadcastOk{};
      node.Send(*msg);
    } else if (auto* bulk_broadcast = std::get_if<BulkBroadcast>(&msg->body)) {
      messages.insert(bulk_broadcast->messages.begin(),
                      bulk_broadcast->messages.end());
      for (auto& [node, broadcaster] : broadcasters) {
        if (node == msg->src) {
          continue;
        }
        broadcaster.AddMessages(bulk_broadcast->messages);
      }
      msg->body = BroadcastOk{};
      node.Send(*msg);
    } else if (std::get_if<BroadcastOk>(&msg->body)) {
      if (auto it = broadcasters.find(msg->src); it != broadcasters.end()) {
        it->second.BroadcastReceived(msg->in_reply_to.value_or(-1));
      }
    } else if (std::get_if<Read>(&msg->body)) {
      msg->body = ReadOk{
          .messages = std::vector<int>(messages.begin(), messages.end())};
      node.Send(*msg);
    } else {
      std::cerr << "Unexpected message.\n";
    }
  }
}
