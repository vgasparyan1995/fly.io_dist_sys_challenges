#pragma once

#include <set>
#include <unordered_map>
#include <unordered_set>

template <typename T>
using AdjacencyList = std::unordered_map<T, std::unordered_set<T>>;

template <typename T>
using Edge = std::pair<T, T>;

template <typename T>
AdjacencyList<T> MinimumSpanningTree(std::set<Edge<T>>&& edges) {
  AdjacencyList<T> graph;
  std::unordered_map<T, T> parent;
  for (auto [v, u] : edges) {
    parent.insert({v, v});
    parent.insert({u, u});
    if (parent[v] == parent[u]) {
      continue;
    }
    graph[v].insert(u);
    graph[u].insert(v);
    parent[parent[v]] = parent[u];
  }
  return graph;
}
