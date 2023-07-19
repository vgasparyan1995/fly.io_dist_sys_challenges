cc_binary(
    name = "broadcast",
    srcs = ["main.cc"],
    deps = [
        "//common:maelstrom_node",
        ":broadcaster",
        ":handlers",
    ],
)

cc_library(
    name = "handlers",
    hdrs = ["handlers.h"],
    srcs = ["handlers.cc"],
    deps = [
        "//common:ids",
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
