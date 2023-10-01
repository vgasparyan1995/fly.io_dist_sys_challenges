#include <algorithm>
#include <iostream>
#include <mutex>
#include <random>
#include <set>
#include <thread>
#include <tuple>
#include <utility>
#include <variant>

#include "common/graph_utils.h"
#include "common/maelstrom_node.h"
#include "common/message.h"

inline constexpr auto kGossipInterval = std::chrono::milliseconds(50);

int Random(int from, int to) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(from, to);
  return dist(gen);
}

std::optional<AdjacencyList<NodeId>> ConstructTopology(
    MaelstromNode& maelstrom_node) {
  auto topology_msg = maelstrom_node.Receive();
  if (!topology_msg) {
    return {};
  }
  auto* topology = std::get_if<Topology>(&topology_msg->body);
  if (!topology) {
    return {};
  }
  auto result = topology->topology;
  topology_msg->body = TopologyOk{};
  maelstrom_node.Send(*topology_msg);
  return result;
}

template <typename T>
std::vector<T> ToVector(const std::unordered_set<T>& set) {
  return {set.begin(), set.end()};
}

int main() {
  MaelstromNode node;
  if (!node.Initialize()) {
    std::cerr << "Failed initializing the node.\n";
    return -1;
  }
  auto node_graph = ConstructTopology(node);
  if (!node_graph) {
    std::cerr << "Failed constructing topology.\n";
    return -1;
  }
  const auto neighbors = ToVector((*node_graph)[node.Id()]);
  std::mutex mu_numbers;
  std::unordered_set<int> numbers;
  std::jthread gossiper([&] {
    while (true) {
      std::this_thread::sleep_for(kGossipInterval);
      const auto random_neighbor = neighbors[Random(0, neighbors.size() - 1)];
      std::unordered_set<int> numbers_to_gossip;
      {
        std::unique_lock lck{mu_numbers};
        numbers_to_gossip = numbers;
      }
      Message msg = {.src = node.Id(),
                     .dest = random_neighbor,
                     .body = Gossip{.numbers = std::move(numbers_to_gossip)}};
      node.Send(msg);
    }
  });

  while (true) {
    auto msg = node.Receive();
    if (!msg) {
      std::cerr << "Failed parsing the received message.\n";
      continue;
    }
    if (std::get_if<Broadcast>(&msg->body)) {
      {
        std::unique_lock lck{mu_numbers};
        numbers.insert(std::get<Broadcast>(msg->body).number);
      }
      msg->body = BroadcastOk{};
      node.Send(std::move(*msg));
    } else if (std::get_if<Gossip>(&msg->body)) {
      const auto new_numbers = std::move(std::get<Gossip>(msg->body).numbers);
      msg->body = GossipOk{};
      node.Send(std::move(*msg));
      {
        std::unique_lock lck{mu_numbers};
        numbers.insert(new_numbers.begin(), new_numbers.end());
      }
    } else if (std::get_if<Read>(&msg->body)) {
      msg->body = ReadOk{.numbers = numbers};
      node.Send(std::move(*msg));
    }
  }
}
