#pragma once

#include "json.hpp"
#include "message.h"

using Json = nlohmann::json;

std::optional<Body> InitFrom(const Json& j_init);
std::optional<Body> InitOkFrom(const Json& j_init_ok);
std::optional<Body> EchoFrom(const Json& j_echo);
std::optional<Body> EchoOkFrom(const Json& j_echo_ok);
std::optional<Body> GenerateFrom(const Json& j_generate);
std::optional<Body> GenerateOkFrom(const Json& j_generate_ok);
std::optional<Body> BroadcastFrom(const Json& j_broadcast);
std::optional<Body> BulkBroadcastFrom(const Json& j_bulk_broadcast);
std::optional<Body> BroadcastOkFrom(const Json& j_broadcast_ok);
std::optional<Body> ReadFrom(const Json& j_read);
std::optional<Body> ReadOkFrom(const Json& j_read_ok);
std::optional<Body> TopologyFrom(const Json& j_topology);
std::optional<Body> TopologyOkFrom(const Json& j_topology_ok);
std::optional<Message> MessageFrom(const Json& j_msg);

Json Serialize(const Init& init);
Json Serialize(const InitOk& init_ok);
Json Serialize(const Echo& echo);
Json Serialize(const EchoOk& echo_ok);
Json Serialize(const Generate& generate);
Json Serialize(const GenerateOk& generate_ok);
Json Serialize(const Broadcast& broadcast);
Json Serialize(const BulkBroadcast& bulk_broadcast);
Json Serialize(const BroadcastOk& broadcast_ok);
Json Serialize(const Read& read);
Json Serialize(const ReadOk& read_ok);
Json Serialize(const Topology& topology);
Json Serialize(const TopologyOk& topology_ok);
Json Serialize(const Body& body);
Json Serialize(const Message& msg);
