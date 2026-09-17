load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "linked_list_unit_test",
    srcs = ["linked_list_unit_test.cc"],
    deps = [
        "//src/algorithms:linked_list",
        "@com_google_googletest//:gtest_main",
    ]
)
