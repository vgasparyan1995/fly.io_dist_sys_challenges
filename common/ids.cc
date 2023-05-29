#include "ids.h"

NodeId::NodeId(const std::string& node_id) {
  std::from_chars(node_id.c_str() + 1, node_id.c_str() + node_id.size(), id_);
}

std::string NodeId::ToString() const { return "n" + std::to_string(id_); }

std::vector<std::string> ToString(const std::vector<NodeId>& node_ids) {
  std::vector<std::string> result(node_ids.size());
  std::transform(node_ids.begin(), node_ids.end(), result.begin(),
                 [](NodeId id) { return id.ToString(); });
  return result;
}

std::vector<NodeId> FromString(const std::vector<std::string>& node_ids) {
  std::vector<NodeId> result(node_ids.size());
  std::transform(node_ids.begin(), node_ids.end(), result.begin(),
                 [](const auto& id) { return NodeId(id); });
  return result;
}
