#include "json_utils.h"

namespace {

template <typename T>
std::optional<T> operator||(std::optional<T> lhs, std::optional<T> rhs) {
  return lhs ? lhs : rhs;
}

} // namespace

std::optional<Body> InitFrom(const Json& j_init) {
  if (!j_init.contains("type") || j_init["type"] != "init" ||
      !j_init.contains("node_id") || !j_init.contains("node_ids")) {
    return {};
  }
  return {Init{.node_id = j_init["node_id"], .node_ids = j_init["node_ids"]}};
}

std::optional<Body> InitOkFrom(const Json& j_init_ok) {
  if (!j_init_ok.contains("type") || j_init_ok["type"] != "init_ok") {
    return {};
  }
  return InitOk{};
}

std::optional<Body> EchoFrom(const Json& j_echo) {
  if (!j_echo.contains("echo") || j_echo["type"] != "echo" ||
      !j_echo.contains("echo")) {
    return {};
  }
  return Echo{.echo = j_echo["echo"]};
}

std::optional<Body> EchoOkFrom(const Json& j_echo_ok) {
  if (!j_echo_ok.contains("type") || j_echo_ok["type"] != "echo_ok" ||
      !j_echo_ok.contains("echo")) {
    return {};
  }
  return EchoOk{.echo = j_echo_ok["echo"]};
}

std::optional<Body> GenerateFrom(const Json& j_generate) {
  if (!j_generate.contains("type") || j_generate["type"] != "generate") {
    return {};
  }
  return Generate{};
}

std::optional<Body> GenerateOkFrom(const Json& j_generate_ok) {
  if (!j_generate_ok.contains("type") ||
      j_generate_ok["type"] != "generate_ok" || !j_generate_ok.contains("id")) {
    return {};
  }
  return GenerateOk{.id = j_generate_ok["id"]};
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
  std::optional<Body> body = InitFrom(j_body) || InitOkFrom(j_body) ||
                             EchoFrom(j_body) || EchoOkFrom(j_body) ||
                             GenerateFrom(j_body) || GenerateOkFrom(j_body);
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
