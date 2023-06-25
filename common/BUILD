package(default_visibility = ["//visibility:public"])

cc_library(
    name = "json",
    hdrs = ["json.hpp"],
)

cc_library(
    name = "ids",
    hdrs = ["ids.h"],
    srcs = ["ids.cc"],
)

cc_library(
    name = "message",
    hdrs = ["message.h"],
    deps = [
        ":graph_utils",
        ":ids",
    ],
)

cc_library(
    name = "graph_utils",
    hdrs = ["graph_utils.h"],
)

cc_test(
    name = "graph_utils_test",
    srcs = ["graph_utils_test.cc"],
    deps = [
        ":graph_utils",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "thread_pool",
    hdrs = ["thread_pool.h"],
    srcs = ["thread_pool.cc"],
)

cc_test(
    name = "thread_pool_test",
    srcs = ["thread_pool_test.cc"],
    deps = [
        ":thread_pool",
        "@com_google_googletest//:gtest_main",
    ],
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
