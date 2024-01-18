// naive.cc 

#include <iostream>
#include <stdexcept>

template <typename T>
class CircularBuffer {
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
  }

  void Pop(T& item) {
    if (begin_ == end_) {
      throw std::out_of_range("buffer underflow!");
    }
    item = std::move(buf_[begin_]);
    begin_ = Next(begin_);
  }

 private:
  size_t Next(size_t cur) {
    size_t n = cur + 1;
    if (n >= cap_) n -= cap_;
    return n;
  }

  size_t begin_;
  size_t end_;
  size_t cap_;
  T* buf_;
};

int main() {
  int n = 10;
  auto buf = CircularBuffer<int>(n);
  for (int i = 0; i < n; ++i) {
    std::cout << "pushing " << i << std::endl;
    buf.Push(i);
  }
  try {
    buf.Push(0);
  } catch (const std::out_of_range& e) {
    std::cout << e.what() << std::endl;
  };
  int value;
  for (int i = 0; i < n; ++i) {
    buf.Pop(value);
    std::cout << value << (i < n - 1 ? ", " : "");
  }
  std::cout << std::endl;
  try {
    buf.Pop(value);
  } catch (const std::out_of_range& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
