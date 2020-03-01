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

#include "concol.h"

using namespace concol;
using namespace concol_literals;
#ifndef CONCOL_NO_STRING_VIEW
using namespace std::string_view_literals;
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) try {
  std::cout << 255 << '\t';
  {
    ios_fmt_saver fmt_saver(std::cout);
    std::cout << std::hex;
    std::cout << 255 << '\t';
  }
  std::cout << 255 << '\n';

  color::set_ostream(stderr);
  color::printf("{blue}no color print to stderr\n{}");
  color::set_ostream(stdout);
  color::printf("{green}no color print to stdout\n{}");
  color::set_enabled(true);
  color::printf("{+cyan}color {+red}print{} to {+magenta} stderr\n{}");
  color::set_ostream(stdout);
  color::printf("{+yellow}color {+white}print{} to {+blue} stdout\n{}");

  color::printf(
      "{+blue}%d{}, {+green}%.2f{}, {+cyan}0x%X{}, {+red}%c{}, {+magenta}%s{}, "
      "{+yellow}%d{}\n",
      128, 3.14, 0xAA, 'S', "Hi", -128);

  color(128_blue + ", " + 3.14_green + ", " + 0xAA_cyan + ", " + "S"_red +
        ", " + "Hi"_magenta + ", " + "-128"_yellow + '\n')
      .print();

  color::printf(
      "{blue}%s{}, {green}%s{}, {cyan}%s{}, {red}%s{}, {magenta}%s{}, "
      "{yellow}%s{}, {white}%s{}\n",
      "blue", "green", "cyan", "red", "magenta", "yellow", "white");

  color::printf(
      "{+blue}%s{}, {+green}%s{}, {+cyan}%s{}, {+red}%s{}, {+magenta}%s{}, "
      "{+yellow}%s{}, {+white}%s{}\n",
      "blue", "green", "cyan", "red", "magenta", "yellow", "white");

  color("blue"_blue + ", " + "green"_green + ", " + "cyan"_cyan + ", " +
        "red"_red + ", " + "magenta"_magenta + ", " + "yellow"_yellow + ", " +
        "white\n"_white)
      .print();

  color("blue"_blue_bright + ", " + "green"_green_bright + ", " +
        "cyan"_cyan_bright + ", " + "red"_red_bright + ", " +
        "magenta"_magenta_bright + ", " + "yellow"_yellow_bright + ", " +
        "white\n"_white_bright)
      .print();

  color("blue").print_blue();
  color::printf(", ");
  color("green").print_green();
  color::printf(", ");
  color("cyan").print_cyan();
  color::printf(", ");
  color("red").print_red();
  color::printf(", ");
  color("magenta").print_magenta();
  color::printf(", ");
  color("yellow").print_yellow();
  color::printf(", ");
  color("white\n").print_white();

  color("blue").print_blue_bright();
  color::printf(", ");
  color("green").print_green_bright();
  color::printf(", ");
  color("cyan").print_cyan_bright();
  color::printf(", ");
  color("red").print_red_bright();
  color::printf(", ");
  color("magenta").print_magenta_bright();
  color::printf(", ");
  color("yellow").print_yellow_bright();
  color::printf(", ");
  color("white\n").print_white_bright();

  std::cout << color_type::blue << "blue" << color_ctrl::reset << ", ";
  std::cout << color_type::green << "green" << color_ctrl::reset << ", ";
  std::cout << color_type::cyan << "cyan" << color_ctrl::reset << ", ";
  std::cout << color_type::red << "red" << color_ctrl::reset << ", ";
  std::cout << color_type::magenta << "magenta" << color_ctrl::reset << ", ";
  std::cout << color_type::yellow << "yellow" << color_ctrl::reset << ", ";
  std::cout << color_type::white << "white\n" << color_ctrl::reset;

  std::cout << color_type::blue_bright << "blue" << color_ctrl::reset << ", ";
  std::cout << color_type::green_bright << "green" << color_ctrl::reset << ", ";
  std::cout << color_type::cyan_bright << "cyan" << color_ctrl::reset << ", ";
  std::cout << color_type::red_bright << "red" << color_ctrl::reset << ", ";
  std::cout << color_type::magenta_bright << "magenta" << color_ctrl::reset
            << ", ";
  std::cout << color_type::yellow_bright << "yellow" << color_ctrl::reset
            << ", ";
  std::cout << color_type::white_bright << "white\n" << color_ctrl::reset;

#ifndef _WIN32
  auto str =
      color::to_string("blue"_blue + ", " + "green"_green + ", " + "cyan"_cyan +
                       ", " + "red"_red + ", " + "magenta"_magenta + ", " +
                       "yellow"_yellow + ", " + "white\n"_white);
  std::cout << str;
  str = color::to_string(
      "{+blue}%s{}, {+green}%s{}, {+cyan}%s{}, {+red}%s{}, {+magenta}%s{}, "
      "{+yellow}%s{}, {+white}%s{}",
      "blue", "green", "cyan", "red", "magenta", "yellow", "white");
  std::puts(str.c_str());
#endif

  color::set_enabled(false);

  color("blue").print_blue();
  color::printf(", ");
  color("green").print_green();
  color::printf(", ");
  color("cyan").print_cyan();
  color::printf(", ");
  color("red").print_red();
  color::printf(", ");
  color("magenta").print_magenta();
  color::printf(", ");
  color("yellow").print_yellow();
  color::printf(", ");
  color("white\n").print_white();

  color("blue").print_blue_bright();
  color::printf(", ");
  color("green").print_green_bright();
  color::printf(", ");
  color("cyan").print_cyan_bright();
  color::printf(", ");
  color("red").print_red_bright();
  color::printf(", ");
  color("magenta").print_magenta_bright();
  color::printf(", ");
  color("yellow").print_yellow_bright();
  color::printf(", ");
  color("white\n").print_white_bright();

  return 0;
} catch (...) {
  std::cerr << "\nunexpected exception\n";
  return 0;
}
