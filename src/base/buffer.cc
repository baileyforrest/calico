#include <algorithm>
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>

#include "base/buffer.h"

Buffer::iterator::iterator(Buffer* buf, Node* node, int16_t offset)
    : buf_(buf), node_(node), offset_(offset) {}

Buffer::iterator& Buffer::iterator::operator++() {
  assert(node_->gap_start >= 0);
  assert(node_->gap_end >= 0);
  assert(node_->gap_start <= kNodeSize);
  assert(node_->gap_end <= kNodeSize);
  assert(offset_ < kNodeSize);
  assert(offset_ < node_->gap_start || offset_ >= node_->gap_end);

  ++offset_;

  // Skip the gap if needed.
  if (offset_ == node_->gap_start) {
    offset_ = node_->gap_end;
  }

  // We're done if |offset_| is valid.
  if (offset_ < kNodeSize) {
    return *this;
  }

  // Need to move to next buffer.
  node_ = node_->next;
  offset_ = 0;

  // Reached end of data structure.
  if (node_ == nullptr) {
    return *this;
  }

  // Skip gap if needed.
  if (offset_ == node_->gap_start) {
    offset_ = node_->gap_end;
  }
  // It's an error to have an empty buffer.
  assert(offset_ < kNodeSize - 1);
  return *this;
}

Buffer::iterator& Buffer::iterator::operator--() {
  if (node_ == nullptr) {
    node_ = buf_->tail_;
    offset_ = node_->gap_end;
    if (offset_ == kNodeSize) {
      offset_ = node_->gap_start - 1;
    }

    return *this;
  }

  assert(node_->gap_start >= 0);
  assert(node_->gap_end >= 0);
  assert(node_->gap_start <= kNodeSize);
  assert(node_->gap_end <= kNodeSize);
  assert(offset_ < kNodeSize);
  assert(offset_ < node_->gap_start || offset_ >= node_->gap_end);
  --offset_;

  // Skip the gap if needed.
  if (offset_ == node_->gap_end - 1) {
    offset_ = node_->gap_start - 1;
  }

  // We're done if |offset_| is valid.
  if (offset_ >= 0) {
    return *this;
  }

  // Need to move to next buffer.
  node_ = node_->prev;
  offset_ = kNodeSize - 1;

  // Reached end of data structure.
  if (node_ == nullptr) {
    offset_ = 0;
    return *this;
  }

  // Skip gap if needed.
  if (offset_ == node_->gap_end - 1) {
    offset_ = node_->gap_start - 1;
  }
  // It's an error to have an empty buffer.
  assert(offset_ >= 0);
  return *this;
}

Buffer::iterator Buffer::iterator::LastLineStart(bool ignore_current_pos,
                                                 size_t* pdiff) const {
  size_t diff = 0;
  if (*this == buf_->begin()) {
    if (pdiff) {
      *pdiff = diff;
    }
    return *this;
  }
  iterator it = *this;
  if (ignore_current_pos) {
    --it;
    ++diff;
  }
  // Special case: Empty line
  if (*it == '\n') {
    if (pdiff) {
      *pdiff = diff;
    }
    return it;
  }

  // Find the next newline.
  while (*it != '\n') {
    if (it == buf_->begin()) {
      if (pdiff) {
        *pdiff = diff;
      }
      return it;
    }
    --it;
    ++diff;
  }
  ++it;
  --diff;

  if (pdiff) {
    *pdiff = diff;
  }
  return it;
}

Buffer::iterator Buffer::iterator::NextLineStart(size_t* pdiff) const {
  size_t diff = 0;
  if (*this == buf_->end()) {
    if (pdiff) {
      *pdiff = diff;
    }
    return *this;
  }
  iterator it = *this;
  while (it != buf_->end() && *it != '\n') {
    ++it;
    ++diff;
  }

  if (it != buf_->end()) {
    ++it;
    ++diff;
  }

  if (pdiff) {
    *pdiff = diff;
  }
  return it;
}

