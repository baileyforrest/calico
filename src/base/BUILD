package(default_visibility = ["//visibility:public"])

cc_library(
    name = "base",
    srcs = [
        "string_util.cc",
        "task_runner.cc",
    ],
    hdrs = [
        "macros.h",
        "string_util.h",
        "task_runner.h",
    ],
)

cc_library(
    name = "buffer",
    srcs = [
        "buffer.cc",
        "buffer_test.cc",
    ],
    hdrs = [
        "buffer.h",
    ],
    deps = [
        ":base",
        "@gtest",
    ],
)

cc_test(
    name = "buffer_test",
    srcs = [
        "buffer_test.cc",
    ],
    deps = [
        ":buffer",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
