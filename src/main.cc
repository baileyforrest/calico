#include <err.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "base/buffer.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Missing file\n";
    return EXIT_FAILURE;
  }

  std::ifstream ifs(argv[1]);
  if (!ifs) {
    err(EXIT_FAILURE, "Failed to open %s", argv[1]);
  }

  Buffer buf;
  while (!ifs.eof()) {
    buf.insert(buf.end(), ifs.get());
  }

  return EXIT_SUCCESS;
}