Buffer Buffer::FromFile(const std::string& path) {
  std::unique_ptr<FILE, decltype(&fclose)> file(fopen(path.c_str(), "r"),
                                                &fclose);
  Buffer buf;

  if (!file) {
    return buf;
  }

  Node* cur = nullptr;

  while (!feof(file.get())) {
    char raw[BUFSIZ];
    size_t num_read = fread(raw, 1, sizeof(buf), file.get());
    for (size_t i = 0; i < num_read; ++i) {
      // TODO(bcf): Handle unicode
      if (cur == nullptr || cur->gap_start == kNodeSize) {
        Node* new_node = new Node;
        new_node->prev = cur;
        new_node->next = nullptr;
        new_node->gap_start = 0;
        new_node->gap_end = kNodeSize;

        if (cur == nullptr) {
          buf.head_ = new_node;
          buf.tail_ = new_node;
        } else {
          cur->next = new_node;
          buf.tail_ = new_node;
        }

        cur = new_node;
      }

      cur->buf[cur->gap_start++] = raw[i];
    }
  }

  return buf;
}

Buffer::Buffer() = default;
Buffer::Buffer(Buffer&& other) {
  *this = std::move(other);
}

Buffer::~Buffer() {
  for (Node* node = head_; node != nullptr;) {
    Node* cur = node;
    node = cur->next;
    delete cur;
  }
}

void Buffer::operator=(Buffer&& other) {
  head_ = other.head_;
  tail_ = other.tail_;
  other.head_ = nullptr;
  other.tail_ = nullptr;
}

Buffer::iterator Buffer::begin() {
  int16_t offset = 0;
  if (head_ && head_->gap_start == 0) {
    offset = head_->gap_end;
    assert(offset < kNodeSize);
  }

  return iterator(this, head_, offset);
}

Buffer::iterator Buffer::insert(iterator iter, wchar_t value) {
  // Buffer is empty
  if (head_ == nullptr) {
    Node* node = new Node;
    node->prev = nullptr;
    node->next = nullptr;
    node->gap_start = 1;
    node->gap_end = kNodeSize;

    node->buf[0] = value;

    head_ = node;
    tail_ = node;

    return iterator(this, head_, 0);
  }

  // Inserting at end
  if (iter.node_ == nullptr) {
    assert(iter.offset_ == 0);
    iter.node_ = tail_;
    iter.offset_ = kNodeSize;
  }

  Node* node = iter.node_;
  int16_t offset = iter.offset_;

  assert(node->gap_start >= 0);
  assert(node->gap_end >= 0);
  assert(node->gap_start <= kNodeSize);
  assert(node->gap_end <= kNodeSize);
  assert(offset <= kNodeSize);
  assert(offset < node->gap_start || offset >= node->gap_end);

  // Current buffer is full and so is the last one.
  if (node->gap_start == kNodeSize &&
      (!node->prev || node->prev->gap_start == kNodeSize)) {
    assert(node->gap_end == kNodeSize);
    Node* new_node = new Node;
    // Special case: Adding to the beginning while full.
    if (offset == 0) {
      new_node->prev = node->prev;
      new_node->next = node;
      new_node->gap_start = 1;
      new_node->gap_end = kNodeSize;
      new_node->buf[0] = value;

      if (node->prev) {
        node->prev->next = new_node;
      } else {
        head_ = new_node;
      }
      node->prev = new_node;

      return iterator(this, new_node, 0);
    }

    // Special case: Adding to the end while full.
    if (offset == kNodeSize) {
      new_node->prev = node;
      new_node->next = node->next;
      new_node->gap_start = 1;
      new_node->gap_end = kNodeSize;
      new_node->buf[0] = value;
      if (node->next) {
        node->next->prev = new_node;
      } else {
        tail_ = new_node;
      }
      node->next = new_node;
      return iterator(this, new_node, 0);
    }

    new_node->prev = node;
    new_node->next = node->next;
    new_node->gap_start = 0;
    new_node->gap_end = offset;

    std::copy(node->buf + offset, node->buf + kNodeSize,
              new_node->buf + new_node->gap_end);
    if (node->next) {
      node->next->prev = new_node;
    } else {
      tail_ = new_node;
    }
    node->next = new_node;
    node->gap_start = offset;
    node->gap_end = kNodeSize;

    node->buf[node->gap_start] = value;
    ++node->gap_start;
    return iterator(this, node, offset);
  }

  // Current node is full, but last one is not.
  if (node->gap_start == kNodeSize) {
    assert(node->prev && node->prev->gap_start != kNodeSize);
    node = node->prev;
    offset = kNodeSize;
  }

  if (offset < node->gap_start) {  // Case 1: Inserting before the gap
    size_t to_move = node->gap_start - offset;
    std::copy_backward(node->buf + offset, node->buf + offset + to_move,
                       node->buf + node->gap_end);
    node->gap_start -= to_move;
    node->gap_end -= to_move;
    offset = node->gap_end;
  } else if (offset >= node->gap_end) {  // Case 2: Inserting after the gap
    size_t to_move = offset - node->gap_end;
    std::copy(node->buf + node->gap_end, node->buf + node->gap_end + to_move,
              node->buf + node->gap_start);
    node->gap_start += to_move;
    node->gap_end += to_move;
  } else {
    printf("Invalid buffer offset: %" PRId16 "\n", offset);
    abort();
  }

  assert(offset == node->gap_end);
  int16_t insert_idx = node->gap_start;
  node->buf[insert_idx] = value;
  ++node->gap_start;

  // Mark the buffer as full if needed.
  if (node->gap_start == node->gap_end) {
    node->gap_start = kNodeSize;
    node->gap_end = kNodeSize;
  }

  return iterator(this, node, insert_idx);
}

