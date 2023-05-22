#include "json_utils.h"
#include "common/message.h"

namespace {

template <typename T>
std::optional<T> operator||(std::optional<T> lhs, std::optional<T> rhs) {
  return lhs ? std::move(lhs) : std::move(rhs);
}

} // namespace

std::optional<Body> InitFrom(const Json& j_init) {
  if (j_init["type"] != "init" || !j_init.contains("node_id") ||
      !j_init.contains("node_ids")) {
    return {};
  }
  return {Init{.node_id = j_init["node_id"], .node_ids = j_init["node_ids"]}};
}

std::optional<Body> InitOkFrom(const Json& j_init_ok) {
  if (j_init_ok["type"] != "init_ok") {
    return {};
  }
  return InitOk{};
}

std::optional<Body> EchoFrom(const Json& j_echo) {
  if (j_echo["type"] != "echo" || !j_echo.contains("echo")) {
    return {};
  }
  return Echo{.echo = j_echo["echo"]};
}

std::optional<Body> EchoOkFrom(const Json& j_echo_ok) {
  if (j_echo_ok["type"] != "echo_ok" || !j_echo_ok.contains("echo")) {
    return {};
  }
  return EchoOk{.echo = j_echo_ok["echo"]};
}

std::optional<Body> GenerateFrom(const Json& j_generate) {
  if (j_generate["type"] != "generate") {
    return {};
  }
  return Generate{};
}

std::optional<Body> GenerateOkFrom(const Json& j_generate_ok) {
  if (j_generate_ok["type"] != "generate_ok" || !j_generate_ok.contains("id")) {
    return {};
  }
  return GenerateOk{.id = j_generate_ok["id"]};
}

std::optional<Body> BroadcastFrom(const Json& j_broadcast) {
  if (j_broadcast["type"] != "broadcast" || !j_broadcast.contains("message")) {
    return {};
  }
  return Broadcast{.message = j_broadcast["message"]};
}

std::optional<Body> BroadcastOkFrom(const Json& j_broadcast_Ok) {
  if (j_broadcast_Ok["type"] != "broadcast_ok") {
    return {};
  }
  return BroadcastOk{};
}

std::optional<Body> ReadFrom(const Json& j_read) {
  if (j_read["type"] != "read") {
    return {};
  }
  return Read{};
}

std::optional<Body> ReadOkFrom(const Json& j_read_ok) {
  if (j_read_ok["type"] != "read_ok" || !j_read_ok.contains("messages")) {
    return {};
  }
  return ReadOk{.messages = j_read_ok["messages"]};
}

std::optional<Body> TopologyFrom(const Json& j_topology) {
  if (j_topology["type"] != "topology" || !j_topology.contains("topology")) {
    return {};
  }
  return Topology{.topology = j_topology["topology"]};
}

std::optional<Body> TopologyOkFrom(const Json& j_topology_ok) {
  if (j_topology_ok["type"] != "topology_ok") {
    return {};
  }
  return TopologyOk{};
}

std::optional<Message> MessageFrom(const Json& j_msg) {
  if (!j_msg.contains("src") || !j_msg.contains("dest") ||
      !j_msg.contains("body")) {
    return {};
  }
  Message msg;
  j_msg["src"].get_to(msg.src);
  j_msg["dest"].get_to(msg.dest);
  const auto& j_body = j_msg["body"];
  if (j_body.contains("msg_id")) {
    msg.msg_id = j_body["msg_id"].get<int>();
  }
  if (j_body.contains("in_reply_to")) {
    msg.in_reply_to = j_body["in_reply_to"].get<int>();
  }
  if (!j_body.contains("type")) {
    return {};
  }
  std::optional<Body> body =
      InitFrom(j_body) || InitOkFrom(j_body) || EchoFrom(j_body) ||
      EchoOkFrom(j_body) || GenerateFrom(j_body) || GenerateOkFrom(j_body) ||
      BroadcastFrom(j_body) || BroadcastOkFrom(j_body) || ReadFrom(j_body) ||
      ReadOkFrom(j_body) || TopologyFrom(j_body) || TopologyOkFrom(j_body);
  if (!body) {
    return {};
  }
  msg.body = std::move(*body);
  return msg;
}

Json Serialize(const Init& init) {
  return {
      {"type", "init"}, {"node_id", init.node_id}, {"node_ids", init.node_ids}};
}

Json Serialize(const InitOk& init_ok) { return {{"type", "init_ok"}}; }

Json Serialize(const Echo& echo) {
  return {{"type", "echo"}, {"echo", echo.echo}};
}

Json Serialize(const EchoOk& echo_ok) {
  return {{"type", "echo_ok"}, {"echo", echo_ok.echo}};
}

Json Serialize(const Generate& generate) { return {{"type", "generate"}}; }

Json Serialize(const GenerateOk& generate_ok) {
  return {{"type", "generate_ok"}, {"id", generate_ok.id}};
}

Json Serialize(const Broadcast& broadcast) {
  return {{"type", "broadcast"}, {"message", broadcast.message}};
}

Json Serialize(const BroadcastOk& broadcast_ok) {
  return {{"type", "broadcast_ok"}};
}

Json Serialize(const Read& read) { return {{"type", "read"}}; }

Json Serialize(const ReadOk& read_ok) {
  return {{"type", "read_ok"}, {"messages", read_ok.messages}};
}

Json Serialize(const Topology& topology) {
  return {{"type", "topology"}, {"topology", topology.topology}};
}

Json Serialize(const TopologyOk& topology_ok) {
  return {{"type", "topology_ok"}};
}

Json Serialize(const Body& body) {
  return std::visit([](const auto& body) { return Serialize(body); }, body);
}

Json Serialize(const Message& msg) {
  Json j_msg = {
      {"src", msg.src}, {"dest", msg.dest}, {"body", Serialize(msg.body)}};
  if (msg.msg_id) {
    j_msg["body"]["msg_id"] = *msg.msg_id;
  }
  if (msg.in_reply_to) {
    j_msg["body"]["in_reply_to"] = *msg.in_reply_to;
  }
  return j_msg;
}
