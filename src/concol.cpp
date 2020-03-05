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

#include <cstring>

#include "concol.h"

using namespace concol;
using namespace detail;

bool color_base::_enabled{false};
std::FILE* color_base::_stream{stdout};

#if __cplusplus < 201703L
constexpr color_data color_constants::values[];
#endif

namespace concol {
namespace detail {

color_type to_bright(color_type _fg) noexcept {
  return color_type(int(_fg) + int(color_type::black_bright));
}

}  // namespace detail
}  // namespace concol

std::string color_base::ansi_color_code(color_type _fg, color_type _bg) {
  const int _color[]{0, 4, 2, 6, 1, 5, 3, 7};
  std::string str{"\x1b[0;"};
  if (_fg != color_type::none) {
    str += std::to_string(30 + _color[(int(_fg) & int(color_type::white))]);
    if (int(_fg) > int(color_type::white)) {
      str += ";1";
    }
  }
  if (_bg != color_type::none) {
    str +=
        ';' + std::to_string(40 + _color[(int(_bg) & int(color_type::white))]);
  }
  return std::string{str + 'm'};
}

#ifdef _WIN32

void color_base::windows_set_color(color_type _fg, color_type _bg) {
  auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (handle != nullptr) {
    CONSOLE_SCREEN_BUFFER_INFO info{};
    auto status = GetConsoleScreenBufferInfo(handle, &info);
    if (status) {
      WORD color = info.wAttributes;
      if (_fg != color_type::none) {
        color = (color & 0xFFF0) | int(_fg);
      }
      if (_bg != color_type::none) {
        color = (color & 0xFF0F) | int(_bg) << 4;
      }
      SetConsoleTextAttribute(handle, color);
    }
  }
}

void color_base::windows_printf(std::string&& str) {
  for (;;) {
    auto start_pos = str.find_first_of(_open_tag);
    auto stop_pos = str.find_first_of(_close_tag, start_pos + 1);
    if (start_pos == std::string::npos || stop_pos == std::string::npos) {
      std::fprintf(_stream, str.c_str());
      break;
    }
    if (start_pos != 0) {
      std::fprintf(_stream, str.substr(0, start_pos).c_str());
      str.erase(0, start_pos);
      continue;
    }
    if (stop_pos - start_pos == 1) {
      if (_enabled) {
        color_base::windows_set_color(color_type::white, color_type::black);
      }
      str.erase(0, stop_pos - start_pos + 1);
    } else {
      size_t start_substr{1};
      size_t size_substr{stop_pos - start_pos - 1};
      bool bright{};
      if (str[start_substr] == _bright_tag) {
        bright = true;
        start_substr += 1;
        size_substr -= 1;
      }
      bool isColorKey{};
      auto color_tag = str.substr(start_substr, size_substr);
      for (const auto& val : color_constants::values) {
        if (color_tag == val.color) {
          isColorKey = true;
          if (_enabled) {
            auto fg_key = (bright) ? to_bright(val.fg_key) : val.fg_key;
            color_base::windows_set_color(fg_key);
          }
          break;
        }
      }
      if (!isColorKey) {
        std::fprintf(_stream, str.substr(0, stop_pos - start_pos + 1).c_str());
      }
      str.erase(0, stop_pos - start_pos + 1);
    }
  }
}

#endif

std::string color_base::fmt_parse(const char* fmt) {
  std::string fmt_str{fmt};
  size_t start_pos{};
  size_t stop_pos{};
  for (;;) {
    bool bright{};
    start_pos = fmt_str.find_first_of(_open_tag, stop_pos);
    if (start_pos == std::string::npos) break;
    stop_pos = fmt_str.find_first_of(_close_tag, start_pos + 1);
    if (stop_pos == std::string::npos) break;
    std::string color_tag{};
    if (stop_pos - start_pos == 1) {
      fmt_str.erase(start_pos, stop_pos - start_pos + 1);
      stop_pos -= (stop_pos - start_pos);
      if (_enabled) {
        auto reset_str = color_base::ansi_color_reset();
        fmt_str.insert(start_pos, reset_str);
        stop_pos += std::strlen(reset_str);
      }
      continue;
    } else {
      size_t start_substr{start_pos + 1};
      size_t size_substr{stop_pos - start_pos - 1};
      if (fmt_str[start_substr] == _bright_tag) {
        bright = true;
        start_substr += 1;
        size_substr -= 1;
      }
      color_tag = fmt_str.substr(start_substr, size_substr);
    }
    for (const auto& val : color_constants::values) {
      if (color_tag == val.color) {
        fmt_str.erase(start_pos, stop_pos - start_pos + 1);
        stop_pos -= (stop_pos - start_pos);
        if (_enabled) {
          auto fg_key = (bright) ? to_bright(val.fg_key) : val.fg_key;
          auto color_ansi = color_base::ansi_color_code(fg_key);
          fmt_str.insert(start_pos, color_ansi);
          stop_pos += color_ansi.size();
        }
        break;
      }
    }
  };
  return fmt_str;
}

