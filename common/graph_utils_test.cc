#include "common/graph_utils.h"

#include <numeric>

#include "gtest/gtest.h"

TEST(MinimumSpanningTree, Empty) {
  std::set<Edge<int>> edges;
  const auto result = MinimumSpanningTree(edges);
  EXPECT_TRUE(result.empty());
}

TEST(MinimumSpanningTree, Full) {
  for (int num_vertices = 1; num_vertices <= 20; ++num_vertices) {
    std::set<Edge<int>> edges;
    for (int v = 0; v < num_vertices; ++v) {
      for (int u = 0; u < num_vertices; ++u) {
        if (v == u) {
          continue;
        }
        edges.insert({v, u});
      }
    }
    const auto result = MinimumSpanningTree(edges);
    const auto num_edges = std::accumulate(result.begin(), result.end(), 0,
                                           [](int count, const auto& node) {
                                             return count + node.second.size();
                                           }) /
                           2;
    EXPECT_EQ(num_edges, num_vertices - 1);
  }
}
