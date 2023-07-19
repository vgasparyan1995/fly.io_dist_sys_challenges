#include "handlers.h"

#include <iostream>

std::optional<Message> OnBroadcast(BroadcasterNodeState& state, Message msg) {
  auto& broadcast = std::get<Broadcast>(msg.body);
  const auto number = broadcast.number;
  msg.body = BroadcastOk{};
  {
    std::unique_lock lck(state.mu_numbers);
    auto [_, is_new_number] = state.numbers.insert(number);
    if (!is_new_number) {
      return msg;
    }
  }
  for (auto& [_, broadcaster] : state.broadcasters) {
    broadcaster.AddNumbers({broadcast.number});
  }
  return msg;
}

std::optional<Message> OnGossip(BroadcasterNodeState& state, Message msg) {
  auto& gossip = std::get<Gossip>(msg.body);
  std::vector<int> new_numbers = std::move(gossip.numbers);
  new_numbers.erase(std::remove_if(new_numbers.begin(), new_numbers.end(),
                                   [&state](int num) {
                                     std::unique_lock lck(state.mu_numbers);
                                     return state.numbers.contains(num);
                                   }),
                    new_numbers.end());
  {
    std::unique_lock lck(state.mu_numbers);
    state.numbers.insert(new_numbers.begin(), new_numbers.end());
  }
  for (auto& [node_id, broadcaster] : state.broadcasters) {
    if (node_id == msg.src) {
      continue;
    }
    broadcaster.AddNumbers(new_numbers);
  }
  msg.body = GossipOk{};
  return msg;
}

std::optional<Message> OnGossipOk(BroadcasterNodeState& state, Message msg) {
  auto it = state.broadcasters.find(msg.src);
  if (it == state.broadcasters.end()) {
    std::cerr << "Broadcaster for node " << msg.src.ToString() << " not found.";
    std::exit(-1);
  }
  if (!msg.in_reply_to) {
    std::cerr << "GossipOk must be in response to some message.";
    std::exit(-1);
  }
  it->second.GossipReceived(msg.in_reply_to.value());
  return msg;
}

std::optional<Message> OnRead(BroadcasterNodeState& state, Message msg) {
  std::unique_lock lck(state.mu_numbers);
  msg.body = ReadOk{
      .numbers = std::vector<int>(state.numbers.begin(), state.numbers.end())};
  return msg;
}
