#ifndef IMAGE_IO_HPP
#define IMAGE_IO_HPP

#include <string>
#include <vector>
#include "Colors.hpp"
#include "Quadtree.hpp"

class ImageIO {
public:
    static bool loadImage(const std::string &path, std::vector<std::vector<Color>> &pixelData);
    static bool saveImage(const std::string &path, Quadtree *tree, bool drawOutline = false);
    static long getFileSize(const std::string &path);
};

#endif