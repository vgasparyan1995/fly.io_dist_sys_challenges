#include <functional>
#include <iostream>
#include <variant>

#include "common/maelstrom_node.h"

int main() {
  int id = 0;
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
    if (std::holds_alternative<Generate>(msg->body)) {
      msg->body = GenerateOk{
          .id = std::hash<std::string_view>{}(maelstrom_node.Id()) ^ id++};
      maelstrom_node.Send(*msg);
      continue;
    }
    std::cerr << "Unexpected request.\n";
  }
}
