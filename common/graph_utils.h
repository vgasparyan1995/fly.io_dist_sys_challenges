#pragma once

#include <set>
#include <unordered_map>
#include <unordered_set>

template <typename T>
using AdjacencyList = std::unordered_map<T, std::unordered_set<T>>;

template <typename T>
using Edge = std::pair<T, T>;

template <typename T>
class DisjointSet {
 public:
  T Parent(T v) {
    if (!parents_.contains(v)) {
      parents_.insert({v, v});
      return v;
    }
    T parent = v;
    while (parent != parents_[parent]) {
      parent = parents_[parent];
    }
    parents_[v] = parent;
    return parent;
  }

  void Join(T v, T u) { parents_[Parent(v)] = Parent(u); }

 private:
  std::unordered_map<T, T> parents_;
};

template <typename T>
AdjacencyList<T> MinimumSpanningTree(const std::set<Edge<T>>& edges) {
  AdjacencyList<T> graph;
  DisjointSet<T> vertices;
  for (auto [v, u] : edges) {
    if (vertices.Parent(v) == vertices.Parent(u)) {
      continue;
    }
    graph[v].insert(u);
    graph[u].insert(v);
    vertices.Join(v, u);
  }
  return graph;
}
