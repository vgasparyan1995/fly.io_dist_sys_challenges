#include <iostream>
#include <set>
#include <tuple>
#include <utility>
#include <variant>

#include "common/graph_utils.h"
#include "common/maelstrom_node.h"
#include "common/message.h"
#include "handlers.h"

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
  /*
  std::set<Edge<NodeId>> edges;
  for (const auto& [v, us] : topology->topology) {
    for (const auto& u : us) {
      if (v > u) {
        continue;
      }
      edges.insert({v, u});
    }
  }
  */
  auto result = topology->topology;
  topology_msg->body = TopologyOk{};
  maelstrom_node.Send(*topology_msg);
  // return MinimumSpanningTree(std::move(edges));
  // Testing a non-MST approach.
  return result;
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

  BroadcasterNodeState node_state;
  for (const auto& neighbor : (*node_graph)[node.Id()]) {
    node_state.broadcasters.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(neighbor),
                                    std::forward_as_tuple(node, neighbor));
  }

  node.AddHandler<Broadcast>(
      [&](Message msg) { return OnBroadcast(node_state, std::move(msg)); });
  node.AddHandler<Gossip>(
      [&](Message msg) { return OnGossip(node_state, std::move(msg)); });
  node.AddHandler<GossipOk>(
      [&](Message msg) { return OnGossipOk(node_state, std::move(msg)); });
  node.AddHandler<Read>(
      [&](Message msg) { return OnRead(node_state, std::move(msg)); });

  node.StartReceiving();
}
