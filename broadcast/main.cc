#include <iostream>
#include <set>
#include <unordered_set>
#include <variant>

#include "common/graph_utils.h"
#include "common/maelstrom_node.h"

int main() {
  MaelstromNode node;
  if (!node.Initialize()) {
    std::cerr << "Failed initializing the node.\n";
    return -1;
  }

  std::unordered_set<int> numbers;
  AdjacencyList<std::string> node_graph;

  while (true) {
    auto msg = node.Receive();
    if (!msg) {
      continue;
    }
    if (auto* broadcast = std::get_if<Broadcast>(&msg->body)) {
      if (numbers.find(broadcast->message) == numbers.end()) {
        numbers.insert(broadcast->message);
        Message broadcast_along = {
            .src = node.Id(), .body = Broadcast{.message = broadcast->message}};
        for (const auto& neighbor : node_graph[node.Id()]) {
          if (neighbor == msg->src) {
            continue;
          }
          broadcast_along.dest = neighbor;
          node.Send(broadcast_along);
        }
      }
      msg->body = BroadcastOk{};
      node.Send(*msg);
    } else if (std::get_if<Read>(&msg->body)) {
      msg->body =
          ReadOk{.messages = std::vector<int>(numbers.begin(), numbers.end())};
      node.Send(*msg);
    } else if (auto* topology = std::get_if<Topology>(&msg->body)) {
      std::set<Edge<std::string>> edges;
      for (const auto& [v, us] : topology->topology) {
        for (const auto& u : us) {
          if (v > u) {
            continue;
          }
          edges.insert({v, u});
        }
      }
      node_graph = MinimumSpanningTree(std::move(edges));
      msg->body = TopologyOk{};
      node.Send(*msg);
    } else {
      if (std::get_if<BroadcastOk>(&msg->body)) {
        continue;
      }
      std::cerr << "Unexpected message.\n";
    }
  }
}
