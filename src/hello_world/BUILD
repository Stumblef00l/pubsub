load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "hello_world",
    srcs = glob(["src/**/*.cc"]),
    hdrs = glob(["include/hello_world/**/*.hpp"]),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"]
)
