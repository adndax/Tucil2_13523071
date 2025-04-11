#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "ImageIO.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

bool ImageIO::loadImage(const std::string &path, std::vector<std::vector<Color>> &pixelData) {
    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 3); // Paksa jadi RGB (tanpa alpha)
    if (!data) {
        cerr << "Failed to load image: " << path << endl;
        return false;
    }

    pixelData.resize(height, std::vector<Color>(width));
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            pixelData[y][x] = Color(data[idx], data[idx + 1], data[idx + 2]);
        }

    stbi_image_free(data);
    return true;
}

bool ImageIO::saveImage(const std::string &path, Quadtree *tree, bool drawOutline) {
    if (!tree || !tree->getRoot()) {
        cerr << "Invalid quadtree.\n";
        return false;
    }

    int width = tree->getWidth();
    int height = tree->getHeight();
    int channels = 3;
    std::vector<unsigned char> imageData(width * height * channels, 255);

    auto setPixel = [&](int x, int y, const Color &color) {
        int idx = (y * width + x) * 3;
        imageData[idx + 0] = color.r;
        imageData[idx + 1] = color.g;
        imageData[idx + 2] = color.b;
    };

    std::function<void(QuadtreeNode*)> drawNode = [&](QuadtreeNode *node) {
        if (!node) return;
        if (node->is_leaf) {
            for (int y = node->y; y < node->y + node->height; ++y)
                for (int x = node->x; x < node->x + node->width; ++x)
                    setPixel(x, y, node->color);

            if (drawOutline) {
                Color black(0, 0, 0);
                for (int x = node->x; x < node->x + node->width; ++x) {
                    setPixel(x, node->y, black);
                    setPixel(x, node->y + node->height - 1, black);
                }
                for (int y = node->y; y < node->y + node->height; ++y) {
                    setPixel(node->x, y, black);
                    setPixel(node->x + node->width - 1, y, black);
                }
            }
        } else {
            for (int i = 0; i < 4; ++i)
                drawNode(node->children[i]);
        }
    };

    drawNode(tree->getRoot());

    int success = stbi_write_png(path.c_str(), width, height, channels, imageData.data(), width * channels);
    if (!success) {
        cerr << "Failed to write image: " << path << endl;
        return false;
    }

    return true;
}

long ImageIO::getFileSize(const std::string &path) {
    return std::filesystem::file_size(path);
}