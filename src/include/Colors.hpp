#ifndef __COLORS_HPP__
#define __COLORS_HPP__

struct Color {
    int r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(int red, int green, int blue) : r(red), g(green), b(blue) {}
};

#endif