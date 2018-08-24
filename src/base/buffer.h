#pragma once

#include <iterator>
#include <string>

#include "base/macros.h"

// TODO(bcf): Implement const iterator?
class Buffer {
 private:
  static constexpr int kNodeSize = 128;

  // When gap_start == gap_end == kNodeSize, the Node is full.
  struct Node {
    Node* prev;
    Node* next;
    int16_t gap_start;  // Inclusive: first index of the gap
    int16_t gap_end;    // Exclusive: first element outside the gap
    wchar_t buf[kNodeSize];
  };

 public:
  class iterator : public std::iterator<
                       std::bidirectional_iterator_tag,  // iterator_catageory
                       wchar_t,                          // value type
                       ssize_t,                          // difference_type
                       wchar_t*,                         // pointer
                       wchar_t                           // reference
                       > {
   public:
    iterator(Buffer* buf, Node* node, int16_t offset);
    ~iterator() = default;
    iterator& operator++();
    iterator& operator--();
    bool operator==(iterator other) const {
      return node_ == other.node_ && offset_ == other.offset_;
    }
    bool operator!=(iterator other) const { return !(*this == other); }
    reference operator*() const { return node_->buf[offset_]; }

   private:
    friend class Buffer;
    Buffer* buf_ = nullptr;
    Node* node_ = nullptr;
    int16_t offset_ = 0;
  };

  // Returns empty file if |path| doesn't exist.
  static Buffer FromFile(const std::string& path);

  Buffer();
  Buffer(Buffer&& other);
  ~Buffer();

  void operator=(Buffer&& other);

  iterator begin();
  iterator end() { return iterator(this, nullptr, 0); }
  bool empty() const { return head_ == nullptr; }
  iterator insert(iterator iter, wchar_t value);
  iterator erase(iterator iter);

  void DumpContents();

 private:
  Node* head_ = nullptr;
  Node* tail_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(Buffer);
};
