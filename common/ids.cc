#include "ids.h"

#include <string>

NodeId::NodeId(std::string_view node_id) {
  type_ = node_id.starts_with('n') ? NodeType::kServer : NodeType::kClient;
  std::from_chars(node_id.data() + 1, node_id.data() + node_id.size(), id_);
}

NodeId::NodeId(const std::string& node_id)
    : NodeId(std::string_view(node_id)) {}

std::string NodeId::ToString() const {
  return (type_ == NodeType::kServer ? "n" : "c") + std::to_string(id_);
}

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
