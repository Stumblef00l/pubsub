load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "publisher_unit_test",
    srcs = ["publisher_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_test(
    name = "subscriber_family_manager_unit_test",
    srcs = ["subscriber_family_manager_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ]
)
