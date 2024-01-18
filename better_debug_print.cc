// debug_print.cc

#include <iostream>
#include <stdexcept>

struct Debug {
  template <typename P>
  void OnPush(P self) {
    auto [occ, cap] = GetOccCap(self);
    size_t pos = self->end_ == 0 ? self->cap_ - 1 : self->end_ - 1;
    std::cout << "pushed " << self->buf_[pos] << " [" << occ << '/' << cap
              << ']' << std::endl;
  }

  template <typename P>
  void OnPop(P self) {
    auto [occ, cap] = GetOccCap(self);
    size_t pos = self->begin_ == 0 ? self->cap_ - 1 : self->begin_ - 1;
    std::cout << "popped " << self->buf_[pos] << " [" << occ << '/' << cap
              << ']' << std::endl;
  }

 private:
  template <typename P>
  std::pair<int, int> GetOccCap(P self) {
    size_t cap = self->cap_ - 1;
    size_t occ;
    if (self->begin_ <= self->end_) {
      occ = self->end_ - self->begin_;
    } else {
      occ = self->cap_ - self->end_ + self->begin_;
    }
    return {occ, cap};
  }
};

struct NoDebug {
  template <typename P>
  void OnPush(P self) {}

  template <typename P>
  void OnPop(P self) {}
};

template <typename T, typename DebugPolicy = NoDebug>
class CircularBuffer : private DebugPolicy {
 public:
  CircularBuffer(size_t capacity)
      : begin_(0),
        end_(0),
        cap_(capacity + 1),
        buf_(reinterpret_cast<T*>(new char[sizeof(T) * (capacity + 1)])) {}

  ~CircularBuffer() {
    if (buf_) delete[] buf_;
  }

  // allow move
  CircularBuffer(CircularBuffer&& other) { *this = other; }
  CircularBuffer& operator=(CircularBuffer&& other) {
    begin_ = other.begin_;
    end_ = other.end_;
    cap_ = other.cap_;
    buf_ = other.cap_;
    other.begin_ = other.end_ = other.cap_ = 0;
    other.buf_ = nullptr;
    return *this;
  }
  // disable copy
  CircularBuffer(const CircularBuffer&& other) = delete;
  CircularBuffer& operator=(const CircularBuffer& other) = delete;

  void Push(T item) {
    size_t next = Next(end_);
    if (next == begin_) {
      throw std::out_of_range("buffer overflow!");
    }
    buf_[end_] = std::move(item);
    end_ = next;
    DebugPolicy::OnPush(this);
  }

  void Pop(T& item) {
    if (begin_ == end_) {
      throw std::out_of_range("buffer underflow!");
    }
    item = std::move(buf_[begin_]);
    begin_ = Next(begin_);
    DebugPolicy::OnPop(this);
  }

 private:
  size_t Next(size_t cur) {
    size_t n = cur + 1;
    if (n >= cap_) n -= cap_;
    return n;
  }

  friend DebugPolicy;

  size_t begin_;
  size_t end_;
  size_t cap_;
  T* buf_;
};

int main() {
  int n = 10;
  // auto buf = CircularBuffer<int>(n);
  auto buf = CircularBuffer<int, Debug>(n);
  for (int i = 0; i < n; ++i) {
    buf.Push(i);
  }
  std::cout << std::endl;
  int value;
  for (int i = 0; i < n; ++i) {
    buf.Pop(value);
  }
  return 0;
}
