#ifndef __ERRORMEASUREMENT_HPP__
#define __ERRORMEASUREMENT_HPP__

#include <vector>
#include "Colors.hpp"

class ErrorMeasurement {
public:
    static const int MAX_COLOR = 256;
    
    static double variance(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height);
    static double mad(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height);
    static double maxPixelDifference(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height);
    static double entropy(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height);
    static double ssim(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height);
};

#endif