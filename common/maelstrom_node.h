#pragma once

#include <array>
#include <functional>
#include <mutex>
#include <optional>
#include <string_view>
#include <utility>
#include <variant>

#include "message.h"
#include "thread_pool.h"
#include "type_utils.h"

// Thread-safe.
class MaelstromNode {
 public:
  using Handler = std::function<std::optional<Message>(Message)>;

  // The node waits for an "init" message to identify itself and the other
  // nodes. MaelstromNode should only be used after Initialize succeeds.
  bool Initialize();

  // Sends out a message. If `msg.dest` is the current node, `msg.src`,
  // `msg.dest` and `in_reply_to` fields will be set appropriately. Returns the
  // msg_id of the sent message.
  MsgId Send(Message msg);

  // Blocks on STDIN to read a single message.
  std::optional<Message> Receive();

  void StartReceiving();

  template <typename BodyT>
  void AddHandler(Handler&& handler) {
    handlers_[VariantIndex<BodyT>()] = std::move(handler);
  }

  NodeId Id() const { return id_; }

 private:
  MsgId DoSend(Message msg);

  NodeId id_;
  std::vector<NodeId> nodes_;
  std::mutex mu_send_;
  std::mutex mu_receive_;
  MsgId msg_id_ = 0;
  ThreadPool thread_pool_;
  std::mutex mu_handlers_;
  std::array<Handler, std::variant_size_v<Body>> handlers_;
};
