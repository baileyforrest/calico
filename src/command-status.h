#pragma once

#include <string>

#include "absl/strings/string_view.h"

struct CommandStatus {
  static CommandStatus MakeNormal(absl::string_view message) {
    return CommandStatus{Type::kNormal, std::string(message)};
  }

  static CommandStatus MakeError(absl::string_view message) {
    return CommandStatus{Type::kError, std::string(message)};
  }

  enum class Type {
    kNormal,
    kError,
  };

  Type type;
  std::string message;
};
