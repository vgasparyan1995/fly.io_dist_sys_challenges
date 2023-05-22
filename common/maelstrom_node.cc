#include "maelstrom_node.h"

#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <variant>

#include "json_utils.h"

namespace {

std::optional<Json> ReadFromStdIn() {
  std::string raw_msg;
  if (!std::getline(std::cin, raw_msg)) {
    return {};
  }
  auto json_msg =
      Json::parse(raw_msg, /*cb=*/nullptr, /*allow_exceptions=*/false);
  if (json_msg.is_discarded()) {
    return {};
  }
  return json_msg;
}

void WriteToStdOut(Json msg) { std::cout << msg.dump() << '\n'; }

} // namespace

bool MaelstromNode::Initialize() {
  auto msg = Receive();
  if (!msg || !std::holds_alternative<Init>(msg->body)) {
    return false;
  }
  auto& init = std::get<Init>(msg->body);
  id_ = std::move(init.node_id);
  nodes_ = std::move(init.node_ids);
  msg->body = InitOk{};
  Send(std::move(*msg));
  return true;
}

std::optional<Message> MaelstromNode::Receive() {
  if (auto j_msg = ReadFromStdIn(); j_msg) {
    auto msg = MessageFrom(*j_msg);
    if (!msg) {
      std::cerr << "Unsupported request: " << j_msg->dump() << '\n';
    }
    return msg;
  }
  return {};
}

void MaelstromNode::Send(Message msg) {
  if (msg.dest == id_) {
    std::swap(msg.src, msg.dest);
    msg.in_reply_to = msg.msg_id;
    msg.msg_id = msg_id_++;
  }
  WriteToStdOut(Serialize(msg));
}

std::string_view MaelstromNode::Id() const { return id_; }
