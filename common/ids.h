#pragma once

#include <charconv>
#include <functional>
#include <string>
#include <string_view>

using MsgId = int;

enum class NodeType { kServer, kClient };

struct NodeId {
  NodeId() = default;
  explicit NodeId(std::string_view node_id);
  explicit NodeId(const std::string& node_id);

  std::string ToString() const;

  bool operator==(NodeId other) const { return id_ == other.id_; }
  bool operator<(NodeId other) const { return id_ < other.id_; }
  bool operator>(NodeId other) const { return id_ > other.id_; }
  bool operator<=(NodeId other) const { return id_ <= other.id_; }
  bool operator>=(NodeId other) const { return id_ >= other.id_; }

 private:
  NodeType type_ = NodeType::kServer;
  int id_ = -1;

  friend std::hash<NodeId>;
};

template <>
struct std::hash<NodeId> {
  size_t operator()(NodeId node_id) const {
    return std::hash<std::string>{}(node_id.ToString());
  }
};

std::vector<std::string> ToString(const std::vector<NodeId>& node_ids);
std::vector<NodeId> FromString(const std::vector<std::string>& node_ids);
