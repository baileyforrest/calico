#pragma once

#include <cwchar>
#include <map>

#include "src/action.h"

class KeyConfig {
 public:
  KeyConfig();
  ~KeyConfig();

  void AddBinding(wint_t key, Action action);
  Action GetAction(wint_t key);

 private:
  std::map<wint_t, Action> key_to_action_;
};
