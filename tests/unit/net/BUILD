load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "net/wire_format_unit_test",
    srcs = ["wire_format_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "net/reactor_unit_test",
    srcs = ["reactor_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ],
    linkopts = ["-lpthread"],
)

cc_test(
    name = "net/framed_connection_unit_test",
    srcs = ["framed_connection_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ],
    linkopts = ["-lpthread"],
)

cc_test(
    name = "net/remote_subscriber_connection_unit_test",
    srcs = ["remote_subscriber_connection_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ],
    linkopts = ["-lpthread"],
)

cc_test(
    name = "net/network_publisher_service_and_client_integration_test",
    srcs = ["network_publisher_service_and_client_integration_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ],
    linkopts = ["-lpthread"],
)
