#pragma once

template <typename T, unsigned char N>
struct ArrayList {
private:
  T _data[N];
  T* _top;

public:
  ArrayList() : _top{_data} {}

  bool push(const T& value) {
    if (_top >= _data + N) return false;  // переполнение
    *_top = value;
    _top++;
    return true;
  }

  T pop() {
    if (_top == _data) return T{};
    _top--;
    return *_top;
  }

  T& operator[](unsigned char index) { return _data[index]; }

  const T& operator[](unsigned char index) const { return _data[index]; }

  unsigned char size() const { return _top - _data; }

  bool empty() const { return _top == _data; }

  void clear() { _top = _data; }
};