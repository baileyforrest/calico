#include "src/base/string_util.h"

#include <iterator>
#include <sstream>

std::vector<std::string> StringSplit(const std::string& str) {
  std::istringstream iss(str);
  return std::vector<std::string>(std::istream_iterator<std::string>(iss),
                                  std::istream_iterator<std::string>());
}
