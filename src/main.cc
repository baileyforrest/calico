#include <cstdlib>
#include <memory>

#include "absl/memory/memory.h"
#include "src/controller.h"
#include "src/window/file_window.h"

int main(int argc, char** argv) {
  Controller controller;

  std::string file_path;
  if (argc > 1) {
    file_path = argv[1];
  }

  auto window = absl::make_unique<FileWindow>(file_path);
  controller.AddWindow(std::move(window));
  controller.Run();

  return EXIT_SUCCESS;
}
