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

cc_test(
    name = "subscriber_selection_strategy_unit_test",
    srcs = ["subscriber_selection_strategy_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_test(
    name = "subscriber_family_registration_manager_unit_test",
    srcs = ["subscriber_family_registration_manager_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_test(
    name = "subscriber_family_unit_test",
    srcs = ["subscriber_family_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_test(
    name = "hello_world_unit_test",
    srcs = ["hello_world_unit_test.cc"],
    deps = [
        "//src/hello_world:hello_world",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_test(
    name = "utils/task_queue_unit_test",
    srcs = ["utils/task_queue_unit_test.cc"],
    deps = [
        "//src/lib:pubsub",
        "//src/lib:pubsub_testing",
        "@com_google_googletest//:gtest_main",
    ],
    linkopts = ["-lpthread"],
)
