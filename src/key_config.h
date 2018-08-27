#pragma once

#include <cwchar>
#include <map>

#include "action.h"  // NOLINT(build/include)

class KeyConfig {
 public:
  KeyConfig();
  ~KeyConfig();

  void AddBinding(wint_t key, Action action);
  Action GetAction(wint_t key);

 private:
  std::map<wint_t, Action> key_to_action_;
};
