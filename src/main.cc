#include <unistd.h>

#include <cstdlib>
#include <functional>

#include "screen.h"  // NOLINT(build/include)
#include "window/file_window.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }
  Screen s;
  FileWindow window(argv[1]);
  window.NotifySize(s.rows(), s.cols());
  window.Render(s.render_cb());
  s.Refresh();

  return EXIT_SUCCESS;
}
