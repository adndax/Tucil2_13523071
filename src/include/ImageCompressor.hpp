#ifndef IMAGE_COMPRESSOR_HPP
#define IMAGE_COMPRESSOR_HPP

#include <string>
#include <vector>
#include <functional>
#include "Colors.hpp"
#include "Quadtree.hpp"
#include "ErrorMeasurement.hpp"

class ImageCompressor {
public:

    ImageCompressor();
    void run();
    ~ImageCompressor() noexcept = default;  

private:
    double threshold;
    int min_block_size;
    std::function<double(const std::vector<std::vector<Color>>&, int, int, int, int)> errorFunc;

    Color getAverageColor(const std::vector<std::vector<Color>>& image_data, int x, int y, int width, int height);
    bool shouldDivide(double error, int width, int height);
    QuadtreeNode* build(const std::vector<std::vector<Color>>& image_data, int x, int y, int width, int heigh, int depth);
    Quadtree* compress(const std::vector<std::vector<Color>>& image_data,
        double target_compression,
        const std::string& tempPath,
        long originalSize,
        std::vector<std::vector<std::vector<Color>>>* gifFrames);
    void setErrorFunction(std::function<double(const std::vector<std::vector<Color>>&, int, int, int, int)> func);
};

#endif