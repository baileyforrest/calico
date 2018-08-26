#include <cstdlib>
#include <memory>

#include "controller.h"  // NOLINT(build/include)
#include "window/file_window.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }
  Controller controller;
  auto window = std::make_unique<FileWindow>(argv[1]);
  controller.AddWindow(std::move(window));
  controller.Run();

  return EXIT_SUCCESS;
}
