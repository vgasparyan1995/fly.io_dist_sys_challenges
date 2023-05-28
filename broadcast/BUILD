cc_binary(
    name = "broadcast",
    srcs = ["main.cc"],
    deps = [
        "//common:maelstrom_node",
        ":broadcaster",
    ],
)

cc_library(
    name = "broadcaster",
    srcs = ["broadcaster.cc"],
    hdrs = ["broadcaster.h"],
    deps = [
        "//common:maelstrom_node",
    ],
)
