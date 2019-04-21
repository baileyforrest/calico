#include <string>

#include "gtest/gtest.h"

#include "src/base/buffer.h"

namespace {

Buffer BufferFromString(const std::string& str) {
  Buffer b;
  for (auto c : str) {
    b.insert(b.end(), c);
  }

  return b;
}

bool EqualsString(const std::string& str, Buffer* buf) {
  auto it = buf->begin();
  for (auto c : str) {
    if (*it != c) {
      return false;
    }
    ++it;
  }

  return true;
}

void InsertOffset(Buffer* buf, size_t offset, wchar_t value) {
  auto it = buf->begin();
  std::advance(it, offset);
  auto ret = buf->insert(it, value);
  EXPECT_EQ(*ret, value);
}

void EraseOffset(Buffer* buf, size_t offset) {
  auto it = buf->begin();
  std::advance(it, offset);
  buf->erase(it);
}

}  // namespace

TEST(BufferTest, Empty) {
  Buffer buf;
  EXPECT_EQ(buf.begin(), buf.end());
}

TEST(BufferTest, InsertBack) {
  const std::string kChars = "abcdefghijklmnop";
  Buffer buf;

  for (auto c : kChars) {
    auto it = buf.insert(buf.end(), c);
    EXPECT_EQ(*it, c);
  }

  auto it = buf.begin();
  auto next_it = buf.begin();
  for (auto c : kChars) {
    EXPECT_EQ(next_it, it);
    EXPECT_EQ(*it, c);
    next_it = ++it;
  }

  EXPECT_EQ(it, buf.end());
}

TEST(BufferTest, BackIterator) {
  const std::string kChars = "abcdefghijklmnop";
  Buffer buf;

  for (auto c : kChars) {
    buf.insert(buf.end(), c);
  }

  auto it = buf.end();
  auto next_it = buf.end();
  for (auto char_it = kChars.rbegin(); char_it != kChars.rend(); ++char_it) {
    EXPECT_EQ(it, next_it);
    next_it = --it;
    EXPECT_EQ(*it, *char_it);
  }

  EXPECT_EQ(it, buf.begin());
}

TEST(BufferTest, InsertBackBig) {
  const int kNumIters = 4096;
  const std::string kChars = "abcdefghijklmnopqrstuvwxyz";

  Buffer buf;
  for (size_t i = 0; i < kNumIters; ++i) {
    for (auto c : kChars) {
      buf.insert(buf.end(), c);
    }
  }

  auto it = buf.begin();
  for (size_t i = 0; i < kNumIters; ++i) {
    for (auto c : kChars) {
      EXPECT_EQ(*it, c);
      ++it;
    }
  }
}

TEST(BufferTest, InsertFront) {
  const std::string kChars = "abcdefghijklmnop";
  Buffer buf;

  for (auto c : kChars) {
    buf.insert(buf.begin(), c);
  }

  auto it = buf.begin();
  for (auto char_it = kChars.rbegin(); char_it != kChars.rend(); ++char_it) {
    EXPECT_EQ(*it, *char_it);
    ++it;
  }

  EXPECT_EQ(it, buf.end());
}

TEST(BufferTest, InsertFrontBig) {
  const int kNumIters = 4096;
  const std::string kChars = "abcdefghijklmnopqrstuvwxyz";
  Buffer buf;

  for (size_t i = 0; i < kNumIters; ++i) {
    for (auto c : kChars) {
      buf.insert(buf.begin(), c);
    }
  }

  auto it = buf.begin();
  for (size_t i = 0; i < kNumIters; ++i) {
    for (auto char_it = kChars.rbegin(); char_it != kChars.rend(); ++char_it) {
      EXPECT_EQ(*it, *char_it);
      ++it;
    }
  }

  EXPECT_EQ(it, buf.end());
}

TEST(BufferTest, EraseBack) {
  const std::string kChars = "abcdefghijklmnop";
  Buffer buf;

  for (auto c : kChars) {
    buf.insert(buf.end(), c);
  }

  for (size_t i = 0; i < kChars.size(); ++i) {
    auto it = buf.begin();
    for (size_t j = 0; j < kChars.size() - i; ++j) {
      EXPECT_EQ(kChars[j], *it);
      ++it;
    }

    auto last = buf.end();
    --last;
    buf.erase(last);
  }
  EXPECT_EQ(buf.begin(), buf.end());
}

TEST(BufferTest, EraseBackBig) {
  const int kNumIters = 4096;
  const std::string kChars = "abcdefghijklmnopqrstuvwxyz";
  Buffer buf;

  for (size_t i = 0; i < kNumIters; ++i) {
    for (auto c : kChars) {
      buf.insert(buf.end(), c);
    }
  }

  for (size_t i = 0; i < kNumIters; ++i) {
    for (size_t i = 0; i < kChars.size(); ++i) {
      auto it = buf.begin();
      for (size_t j = 0; j < kChars.size() - i; ++j) {
        EXPECT_EQ(kChars[j], *it);
        ++it;
      }

      auto last = buf.end();
      --last;
      buf.erase(last);
    }
  }
  EXPECT_EQ(buf.begin(), buf.end());
}

TEST(BufferTest, InsertMiddle) {
  std::string str = "ab";
  Buffer buf = BufferFromString(str);

  str.insert(1, 1, 'c');
  InsertOffset(&buf, 1, 'c');
  EXPECT_TRUE(EqualsString(str, &buf));

  str.insert(2, 1, 'd');
  InsertOffset(&buf, 2, 'd');
  EXPECT_TRUE(EqualsString(str, &buf));

  str.insert(3, 1, 'e');
  InsertOffset(&buf, 3, 'e');
  EXPECT_TRUE(EqualsString(str, &buf));
}

TEST(BufferTest, InsertMiddleBig) {
  const int kNumIters = 2048;

  int char_idx = 0;
  auto get_char = [&char_idx] {
    return 'a' + (char_idx++ % 26);
  };  // NOLINT(whitespace/newline)

  std::string str = "a";
  Buffer buf = BufferFromString(str);

  uint32_t insert_idx = 0x530253;
  for (size_t i = 0; i < kNumIters; ++i) {
    insert_idx = (insert_idx * 31) << 2;

    size_t offset = insert_idx % str.size();

    char to_insert = get_char();

    str.insert(offset, 1, to_insert);
    InsertOffset(&buf, offset, to_insert);
    ASSERT_TRUE(EqualsString(str, &buf));
  }
}

TEST(BufferTest, EraseMiddleBig) {
  const int kNumIters = 2048;

  int char_idx = 0;
  auto get_char = [&char_idx] {
    return 'a' + (char_idx++ % 26);
  };  // NOLINT(whitespace/newline)

  std::string str;

  for (size_t i = 0; i < kNumIters; ++i) {
    char to_add = get_char();
    str.push_back(to_add);
  }

  Buffer buf = BufferFromString(str);

  uint32_t remove_idx = 0x530253;
  for (size_t i = 0; i < kNumIters; ++i) {
    remove_idx = (remove_idx * 31) << 2;
    size_t offset = remove_idx % str.size();

    str.erase(str.begin() + offset);
    EraseOffset(&buf, offset);
    ASSERT_TRUE(EqualsString(str, &buf));
  }
}
