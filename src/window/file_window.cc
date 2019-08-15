#include "src/window/file_window.h"

#include "absl/strings/str_cat.h"

FileWindow::FileWindow(const std::string& filename) : filename_(filename) {
  buf() = Buffer::FromFile(filename);
  cursor_pos() = buf().begin();
}

FileWindow::~FileWindow() = default;

std::string FileWindow::Name() { return filename_; }

absl::optional<CommandStatus> FileWindow::NotifyCommand(
    absl::Span<std::string> command) {
  if (command[0] == "w") {
    return HandleWrite(command.subspan(1));
  }

  return absl::nullopt;
}

CommandStatus FileWindow::HandleWrite(absl::Span<std::string> args) {
  std::string write_path;

  if (args.size() > 1) {
    return CommandStatus::MakeError("Only one file name allowed");
  }

  // Write |filename_|.
  if (args.empty()) {
    if (filename_.empty()) {
      return CommandStatus::MakeError("No file name");
    }

    write_path = filename_;
  } else {
    ABSL_ASSERT(args.size() == 1);
    write_path = args[0];
  }

  bcf::Result<void> result = buf().WriteToFile(write_path);
  if (!result.ok()) {
    return CommandStatus::MakeError(result.err().msg());
  }

  return CommandStatus::MakeNormal(
      absl::StrCat("\"", write_path, "\" written"));
}
