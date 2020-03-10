/*

MIT License

Copyright (c) 2020 Alexander Chernenko (achernenko@mail.ru)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#if __cplusplus < 201103L
#error \
    "This file requires compiler and library support for the ISO C++ 2011 standard or later."
#elif __cplusplus < 201703L
#define CONCOL_NO_STRING_VIEW
#endif

#include <iostream>
#include <string>
#ifndef CONCOL_NO_STRING_VIEW
#include <string_view>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

namespace concol {

class ios_fmt_saver final {
  std::ios& iosref;
  std::ios ioscopy{NULL};

 public:
  ios_fmt_saver(std::ios& _ios) : iosref{_ios} { ioscopy.copyfmt(_ios); }
  ~ios_fmt_saver() noexcept { iosref.copyfmt(ioscopy); }
  void save() { ioscopy.copyfmt(iosref); }
  void restore() { iosref.copyfmt(ioscopy); }
};

enum class color_type : int {
  none = -1,
  black = 0,
  blue,
  green,
  cyan,
  red,
  magenta,
  yellow,
  white,
  black_bright,
  blue_bright,
  green_bright,
  cyan_bright,
  red_bright,
  magenta_bright,
  yellow_bright,
  white_bright
};

enum class color_ctrl : int { reset = int(color_type::white_bright) + 1 };

namespace detail {

color_type to_bright(color_type _fg) noexcept;

struct color_data {
  color_type fg_key;
  const char* const color;
};

struct color_constants {
  static constexpr color_data black{color_type::black, "black"};
  static constexpr color_data red{color_type::red, "red"};
  static constexpr color_data green{color_type::green, "green"};
  static constexpr color_data yellow{color_type::yellow, "yellow"};
  static constexpr color_data blue{color_type::blue, "blue"};
  static constexpr color_data magenta{color_type::magenta, "magenta"};
  static constexpr color_data cyan{color_type::cyan, "cyan"};
  static constexpr color_data white{color_type::white, "white"};
  static constexpr color_data values[]{black, red,     green, yellow,
                                       blue,  magenta, cyan,  white};
  color_constants() = delete;
};

class color_base {
 protected:
  color_base() = default;
  static constexpr char _open_tag{'{'};
  static constexpr char _close_tag{'}'};
  static constexpr char _bright_tag{'+'};
  static bool _enabled;
  static std::FILE* _stream;

#ifdef _WIN32
  static void windows_printf(std::string&&);
  template <typename... Args>
  static std::string windows_to_string(const char* fmt, const Args&... args) {
    auto size = std::snprintf(nullptr, 0, fmt, args...);
    std::string str{};
    str.resize(size + 1);
#if __cplusplus < 201703L
    std::snprintf(const_cast<char*>(str.data()), str.size(), fmt, args...);
#else
    std::snprintf(str.data(), str.size(), fmt, args...);
#endif
    return str;
  }
#endif
  static std::string fmt_parse(const char*);

 public:
  static std::string ansi_color_code(color_type,
                                     color_type _bg = color_type::none);
  static const char* ansi_color_reset() { return "\x1b[0m"; }
#ifdef _WIN32
  static void windows_set_color(color_type, color_type _bg = color_type::none);
#endif
  static void set_ostream(FILE* stream = stdout) noexcept { _stream = stream; }
  static decltype(_stream) get_ostream() noexcept { return _stream; }
  static void set_enabled(bool enabled) noexcept { _enabled = enabled; }
  static bool is_enabled() noexcept { return _enabled; }
};

struct color_tags {
  static constexpr const char* const black{"{black}"};
  static constexpr const char* const blue{"{blue}"};
  static constexpr const char* const green{"{green}"};
  static constexpr const char* const cyan{"{cyan}"};
  static constexpr const char* const red{"{red}"};
  static constexpr const char* const magenta{"{magenta}"};
  static constexpr const char* const yellow{"{yellow}"};
  static constexpr const char* const white{"{white}"};
  static constexpr const char* const black_bright{"{+black}"};
  static constexpr const char* const blue_bright{"{+blue}"};
  static constexpr const char* const green_bright{"{+green}"};
  static constexpr const char* const cyan_bright{"{+cyan}"};
  static constexpr const char* const red_bright{"{+red}"};
  static constexpr const char* const magenta_bright{"{+magenta}"};
  static constexpr const char* const yellow_bright{"{+yellow}"};
  static constexpr const char* const white_bright{"{+white}"};
  static constexpr const char* const reset{"{}"};
  static constexpr const char* const values[]{
      black,        blue,        green,      cyan,           red,
      magenta,      yellow,      white,      black_bright,   blue_bright,
      green_bright, cyan_bright, red_bright, magenta_bright, yellow_bright,
      white_bright, reset};
  color_tags() = delete;
};

}  // namespace detail

class color final : public detail::color_base {
  std::string _string{};

 public:
  color() = default;
  color(const char*);
  color(const std::string&);
  color(std::string&&);
#ifndef CONCOL_NO_STRING_VIEW
  color(const std::string_view&);
#endif
  color(const color&) = default;
  color& operator=(const color&) = default;
  color operator+(const color&);
  color operator+(const std::string&);
  color operator+(color_type);
  color operator+(color_ctrl);
  color operator+(const char*);
  color operator+(const char);

  friend color operator+(color_type lhs, const color& rhs) {
    color tmp{rhs};
    tmp._string += detail::color_tags::values[int(lhs)];
    return tmp;
  }
  friend color operator+(color_ctrl lhs, const color& rhs) {
    color tmp{rhs};
    tmp._string += detail::color_tags::values[int(lhs)];
    return tmp;
  }
  friend color operator+(const std::string& lhs, const color& rhs) {
    color tmp{rhs};
    tmp._string = lhs + tmp._string;
    return tmp;
  }
  friend color operator+(const char* lhs, const color& rhs) {
    color tmp{rhs};
    tmp._string = std::string(lhs) + tmp._string;
    return tmp;
  }
  friend color operator+(const char lhs, const color& rhs) {
    color tmp{rhs};
    tmp._string = lhs + tmp._string;
    return tmp;
  }
  color& operator+=(const color&);
  color& operator+=(const std::string&);
  color& operator+=(color_type);
  color& operator+=(color_ctrl);
  color& operator+=(const char*);
  color& operator+=(const char);
  void clear() noexcept { _string.clear(); }
  std::string to_string() const noexcept { return _string; }
  const char* c_str() const noexcept { return _string.c_str(); }
  color& add(const std::string&);
  color& add(const char*);
  color& add(const char);
  color& add_black(const std::string&);
  color& add_black(const char*);
  color& add_black(const char);
  color& add_blue(const std::string&);
  color& add_blue(const char*);
  color& add_blue(const char);
  color& add_green(const std::string&);
  color& add_green(const char*);
  color& add_green(const char);
  color& add_cyan(const std::string&);
  color& add_cyan(const char*);
  color& add_cyan(const char);
  color& add_red(const std::string&);
  color& add_red(const char*);
  color& add_red(const char);
  color& add_magenta(const std::string&);
  color& add_magenta(const char*);
  color& add_magenta(const char);
  color& add_yellow(const std::string&);
  color& add_yellow(const char*);
  color& add_yellow(const char);
  color& add_white(const std::string&);
  color& add_white(const char*);
  color& add_white(const char);
  color& add_black_bright(const std::string&);
  color& add_black_bright(const char*);
  color& add_black_bright(const char);
  color& add_blue_bright(const std::string&);
  color& add_blue_bright(const char*);
  color& add_blue_bright(const char);
  color& add_green_bright(const std::string&);
  color& add_green_bright(const char*);
  color& add_green_bright(const char);
  color& add_cyan_bright(const std::string&);
  color& add_cyan_bright(const char*);
  color& add_cyan_bright(const char);
  color& add_red_bright(const std::string&);
  color& add_red_bright(const char*);
  color& add_red_bright(const char);
  color& add_magenta_bright(const std::string&);
  color& add_magenta_bright(const char*);
  color& add_magenta_bright(const char);
  color& add_yellow_bright(const std::string&);
  color& add_yellow_bright(const char*);
  color& add_yellow_bright(const char);
  color& add_white_bright(const std::string&);
  color& add_white_bright(const char*);
  color& add_white_bright(const char);
  template <typename Type>
  color& add(const Type& value) {
    return add(std::to_string(value));
  }
  template <typename Type>
  color& add_black(const Type& value) {
    return add_black(std::to_string(value));
  }
  template <typename Type>
  color& add_blue(const Type& value) {
    return add_blue(std::to_string(value));
  }
  template <typename Type>
  color& add_green(const Type& value) {
    return add_green(std::to_string(value));
  }
  template <typename Type>
  color& add_cyan(const Type& value) {
    return add_cyan(std::to_string(value));
  }
  template <typename Type>
  color& add_red(const Type& value) {
    return add_red(std::to_string(value));
  }
  template <typename Type>
  color& add_magenta(const Type& value) {
    return add_magenta(std::to_string(value));
  }
  template <typename Type>
  color& add_yellow(const Type& value) {
    return add_yellow(std::to_string(value));
  }
  template <typename Type>
  color& add_white(const Type& value) {
    return add_white(std::to_string(value));
  }
  template <typename Type>
  color& add_black_bright(const Type& value) {
    return add_black_bright(std::to_string(value));
  }
  template <typename Type>
  color& add_blue_bright(const Type& value) {
    return add_blue_bright(std::to_string(value));
  }
  template <typename Type>
  color& add_green_bright(const Type& value) {
    return add_green_bright(std::to_string(value));
  }
  template <typename Type>
  color& add_cyan_bright(const Type& value) {
    return add_cyan_bright(std::to_string(value));
  }
  template <typename Type>
  color& add_red_bright(const Type& value) {
    return add_red_bright(std::to_string(value));
  }
  template <typename Type>
  color& add_magenta_bright(const Type& value) {
    return add_magenta_bright(std::to_string(value));
  }
  template <typename Type>
  color& add_yellow_bright(const Type& value) {
    return add_yellow_bright(std::to_string(value));
  }
  template <typename Type>
  color& add_white_bright(const Type& value) {
    return add_white_bright(std::to_string(value));
  }
  void print() const;
  void print_black() const;
  void print_blue() const;
  void print_green() const;
  void print_cyan() const;
  void print_red() const;
  void print_magenta() const;
  void print_yellow() const;
  void print_white() const;
  void print_black_bright() const;
  void print_blue_bright() const;
  void print_green_bright() const;
  void print_cyan_bright() const;
  void print_red_bright() const;
  void print_magenta_bright() const;
  void print_yellow_bright() const;
  void print_white_bright() const;
  template <typename... Args>
  static void printf(const char* fmt, const Args&... args) {
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    auto fmt_str = fmt_parse(fmt);
    std::fprintf(_stream, fmt_str.c_str(), args...);
#pragma GCC diagnostic pop
#else
    auto str = windows_to_string(fmt, args...);
    windows_printf(std::move(str));
#endif
  }
  static void printf(const std::string& str) { printf(str.c_str()); }
#ifndef CONCOL_NO_STRING_VIEW
  static void printf(const std::string_view& str) { printf(str.data()); }
#endif
  template <typename... Args>
  static std::string to_string(const char* fmt, const Args&... args) {
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    auto size = std::snprintf(nullptr, 0, fmt, args...);
    std::string str{};
    str.resize(size + 1);
#if __cplusplus < 201703L
    std::snprintf(const_cast<char*>(str.data()), str.size(), fmt, args...);
#else
    std::snprintf(str.data(), str.size(), fmt, args...);
#endif
    return fmt_parse(str.c_str());
#pragma GCC diagnostic pop
#else
    return windows_to_string(fmt, args...);
#endif
  }
  template <typename... Args>
  static std::string to_string(std::string&& fmt_str, const Args&... args) {
    return to_string(fmt_str.c_str(), args...);
  }
#ifndef CONCOL_NO_STRING_VIEW
  template <typename... Args>
  static std::string to_string(const std::string_view& fmt_str,
                               const Args&... args) {
    return to_string(fmt_str.data(), args...);
  }
#endif
};

template <typename Type>
color to_color(Type value) {
  return color(std::to_string(value));
}

}  // namespace concol

template <typename charT, typename traits>
std::basic_ostream<charT, traits>& operator<<(
    std::basic_ostream<charT, traits>& lhs, concol::color_type rhs) {
  if (concol::color::is_enabled()) {
#ifdef _WIN32
    concol::color::windows_set_color(rhs);
#else
    lhs << concol::color::ansi_color_code(rhs);
#endif
  }
  return lhs;
}

template <typename charT, typename traits>
std::basic_ostream<charT, traits>& operator<<(
    std::basic_ostream<charT, traits>& lhs,
    [[maybe_unused]] concol::color_ctrl rhs) {
  if (concol::color::is_enabled()) {
#ifdef _WIN32
    concol::color::windows_set_color(concol::color_type::white,
                                     concol::color_type::black);
#else
    lhs << concol::color::ansi_color_reset();
#endif
  }
  return lhs;
}

namespace concol_literals {

std::string operator""_black(const char*);
std::string operator""_blue(const char*);
std::string operator""_green(const char*);
std::string operator""_cyan(const char*);
std::string operator""_red(const char*);
std::string operator""_magenta(const char*);
std::string operator""_yellow(const char*);
std::string operator""_white(const char*);
std::string operator""_black_bright(const char*);
std::string operator""_blue_bright(const char*);
std::string operator""_green_bright(const char*);
std::string operator""_cyan_bright(const char*);
std::string operator""_red_bright(const char*);
std::string operator""_magenta_bright(const char*);
std::string operator""_yellow_bright(const char*);
std::string operator""_white_bright(const char*);
std::string operator""_black(const char*, std::size_t);
std::string operator""_blue(const char*, std::size_t);
std::string operator""_green(const char*, std::size_t);
std::string operator""_cyan(const char*, std::size_t);
std::string operator""_red(const char*, std::size_t);
std::string operator""_magenta(const char*, std::size_t);
std::string operator""_yellow(const char*, std::size_t);
std::string operator""_white(const char*, std::size_t);
std::string operator""_black_bright(const char*, std::size_t);
std::string operator""_blue_bright(const char*, std::size_t);
std::string operator""_green_bright(const char*, std::size_t);
std::string operator""_cyan_bright(const char*, std::size_t);
std::string operator""_red_bright(const char*, std::size_t);
std::string operator""_magenta_bright(const char*, std::size_t);
std::string operator""_yellow_bright(const char*, std::size_t);
std::string operator""_white_bright(const char*, std::size_t);
std::string operator""_black(const char);
std::string operator""_blue(const char);
std::string operator""_green(const char);
std::string operator""_cyan(const char);
std::string operator""_red(const char);
std::string operator""_magenta(const char);
std::string operator""_yellow(const char);
std::string operator""_white(const char);
std::string operator""_black_bright(const char);
std::string operator""_blue_bright(const char);
std::string operator""_green_bright(const char);
std::string operator""_cyan_bright(const char);
std::string operator""_red_bright(const char);
std::string operator""_magenta_bright(const char);
std::string operator""_yellow_bright(const char);
std::string operator""_white_bright(const char);

}  // namespace concol_literals
