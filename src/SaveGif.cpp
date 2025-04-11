#include "SaveGif.hpp"
#include <iostream>
#include "gif.h"

bool SaveGif::saveGIF(const std::string &gifPath, const std::vector<std::vector<std::vector<Color>>> &frames, int delayMs) {
    if (frames.empty()) {
        std::cerr << "[ERROR] No frames provided to save GIF." << std::endl;
        return false;
    }

    int height = frames[0].size();
    int width = frames[0][0].size();

    GifWriter writer;
    if (!GifBegin(&writer, gifPath.c_str(), width, height, delayMs)) {
        std::cerr << "[ERROR] Failed to create GIF at path: " << gifPath << std::endl;
        return false;
    }

    for (const auto& frame : frames) {
        std::vector<uint8_t> frameData(width * height * 4);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int idx = (y * width + x) * 4;
                frameData[idx + 0] = frame[y][x].r;
                frameData[idx + 1] = frame[y][x].g;
                frameData[idx + 2] = frame[y][x].b;
                frameData[idx + 3] = 255;
            }
        }

        GifWriteFrame(&writer, frameData.data(), width, height, delayMs);
    }

    GifEnd(&writer);
    std::cout << "[INFO] GIF saved successfully to: " << gifPath << std::endl;
    return true;
}
