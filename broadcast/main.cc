#include <iostream>
#include <unordered_set>
#include <variant>

#include "common/maelstrom_node.h"

int main() {
  MaelstromNode node;
  if (!node.Initialize()) {
    std::cerr << "Failed initializing the node.\n";
    return -1;
  }

  std::unordered_set<int> numbers;
  std::unordered_set<std::string> neighbors;

  while (true) {
    auto msg = node.Receive();
    if (!msg) {
      continue;
    }
    if (auto* broadcast = std::get_if<Broadcast>(&msg->body)) {
      if (numbers.find(broadcast->message) == numbers.end()) {
        numbers.insert(broadcast->message);
        Message broadcast_along = {
            .src = std::string(node.Id()),
            .body = Broadcast{.message = broadcast->message}};
        for (const auto& neighbor : neighbors) {
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
      if (auto it = topology->topology.find(std::string(node.Id()));
          it != topology->topology.end()) {
        neighbors.insert(it->second.begin(), it->second.end());
      }
      msg->body = TopologyOk{};
      node.Send(*msg);
    } else {
      std::cerr << "Unexpected message.\n";
    }
  }
}
