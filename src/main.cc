#include <cstdlib>
#include <memory>

#include "absl/memory/memory.h"
#include "src/controller.h"
#include "src/window/file_window.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }
  Controller controller;
  auto window = absl::make_unique<FileWindow>(argv[1]);
  controller.AddWindow(std::move(window));
  controller.Run();

  return EXIT_SUCCESS;
}
