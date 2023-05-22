package(default_visibility = ["//visibility:public"])

cc_library(
    name = "json",
    hdrs = ["json.hpp"],
)

cc_library(
    name = "message",
    hdrs = ["message.h"],
)

cc_library(
    name = "json_utils",
    hdrs = ["json_utils.h"],
    srcs = ["json_utils.cc"],
    deps = [
        ":json",
        ":message",
    ],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "maelstrom_node",
    hdrs = ["maelstrom_node.h"],
    srcs = ["maelstrom_node.cc"],
    deps = [
        ":json_utils",
        ":message"
    ],
)
