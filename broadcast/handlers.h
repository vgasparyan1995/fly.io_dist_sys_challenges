#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "broadcaster.h"
#include "common/ids.h"
#include "common/message.h"

struct BroadcasterNodeState {
  std::unordered_set<int> numbers;
  std::mutex mu_numbers;
  std::unordered_map<NodeId, Broadcaster> broadcasters;
};

std::optional<Message> OnBroadcast(BroadcasterNodeState& state, Message msg);
std::optional<Message> OnGossip(BroadcasterNodeState& state, Message msg);
std::optional<Message> OnGossipOk(BroadcasterNodeState& state, Message msg);
std::optional<Message> OnRead(BroadcasterNodeState& state, Message msg);