color::color(const char* c_str) : _string{c_str} {}

color::color(const std::string& str) : _string{str} {}

color::color(std::string&& str) : _string{std::move(str)} {}

#ifndef CONCOL_NO_STRING_VIEW
color::color(const std::string_view& str) : _string{str.data()} {}
#endif

color color::operator+(color_type rhs) {
  color tmp{*this};
  tmp._string += color_tags::values[int(rhs)];
  return tmp;
}

color color::operator+(color_ctrl rhs) {
  color tmp{*this};
  tmp._string += color_tags::values[int(rhs)];
  return tmp;
}

color color::operator+(const color& rhs) {
  color tmp{*this};
  tmp._string += rhs._string;
  return tmp;
}

color color::operator+(const std::string& rhs) {
  color tmp{*this};
  tmp._string += rhs;
  return tmp;
}

color color::operator+(const char* rhs) {
  color tmp{*this};
  tmp._string += std::string(rhs);
  return tmp;
}

color color::operator+(const char rhs) {
  color tmp{*this};
  tmp._string += rhs;
  return tmp;
}

color& color::operator+=(const color& rhs) {
  _string += rhs._string;
  return *this;
}

color& color::operator+=(const std::string& rhs) {
  _string += rhs;
  return *this;
}

color& color::operator+=(color_type rhs) {
  _string += color_tags::values[int(rhs)];
  return *this;
}

color& color::operator+=(color_ctrl rhs) {
  _string += color_tags::values[int(rhs)];
  return *this;
}

color& color::operator+=(const char* rhs) {
  _string += std::string(rhs);
  return *this;
}

color& color::operator+=(const char rhs) {
  _string += rhs;
  return *this;
}

void color::print() const { printf(_string.c_str()); }

void color::print_black() const {
  printf(color_tags::black + _string + color_tags::reset);
}

void color::print_blue() const {
  printf(color_tags::blue + _string + color_tags::reset);
}

void color::print_green() const {
  printf(color_tags::green + _string + color_tags::reset);
}

void color::print_cyan() const {
  printf(color_tags::cyan + _string + color_tags::reset);
}

void color::print_red() const {
  printf(color_tags::red + _string + color_tags::reset);
}

void color::print_magenta() const {
  printf(color_tags::magenta + _string + color_tags::reset);
}

void color::print_yellow() const {
  printf(color_tags::yellow + _string + color_tags::reset);
}

void color::print_white() const {
  printf(color_tags::white + _string + color_tags::reset);
}

void color::print_black_bright() const {
  printf(color_tags::black_bright + _string + color_tags::reset);
}

void color::print_blue_bright() const {
  printf(color_tags::blue_bright + _string + color_tags::reset);
}

void color::print_green_bright() const {
  printf(color_tags::green_bright + _string + color_tags::reset);
}

void color::print_cyan_bright() const {
  printf(color_tags::cyan_bright + _string + color_tags::reset);
}

void color::print_red_bright() const {
  printf(color_tags::red_bright + _string + color_tags::reset);
}

void color::print_magenta_bright() const {
  printf(color_tags::magenta_bright + _string + color_tags::reset);
}

void color::print_yellow_bright() const {
  printf(color_tags::yellow_bright + _string + color_tags::reset);
}

void color::print_white_bright() const {
  printf(color_tags::white_bright + _string + color_tags::reset);
}

void color::add(const std::string& str) { _string += str; }

void color::add(const char* c_str) { add(std::string(c_str)); }

void color::add(const char ch) { _string += ch; }

void color::add_black(const std::string& str) {
  _string += color_tags::black + str + color_tags::reset;
}

void color::add_black(const char* c_str) { add_black(std::string(c_str)); }

