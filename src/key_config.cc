#include "key_config.h"  // NOLINT(build/include)

#include <ncurses.h>

namespace {

// clang-format off
const struct {
  wint_t code;
  Action action;
} kKeyCodeToAction[] = {
  {KEY_UP, Action::UP},
  {KEY_DOWN, Action::DOWN},
  {KEY_LEFT, Action::LEFT},
  {KEY_RIGHT, Action::RIGHT},
  {KEY_HOME, Action::HOME},
  {KEY_END, Action::END},
  {KEY_BACKSPACE, Action::BACKSPACE},
};
// clang-format on

}  // namespace

KeyConfig::KeyConfig() {
  for (const auto& binding : kKeyCodeToAction) {
    AddBinding(binding.code, binding.action);
  }
}

KeyConfig::~KeyConfig() = default;

void KeyConfig::AddBinding(wint_t key, Action action) {
  key_to_action_[key] = action;
}

Action KeyConfig::GetAction(wint_t key) {
  auto it = key_to_action_.find(key);
  return it == key_to_action_.end() ? Action::NONE : it->second;
}
