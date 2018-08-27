#pragma once

enum class Action {
  NONE,

  // Key based actions
  UP,
  DOWN,
  LEFT,
  RIGHT,
  HOME,
  END,
  ESCAPE,
  BACKSPACE,

  // Other actions
  START_COMMAND_MODE,
  START_INSERT_MODE,
};
