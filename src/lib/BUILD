load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "pubsub",
    srcs = glob(["src/*.cc"]),
    hdrs = glob(["include/pubsub/*.hpp"]),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"]
)

cc_library(
    name = "pubsub_testing",
    hdrs = glob(["include/pubsub/testing/*.hpp"]),
    deps = [
        ":pubsub",
        "@com_google_googletest//:gtest_main",
    ],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    testing_only = True
)
