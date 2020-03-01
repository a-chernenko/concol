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
using namespace concol::detail;

bool color_base::_enabled{false};
FILE* color_base::_stream{stdout};

#if __cplusplus < 201703L
constexpr color_data color_constants::values[];
#endif

std::string color_base::ansi_color_code(color_type _fg, color_type _bg) {
  const int _color[] = {0, 4, 2, 6, 1, 5, 3, 7};
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
