load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "sync_producer_observer",
    srcs = ["sync_producer_observer.cc"],
    deps = ["//src/lib:pubsub"],
)

cc_binary(
    name = "network_producer_subscriber",
    srcs = ["network_producer_subscriber.cc"],
    deps = ["//src/lib:pubsub"],
    linkopts = ["-lpthread"],
)
