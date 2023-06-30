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

AdjacencyList<NodeId> ConstructTopology(MaelstromNode& maelstrom_node) {
  auto topology_msg = maelstrom_node.Receive();
  if (!topology_msg) {
    return {};
  }
  auto* topology = std::get_if<Topology>(&topology_msg->body);
  if (!topology) {
    return {};
  }
  std::set<Edge<NodeId>> edges;
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

  std::unordered_set<int> numbers;
  std::unordered_map<NodeId, Broadcaster> broadcasters;
  for (const auto& neighbor : node_graph[node.Id()]) {
    broadcasters.emplace(std::piecewise_construct,
                         std::forward_as_tuple(neighbor),
                         std::forward_as_tuple(node, neighbor));
  }

  node.AddHandler<Broadcast>([&numbers, &broadcasters](Message msg) {
    auto& broadcast = std::get<Broadcast>(msg.body);
    const auto number = broadcast.number;
    msg.body = BroadcastOk{};

    // TODO! 'numbers' must be locked
    auto [_, is_new_number] = numbers.insert(number);
    if (!is_new_number) {
      return msg;
    }
    for (auto& [_, broadcaster] : broadcasters) {
      broadcaster.AddNumbers({broadcast.number});
    }

    return msg;
  });

  node.StartReceiving();

  /*
    while (true) {
      auto msg = node.Receive();
      if (!msg) {
        continue;
      }
      if (auto* broadcast = std::get_if<Broadcast>(&msg->body)) {
        const auto number = broadcast->number;
        msg->body = BroadcastOk{};
        node.Send(*msg);

        auto [_, is_new_number] = numbers.insert(number);
        if (!is_new_number) {
          continue;
        }
        for (auto& [_, broadcaster] : broadcasters) {
          broadcaster.AddNumbers({broadcast->number});
        }
      } else if (auto* gossip = std::get_if<Gossip>(&msg->body)) {
        std::vector<int> new_numbers = std::move(gossip->numbers);
        msg->body = GossipOk{};
        node.Send(*msg);

        new_numbers.erase(
            std::remove_if(new_numbers.begin(), new_numbers.end(),
                           [&numbers](int num) { return numbers.contains(num);
    }), new_numbers.end()); numbers.insert(new_numbers.begin(),
    new_numbers.end()); for (auto& [node_id, broadcaster] : broadcasters) { if
    (node_id == msg->src) { continue;
          }
          broadcaster.AddNumbers(new_numbers);
        }
      } else if (std::get_if<GossipOk>(&msg->body)) {
        auto it = broadcasters.find(msg->src);
        if (it == broadcasters.end()) {
          std::cerr << "Broadcaster for node " << msg->src.ToString()
                    << " not found.";
          std::exit(-1);
        }
        if (!msg->in_reply_to) {
          std::cerr << "GossipOk must be in response to some message.";
          std::exit(-1);
        }
        it->second.GossipReceived(msg->in_reply_to.value());
      } else if (std::get_if<Read>(&msg->body)) {
        msg->body =
            ReadOk{.numbers = std::vector<int>(numbers.begin(), numbers.end())};
        node.Send(*msg);
      } else {
        std::cerr << "Unexpected message.\n";
      }
    }
    */
}
