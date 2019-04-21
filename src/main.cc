#include <cstdlib>
#include <memory>

#include "controller.h"  // NOLINT(build/include)
#include "window/file_window.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }
  Controller controller;
  // TODO(bcf): absl::make_unique
  std::unique_ptr<FileWindow> window(new FileWindow(argv[1]));
  controller.AddWindow(std::move(window));
  controller.Run();

  return EXIT_SUCCESS;
}