Buffer::iterator Buffer::erase(iterator iter) {
  Node* node = iter.node_;
  int16_t offset = iter.offset_;

  assert(node->gap_start >= 0);
  assert(node->gap_end >= 0);
  assert(node->gap_start <= kNodeSize);
  assert(node->gap_end <= kNodeSize);
  assert(offset < kNodeSize);
  assert(offset < node->gap_start || offset >= node->gap_end);

  Node* next_node = node;
  int16_t next_offset = offset;

  // Buffer is full.
  if (node->gap_start == kNodeSize) {
    node->gap_start = offset;
    node->gap_end = offset + 1;
    next_offset = node->gap_end;
  } else {
    // Move the gap to the location we're removing.
    if (offset < node->gap_start) {
      size_t to_move = node->gap_start - offset;
      std::copy_backward(node->buf + offset, node->buf + offset + to_move,
                         node->buf + node->gap_end);
      node->gap_start -= to_move;
      node->gap_end -= to_move;
      offset = node->gap_end;
    } else if (offset >= node->gap_end) {
      size_t to_move = node->gap_end - offset;
      std::copy(node->buf + node->gap_end, node->buf + node->gap_end + to_move,
                node->buf + node->gap_start);
      node->gap_start += to_move;
      node->gap_end += to_move;
    } else {
      printf("Invalid buffer offset: %" PRId16 "\n", offset);
      abort();
    }

    ++node->gap_end;
    next_offset = node->gap_end;
  }

  if (node->gap_end == kNodeSize) {
    next_node = node->next;
    next_offset = 0;
    if (next_node && next_node->gap_start == next_offset) {
      next_offset = next_node->gap_end;
      assert(next_offset <= kNodeSize);
    }

    // Buffer is now empty, remove it.
    if (node->gap_start == 0) {
      if (node->prev == nullptr) {
        head_ = node->next;
      } else {
        node->prev->next = node->next;
      }

      if (node->next == nullptr) {
        tail_ = node->prev;
      } else {
        node->next->prev = node->prev;
      }

      delete node;
    }
  }

  return iterator(this, next_node, next_offset);
}

void Buffer::DumpContents() {
  std::cout << "{";
  for (auto c : *this) {
    std::cout << static_cast<char>(c) << " ";
  }
  std::cout << "}\n";
}
