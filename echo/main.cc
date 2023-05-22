#include <iostream>
#include <variant>

#include "common/maelstrom_node.h"

int main() {
  MaelstromNode maelstrom_node;
  if (!maelstrom_node.Initialize()) {
    std::cerr << "Failed to initialize.\n";
    return -1;
  }
  while (true) {
    auto msg = maelstrom_node.Receive();
    if (!msg) {
      std::cerr << "Failed parsing the received message.\n";
      continue;
    }
    if (std::holds_alternative<Echo>(msg->body)) {
      const auto& echo = std::get<Echo>(msg->body);
      msg->body = EchoOk{.echo = echo.echo};
      maelstrom_node.Send(*msg);
      continue;
    }
    std::cerr << "Unexpected request.\n";
  }
}
