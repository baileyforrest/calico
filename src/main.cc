#include <err.h>
#include <cstdlib>
#include <iostream>

#include "base/buffer.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Missing file\n";
    return EXIT_FAILURE;
  }

  Buffer buf = Buffer::FromFile(argv[1]);
  if (buf.empty()) {
    std::cout << "Failed to read file\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
