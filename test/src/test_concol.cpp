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
  color::set_ostream(stderr);
  color::printf("{+cyan}color {+red}print{} to {+magenta} stderr\n{}");
  color::set_ostream(stdout);
  color::printf("{+yellow}color {+white}print{} to {+blue} stdout\n{}");

  color::printf(
      "{+blue}%d{}, {+green}%.2f{}, {+cyan}0x%X{}, {+red}%c{}, "
      "{+magenta}%s{}, "
      "{+yellow}%d{}\n",
      128, 3.14, 0xAA, 'S', "Hi", -128);

  color(128_blue + ", " + 3.14_green + ", " + 0xAA_cyan + ", " + 'S'_red +
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

  color::set_enabled(true);

  color color1{};
  color1 += "color";
  color1 = color1 + '1';
  color color2{"color"};
  color2 += '2';
  color color12 = color1;
  color12 += std::string(" + ") + color2 + '\n';
  color12.print();
  color12.print_blue();
  color12.print_green();
  color12.print_cyan();
  color12.print_red();
  color12.print_magenta();
  color12.print_yellow();
  color12.print_white();
  color12.print_blue_bright();
  color12.print_green_bright();
  color12.print_cyan_bright();
  color12.print_red_bright();
  color12.print_magenta_bright();
  color12.print_yellow_bright();
  color12.print_white_bright();

  color12.clear();
  color12.add_blue("blue");
  color12.add(", ");
  color12.add_green("green");
  color12.add(", ");
  color12.add_cyan("cyan");
  color12.add(", ");
  color12.add_red("red");
  color12.add(", ");
  color12.add_magenta("magenta");
  color12.add(", ");
  color12.add_yellow("yellow");
  color12.add(", ");
  color12.add_white("white\n");
  color12.print();

  color12.clear();
  color12.add_blue_bright("blue");
  color12.add(", ");
  color12.add_green_bright("green");
  color12.add(", ");
  color12.add_cyan_bright("cyan");
  color12.add(", ");
  color12.add_red_bright("red");
  color12.add(", ");
  color12.add_magenta_bright("magenta");
  color12.add(", ");
  color12.add_yellow_bright("yellow");
  color12.add(", ");
  color12.add_white_bright("white\n");
  color12.print();

  color12.clear();
  color12.add_blue(0);
  color12.add_green(1);
  color12.add_cyan(2);
  color12.add_red(3);
  color12.add_magenta(4);
  color12.add_yellow(5);
  color12.add_white(6);
  color12.add('\n');
  color12.print();

  color12.clear();
  color12.add_blue_bright(0);
  color12.add_green_bright(1);
  color12.add_cyan_bright(2);
  color12.add_red_bright(3);
  color12.add_magenta_bright(4);
  color12.add_yellow_bright(5);
  color12.add_white_bright(6);
  color12.add('\n');
  color12.print();

  color1.clear();
  color1 = color1 + color_type::blue + to_color(0);
  color1 = color1 + color_type::green + to_color(1);
  color1 = color1 + color_type::cyan + to_color(2);
  color1 = color1 + color_type::red + to_color(3);
  color1 = color1 + color_type::magenta + to_color(4);
  color1 = color1 + color_type::yellow + to_color(5);
  color1 = color1 + color_type::white + to_color(6);
  color1 = color1 + color_ctrl::reset;
  color1 = color1 + '\n';
  color1.print();

  color2.clear();
  color2 += color_type::blue_bright + to_color(0);
  color2 += color_type::green_bright + to_color(1);
  color2 += color_type::cyan_bright + to_color(2);
  color2 += color_type::red_bright + to_color(3);
  color2 += color_type::magenta_bright + to_color(4);
  color2 += color_type::yellow_bright + to_color(5);
  color2 += color_type::white_bright + to_color(6);
  color2 += color_ctrl::reset;
  color2 += '\n';
  color2.print();

  color1.clear();
  color1.add('A');
  color1.add_blue('B');
  color1.add_green('C');
  color1.add_cyan('D');
  color1.add_red('E');
  color1.add_magenta('F');
  color1.add_yellow('G');
  color1.add_white('H');
  color1.add('\n');
  color1.print();

  color2.clear();
  color2.add('A');
  color2.add_blue_bright('B');
  color2.add_green_bright('C');
  color2.add_cyan_bright('D');
  color2.add_red_bright('E');
  color2.add_magenta_bright('F');
  color2.add_yellow_bright('G');
  color2.add_white_bright('H');
  color2.add('\n');
  color2.print();

  color12.clear();
  color12 += 'I';
  color12 += color_type::blue;
  color12 += 'J';
  color12 += color_type::green;
  color12 += 'K';
  color12 += color_type::cyan;
  color12 += 'L';
  color12 += color_type::red;
  color12 += 'M';
  color12 += color_type::magenta;
  color12 += 'N';
  color12 += color_type::yellow;
  color12 += 'O';
  color12 += color_type::white;
  color12 += 'P';
  color12 += color_ctrl::reset;
  color12 += '\n';
  color12.print();

  color12.clear();
  color12 = color12 + 'I';
  color12 = color12 + color_type::blue_bright;
  color12 = color12 + 'J';
  color12 = color12 + color_type::green_bright;
  color12 = color12 + 'K';
  color12 = color12 + color_type::cyan_bright;
  color12 = color12 + 'L';
  color12 = color12 + color_type::red_bright;
  color12 = color12 + 'M';
  color12 = color12 + color_type::magenta_bright;
  color12 = color12 + 'N';
  color12 = color12 + color_type::yellow_bright;
  color12 = color12 + 'O';
  color12 = color12 + color_type::white_bright;
  color12 = color12 + 'P';
  color12 = color12 + color_ctrl::reset + '\n';
  color12.print();

  color1.clear();
  color1.add('Q');
  color1.add_blue_bright('R');
  color1.add_green_bright('S');
  color1.add_cyan_bright('T');
  color1.add_red_bright('U');
  color2.clear();
  color2.add_magenta_bright('V');
  color2.add_yellow_bright('W');
  color2.add_white_bright('X');
  color2.add('\n');
  color::printf(color1.c_str() + color2.to_string());

  return 0;
} catch (...) {
  std::cerr << "\nunexpected exception\n";
  return 0;
}
