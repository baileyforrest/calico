#pragma once

#include <cstddef>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

#define DISALLOW_MOVE_COPY_AND_ASSIGN(TypeName) \
  DISALLOW_COPY_AND_ASSIGN(TypeName);           \
  TypeName& operator=(TypeName&&) = delete;     \
  TypeName(TypeName&&) = delete

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))
