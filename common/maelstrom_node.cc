#include "maelstrom_node.h"

#include <iostream>
#include <memory>
#include <mutex>
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

void WriteToStdOut(Json msg) { std::cout << msg.dump() << std::endl; }

}  // namespace

bool MaelstromNode::Initialize() {
  auto msg = Receive();
  if (!msg) {
    return false;
  }
  auto* init = std::get_if<Init>(&msg->body);
  if (!init) {
    return false;
  }
  id_ = init->node_id;
  nodes_ = std::move(init->node_ids);
  msg->body = InitOk{};
  Send(std::move(*msg));
  return true;
}

MsgId MaelstromNode::Send(Message msg) {
  if (msg.dest == id_) {
    std::swap(msg.src, msg.dest);
    msg.in_reply_to = msg.msg_id;
  }
  {
    std::unique_lock lck{mu_send_};
    msg.msg_id = msg_id_++;
    WriteToStdOut(Serialize(msg));
  }
  return msg.msg_id.value_or(-1);
}

std::optional<Message> MaelstromNode::Receive() {
  std::unique_lock lck{mu_receive_};
  if (auto j_msg = ReadFromStdIn(); j_msg) {
    auto msg = MessageFrom(*j_msg);
    if (!msg) {
      std::cerr << "Unsupported request: " << j_msg->dump() << '\n';
    }
    return msg;
  }
  return {};
}

void MaelstromNode::StartReceiving() {
  while (true) {
    auto msg = Receive();
    if (!msg) {
      continue;
    }
    thread_pool_.AddTask([this, message = std::move(*msg)]() {
      auto& handler = handlers_[message.body.index()];
      if (!handler) {
        std::cerr << "Message of type (idx): " << message.body.index()
                  << " does not have a handler.\n";
      }
      auto reply = handler(std::move(message));
      if (reply) {
        Send(*reply);
      }
    });
  }
}