void color::add_black(const char ch) {
  _string += color_tags::black;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_blue(const std::string& str) {
  _string += color_tags::blue + str + color_tags::reset;
}

void color::add_blue(const char* c_str) { add_blue(std::string(c_str)); }

void color::add_blue(const char ch) {
  _string += color_tags::blue;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_green(const std::string& str) {
  _string += color_tags::green + str + color_tags::reset;
}

void color::add_green(const char* c_str) { add_green(std::string(c_str)); }

void color::add_green(const char ch) {
  _string += color_tags::green;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_cyan(const std::string& str) {
  _string += color_tags::cyan + str + color_tags::reset;
}

void color::add_cyan(const char* c_str) { add_cyan(std::string(c_str)); }

void color::add_cyan(const char ch) {
  _string += color_tags::cyan;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_red(const std::string& str) {
  _string += color_tags::red + str + color_tags::reset;
}

void color::add_red(const char* c_str) { add_red(std::string(c_str)); }

void color::add_red(const char ch) {
  _string += color_tags::red;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_magenta(const std::string& str) {
  _string += color_tags::magenta + str + color_tags::reset;
}

void color::add_magenta(const char* c_str) { add_magenta(std::string(c_str)); }

void color::add_magenta(const char ch) {
  _string += color_tags::magenta;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_yellow(const std::string& str) {
  _string += color_tags::yellow + str + color_tags::reset;
}

void color::add_yellow(const char* c_str) { add_yellow(std::string(c_str)); }

void color::add_yellow(const char ch) {
  _string += color_tags::yellow;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_white(const std::string& str) {
  _string += color_tags::white + str + color_tags::reset;
}

void color::add_white(const char* c_str) { add_white(std::string(c_str)); }

void color::add_white(const char ch) {
  _string += color_tags::white;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_black_bright(const std::string& str) {
  _string += color_tags::black_bright + str + color_tags::reset;
}

void color::add_black_bright(const char* c_str) {
  add_black_bright(std::string(c_str));
}

void color::add_black_bright(const char ch) {
  _string += color_tags::black_bright;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_blue_bright(const std::string& str) {
  _string += color_tags::blue_bright + str + color_tags::reset;
}

void color::add_blue_bright(const char* c_str) {
  add_blue_bright(std::string(c_str));
}

void color::add_blue_bright(const char ch) {
  _string += color_tags::blue_bright;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_green_bright(const std::string& str) {
  _string += color_tags::green_bright + str + color_tags::reset;
}

void color::add_green_bright(const char* c_str) {
  add_green_bright(std::string(c_str));
}

void color::add_green_bright(const char ch) {
  _string += color_tags::green_bright;
  _string += ch;
  _string += color_tags::reset;
}
void color::add_cyan_bright(const std::string& str) {
  _string += color_tags::cyan_bright + str + color_tags::reset;
}

void color::add_cyan_bright(const char* c_str) {
  add_cyan_bright(std::string(c_str));
}

void color::add_cyan_bright(const char ch) {
  _string += color_tags::cyan_bright;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_red_bright(const std::string& str) {
  _string += color_tags::red_bright + str + color_tags::reset;
}

void color::add_red_bright(const char* c_str) {
  add_red_bright(std::string(c_str));
}

void color::add_red_bright(const char ch) {
  _string += color_tags::red_bright;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_magenta_bright(const std::string& str) {
  _string += color_tags::magenta_bright + str + color_tags::reset;
}

void color::add_magenta_bright(const char* c_str) {
  add_magenta_bright(std::string(c_str));
}

void color::add_magenta_bright(const char ch) {
  _string += color_tags::magenta_bright;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_yellow_bright(const std::string& str) {
  _string += color_tags::yellow_bright + str + color_tags::reset;
}

void color::add_yellow_bright(const char* c_str) {
  add_yellow_bright(std::string(c_str));
}

void color::add_yellow_bright(const char ch) {
  _string += color_tags::yellow_bright;
  _string += ch;
  _string += color_tags::reset;
}

void color::add_white_bright(const std::string& str) {
  _string += color_tags::white_bright + str + color_tags::reset;
}

void color::add_white_bright(const char* c_str) {
  add_white_bright(std::string(c_str));
}

void color::add_white_bright(const char ch) {
  _string += color_tags::white_bright;
  _string += ch;
  _string += color_tags::reset;
}

namespace concol_literals {

std::string operator""_black(const char* c_str) {
  return {color_tags::black + std::string(c_str) + color_tags::reset};
}

std::string operator""_blue(const char* c_str) {
  return {color_tags::blue + std::string(c_str) + color_tags::reset};
}

std::string operator""_green(const char* c_str) {
  return {color_tags::green + std::string(c_str) + color_tags::reset};
}

std::string operator""_cyan(const char* c_str) {
  return {color_tags::cyan + std::string(c_str) + color_tags::reset};
}

std::string operator""_red(const char* c_str) {
  return {color_tags::red + std::string(c_str) + color_tags::reset};
}

std::string operator""_magenta(const char* c_str) {
  return {color_tags::magenta + std::string(c_str) + color_tags::reset};
}

std::string operator""_yellow(const char* c_str) {
  return {color_tags::yellow + std::string(c_str) + color_tags::reset};
}

std::string operator""_white(const char* c_str) {
  return {color_tags::white + std::string(c_str) + color_tags::reset};
}

std::string operator""_black_bright(const char* c_str) {
  return {color_tags::black_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_blue_bright(const char* c_str) {
  return {color_tags::blue_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_green_bright(const char* c_str) {
  return {color_tags::green_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_cyan_bright(const char* c_str) {
  return {color_tags::cyan_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_red_bright(const char* c_str) {
  return {color_tags::red_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_magenta_bright(const char* c_str) {
  return {color_tags::magenta_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_yellow_bright(const char* c_str) {
  return {color_tags::yellow_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_white_bright(const char* c_str) {
  return {color_tags::white_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_black(const char* c_str, std::size_t) {
  return {color_tags::black + std::string(c_str) + color_tags::reset};
}

std::string operator""_blue(const char* c_str, std::size_t) {
  return {color_tags::blue + std::string(c_str) + color_tags::reset};
}

std::string operator""_green(const char* c_str, std::size_t) {
  return {color_tags::green + std::string(c_str) + color_tags::reset};
}

std::string operator""_cyan(const char* c_str, std::size_t) {
  return {color_tags::cyan + std::string(c_str) + color_tags::reset};
}

std::string operator""_red(const char* c_str, std::size_t) {
  return {color_tags::red + std::string(c_str) + color_tags::reset};
}

std::string operator""_magenta(const char* c_str, std::size_t) {
  return {color_tags::magenta + std::string(c_str) + color_tags::reset};
}

std::string operator""_yellow(const char* c_str, std::size_t) {
  return {color_tags::yellow + std::string(c_str) + color_tags::reset};
}

std::string operator""_white(const char* c_str, std::size_t) {
  return {color_tags::white + std::string(c_str) + color_tags::reset};
}

std::string operator""_black_bright(const char* c_str, std::size_t) {
  return {color_tags::black_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_blue_bright(const char* c_str, std::size_t) {
  return {color_tags::blue_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_green_bright(const char* c_str, std::size_t) {
  return {color_tags::green_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_cyan_bright(const char* c_str, std::size_t) {
  return {color_tags::cyan_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_red_bright(const char* c_str, std::size_t) {
  return {color_tags::red_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_magenta_bright(const char* c_str, std::size_t) {
  return {color_tags::magenta_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_yellow_bright(const char* c_str, std::size_t) {
  return {color_tags::yellow_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_white_bright(const char* c_str, std::size_t) {
  return {color_tags::white_bright + std::string(c_str) + color_tags::reset};
}

std::string operator""_black(const char ch) {
  std::string str{color_tags::black};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_blue(const char ch) {
  std::string str{color_tags::blue};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_green(const char ch) {
  std::string str{color_tags::green};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_cyan(const char ch) {
  std::string str{color_tags::cyan};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_red(const char ch) {
  std::string str{color_tags::red};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_magenta(const char ch) {
  std::string str{color_tags::magenta};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_yellow(const char ch) {
  std::string str{color_tags::yellow};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_white(const char ch) {
  std::string str{color_tags::white};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_black_bright(const char ch) {
  std::string str{color_tags::black_bright};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_blue_bright(const char ch) {
  std::string str{color_tags::blue_bright};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_green_bright(const char ch) {
  std::string str{color_tags::green_bright};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_cyan_bright(const char ch) {
  std::string str{color_tags::cyan_bright};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_red_bright(const char ch) {
  std::string str{color_tags::red_bright};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_magenta_bright(const char ch) {
  std::string str{color_tags::magenta_bright};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_yellow_bright(const char ch) {
  std::string str{color_tags::yellow_bright};
  str += ch;
  str += color_tags::reset;
  return str;
}

std::string operator""_white_bright(const char ch) {
  std::string str{color_tags::white_bright};
  str += ch;
  str += color_tags::reset;
  return str;
}

}  // namespace concol_literals
