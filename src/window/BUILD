package(default_visibility = ["//visibility:public"])

cc_library(
    name = "window",
    srcs = [
        "buffer_window.cc",
        "command_window.cc",
        "file_window.cc",
    ],
    hdrs = [
        "buffer_window.h",
        "command_window.h",
        "file_window.h",
        "window.h",
    ],
    deps = [
        "//src:action",
        "//src:command-status",
        "//src/base",
        "//src/base:buffer",
        "@absl//absl/types:optional",
        "@absl//absl/types:span",
    ],
)
