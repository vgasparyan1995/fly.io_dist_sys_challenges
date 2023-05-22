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
std::optional<Message> MessageFrom(const Json& j_msg);

Json Serialize(const Init& init);
Json Serialize(const InitOk& init_ok);
Json Serialize(const Echo& echo);
Json Serialize(const EchoOk& echo_ok);
Json Serialize(const Generate& generate);
Json Serialize(const GenerateOk& generate_ok);
Json Serialize(const Body& body);
Json Serialize(const Message& msg);
