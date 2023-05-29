#pragma once

#include <charconv>
#include <functional>
#include <string>

using MsgId = int;

struct NodeId {
  NodeId() = default;
  explicit NodeId(const std::string& node_id);

  std::string ToString() const;

  bool operator==(NodeId other) const { return id_ == other.id_; }
  bool operator<(NodeId other) const { return id_ < other.id_; }
  bool operator>(NodeId other) const { return id_ > other.id_; }
  bool operator<=(NodeId other) const { return id_ <= other.id_; }
  bool operator>=(NodeId other) const { return id_ >= other.id_; }

 private:
  int id_ = -1;

  friend std::hash<NodeId>;
};

template <>
struct std::hash<NodeId> {
  size_t operator()(NodeId node_id) const {
    return std::hash<int>{}(node_id.id_);
  }
};

std::vector<std::string> ToString(const std::vector<NodeId>& node_ids);
std::vector<NodeId> FromString(const std::vector<std::string>& node_ids);
