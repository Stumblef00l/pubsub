load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "pubsub_test",
    srcs = glob(["*_test.cc"]),
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ]
)
