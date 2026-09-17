load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "sync_producer_observer",
    srcs = ["sync_producer_observer.cc"],
    deps = ["//src/lib:pubsub"],
)

cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    deps = ["//src/hello_world:hello_world"],
)
