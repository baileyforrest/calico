package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "calico",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":core",
        "@absl//absl/memory",
    ],
)

cc_library(
    name = "action",
    hdrs = [
        "action.h",
    ],
)

cc_library(
    name = "core",
    srcs = [
        "controller.cc",
        "key_config.cc",
        "screen.cc",
    ],
    hdrs = [
        "action.h",
        "controller.h",
        "key.h",
        "key_config.h",
        "screen.h",
    ],
    copts = [
        "-I/usr/include/ncursesw",
    ],
    linkopts = [
        "-lncursesw",
        "-ltinfo",
    ],
    deps = [
        "//src/base",
        "//src/base:buffer",
        "//src/window",
    ],
)
