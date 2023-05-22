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
  while (true) {
    auto msg = node.Receive();
    if (!msg) {
      continue;
    }
    if (auto* broadcast = std::get_if<Broadcast>(&msg->body)) {
      numbers.insert(broadcast->message);
      msg->body = BroadcastOk{};
      node.Send(*msg);
    } else if (std::get_if<Read>(&msg->body)) {
      msg->body =
          ReadOk{.messages = std::vector<int>(numbers.begin(), numbers.end())};
      node.Send(*msg);
    } else if (auto* topology = std::get_if<Topology>(&msg->body)) {
      std::cerr << "Received the topology:\n";
      for (const auto& [node, neighbors] : topology->topology) {
        std::cerr << node << ": ";
        for (const auto& neighbor : neighbors) {
          std::cerr << neighbor << ", ";
        }
        std::cerr << '\n';
      }
      msg->body = TopologyOk{};
      node.Send(*msg);
    } else {
      std::cerr << "Unexpected message.\n";
    }
  }
}
