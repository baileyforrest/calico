workspace(name = "calico")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

local_repository(
    name = "gtest",
    path = "third_party/googletest",
)

# Buildifier setup
http_archive(
    name = "io_bazel_rules_go",
    sha256 = "86ae934bd4c43b99893fc64be9d9fc684b81461581df7ea8fc291c816f5ee8c5",
    urls = ["https://github.com/bazelbuild/rules_go/releases/download/0.18.3/rules_go-0.18.3.tar.gz"],
)

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")

go_rules_dependencies()

go_register_toolchains()

# 0.22.0
http_archive(
    name = "com_github_bazelbuild_buildtools",
    sha256 = "34d920f4d91b8ca7fc9e2e2e43b90e9ae3ce364ee8b929b413c7d12d9ef0cfb5",
    strip_prefix = "buildtools-2172a1166a81953debaa767f332d2c8860e2e5f3",
    url = "https://github.com/bazelbuild/buildtools/archive/2172a1166a81953debaa767f332d2c8860e2e5f3.zip",
)

load("@com_github_bazelbuild_buildtools//buildifier:deps.bzl", "buildifier_dependencies")

buildifier_dependencies()
