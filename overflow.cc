// overflow.cc

#include <iostream>
#include <stdexcept>
#include <string>

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

template <typename P>
struct OverflowPop {
  void OnOverflow() { static_cast<P*>(this)->Pop(); }
};

template <typename P>
struct OverflowException {
  void OnOverflow() {
    std::string msg = "no more space!";
    throw std::out_of_range(msg);
  }
};

template <typename T, template <typename> class OverflowPolicy = OverflowPop,
          typename DebugPolicy = NoDebug>
class CircularBuffer
    : private OverflowPolicy<CircularBuffer<T, OverflowPolicy>>,
      private DebugPolicy {
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
      OverflowPolicy<CircularBuffer>::OnOverflow();
      next = Next(end_);
    }
    buf_[end_] = std::move(item);
    end_ = next;
    DebugPolicy::OnPush(this);
  }

  T Pop() {
    if (begin_ == end_) {
      throw std::out_of_range("buffer underflow!");
    }
    T item = std::move(buf_[begin_]);
    begin_ = Next(begin_);
    DebugPolicy::OnPop(this);
    return item;
  }

 private:
  size_t Next(size_t cur) {
    size_t n = cur + 1;
    if (n >= cap_) n -= cap_;
    return n;
  }

  friend DebugPolicy;
  friend OverflowPolicy<CircularBuffer>;

  size_t begin_;
  size_t end_;
  size_t cap_;
  T* buf_;
};

int main() {
  int n = 3;
  // auto buf = CircularBuffer<int, OverflowException>(n);
  auto buf = CircularBuffer<int>(n);
  for (int i = 0; i < n + 2; ++i) {
    buf.Push(i);
  }
  int value;
  for (int i = 0; i < n; ++i) {
    value = buf.Pop();
    std::cout << "popped " << value << std::endl;
  }
  return 0;
}
