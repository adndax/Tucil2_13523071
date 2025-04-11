#include "ImageCompressor.hpp"
#include "ImageIO.hpp"
#include "SaveGif.hpp"
#include <iostream>
#include <cmath>
#include <chrono>
#include <functional>
#include <stdexcept>

ImageCompressor::ImageCompressor()
    : threshold(0), min_block_size(1), errorFunc(ErrorMeasurement::variance) {}

Color ImageCompressor::getAverageColor(const std::vector<std::vector<Color>>& image_data, int x, int y, int width, int height) {
    long sum_r = 0, sum_g = 0, sum_b = 0;
    for (int i = y; i < y + height; ++i)
        for (int j = x; j < x + width; ++j) {
            sum_r += image_data[i][j].r;
            sum_g += image_data[i][j].g;
            sum_b += image_data[i][j].b;
        }
    int total = width * height;
    return Color(sum_r / total, sum_g / total, sum_b / total);
}

bool ImageCompressor::shouldDivide(double error, int width, int height) {
    if (width <= min_block_size || height <= min_block_size) return false;
    if (error <= threshold) return false;
    return true;
}

QuadtreeNode* ImageCompressor::build(const std::vector<std::vector<Color>>& image_data,
                                     int x, int y, int width, int height, int depth) {
    QuadtreeNode* node = new QuadtreeNode(x, y, width, height);
    node->depth = depth;

    double error = errorFunc(image_data, x, y, width, height);

    if (!shouldDivide(error, width, height)) {
        node->color = getAverageColor(image_data, x, y, width, height);
        node->is_leaf = true;
        return node;
    }

    node->is_leaf = false;

    int half_width = width / 2;
    int half_height = height / 2;

    node->children[0] = build(image_data, x, y, half_width, half_height, depth + 1);
    node->children[1] = build(image_data, x + half_width, y, width - half_width, half_height, depth + 1);
    node->children[2] = build(image_data, x, y + half_height, half_width, height - half_height, depth + 1);
    node->children[3] = build(image_data, x + half_width, y + half_height, width - half_width, height - half_height, depth + 1);

    return node;
}

Quadtree* ImageCompressor::compress(const std::vector<std::vector<Color>>& image_data,
                                    double target_compression,
                                    const std::string& tempPath,
                                    long originalSize,
                                    std::vector<std::vector<std::vector<Color>>>* gifFrames) {
    if (target_compression <= 0.0) {
        // Kompresi biasa tanpa target
        int height = image_data.size();
        int width = image_data[0].size();
        QuadtreeNode* root = build(image_data, 0, 0, width, height, 0);
        return new Quadtree(root, width, height);
    }

    double currentThreshold = this->threshold;
    const double maxThreshold = 1000.0;
    const double step = 5.0;
    const int maxSteps = 200;
    double bestRatio = 0.0;
    Quadtree* bestTree = nullptr;

    for (int stepCount = 0; currentThreshold <= maxThreshold && stepCount < maxSteps; ++stepCount, currentThreshold += step) {
        ImageCompressor tempCompressor;
        tempCompressor.threshold = currentThreshold;
        tempCompressor.min_block_size = min_block_size;
        tempCompressor.setErrorFunction(this->errorFunc); 

        Quadtree* tempTree = tempCompressor.compress(image_data, 0.0, "", 0, nullptr); 
        ImageIO::saveImage(tempPath, tempTree, false);

        long tempSize = ImageIO::getFileSize(tempPath);
        double ratio = 1.0 - static_cast<double>(tempSize) / originalSize;
        std::cout << "\033[1;36m[OUTPUT]\033[0m [THRESHOLD = " << currentThreshold << "] Compression: " << ratio * 100 << "%\n";

        if (ratio >= target_compression) {
            if (bestTree) delete bestTree;
            return tempTree;
        }
        if (ratio > bestRatio) {
            if (bestTree) delete bestTree;
            bestTree = tempTree;
            bestRatio = ratio;
        } else {
            delete tempTree;
        }
        
    }

    std::cout << "\033[1;36m[OUTPUT]\033[0m [INFO] Target not reached. Using best compression: " << bestRatio * 100 << "%\n";
    return bestTree;
}

void ImageCompressor::setErrorFunction(std::function<double(const std::vector<std::vector<Color>>&, int, int, int, int)> func) {
    errorFunc = func;
}

void ImageCompressor::run() {
    std::string inputPath, outputPath, gifPath;
    int methodChoice = 0;
    double targetCompression = 0.0;
    int saveGifAnswer = -1;
    bool drawOutline = false;

    std::cout << "\033[1;36m[INPUT]\033[0m Enter image path: ";
    std::getline(std::cin, inputPath);
    while (inputPath.empty()) {
        std::cerr << "\033[1;31m[ERROR]\033[0m Image path cannot be empty. Please enter again: ";
        std::getline(std::cin, inputPath);
    }

    std::cout << "\033[1;36m[INPUT]\033[0m Choose error metric (1-5):\n"
              << "   1. Variance\n   2. MAD\n   3. Max Pixel Difference\n   4. Entropy\n   5. SSIM\n"
              << "\033[1;36m[INPUT]\033[0m Your choice: ";
    while (!(std::cin >> methodChoice) || methodChoice < 1 || methodChoice > 5) {
        std::cin.clear(); std::cin.ignore(10000, '\n');
        std::cerr << "\033[1;31m[ERROR]\033[0m Invalid metric. Please enter 1–5: ";
    }

    std::cout << "\033[1;36m[INPUT]\033[0m Enter threshold (>= 0): ";
    while (!(std::cin >> threshold) || threshold < 0) {
        std::cin.clear(); std::cin.ignore(10000, '\n');
        std::cerr << "\033[1;31m[ERROR]\033[0m Threshold must be >= 0. Please re-enter: ";
    }

    std::cout << "\033[1;36m[INPUT]\033[0m Enter minimum block size (>= 1): ";
    while (!(std::cin >> min_block_size) || min_block_size < 1) {
        std::cin.clear(); std::cin.ignore(10000, '\n');
        std::cerr << "\033[1;31m[ERROR]\033[0m Minimum block size must be >= 1. Please re-enter: ";
    }

    std::cout << "\033[1;36m[INPUT]\033[0m Enter target compression (0–1, 0 to disable): ";
    while (!(std::cin >> targetCompression) || targetCompression < 0.0 || targetCompression > 1.0) {
        std::cin.clear(); std::cin.ignore(10000, '\n');
        std::cerr << "\033[1;31m[ERROR]\033[0m Compression must be between 0 and 1. Please re-enter: ";
    }

    std::cout << "\033[1;36m[INPUT]\033[0m Draw outline? (1 = yes, 0 = no): ";
    int outlineInput;
    while (!(std::cin >> outlineInput) || (outlineInput != 0 && outlineInput != 1)) {
        std::cin.clear(); std::cin.ignore(10000, '\n');
        std::cerr << "\033[1;31m[ERROR]\033[0m Please enter 0 or 1: ";
    }
    drawOutline = (outlineInput == 1);
    std::cin.ignore();

    std::cout << "\033[1;36m[INPUT]\033[0m Enter output image path: ";
    std::getline(std::cin, outputPath);
    while (outputPath.empty()) {
        std::cerr << "\033[1;31m[ERROR]\033[0m Output path cannot be empty. Please enter again: ";
        std::getline(std::cin, outputPath);
    }

    std::cout << "\033[1;36m[INPUT]\033[0m Save as GIF? (1 = yes, 0 = no): ";
    while (!(std::cin >> saveGifAnswer) || (saveGifAnswer != 0 && saveGifAnswer != 1)) {
        std::cin.clear(); std::cin.ignore(10000, '\n');
        std::cerr << "\033[1;31m[ERROR]\033[0m Please enter 0 or 1: ";
    }
    std::cin.ignore();
    if (saveGifAnswer == 1) {
        std::cout << "\033[1;36m[INPUT]\033[0m Enter GIF path: ";
        std::getline(std::cin, gifPath);
        while (gifPath.empty()) {
            std::cerr << "\033[1;31m[ERROR]\033[0m GIF path cannot be empty. Please enter again: ";
            std::getline(std::cin, gifPath);
        }
    }

    std::vector<std::vector<Color>> pixelData;
    if (!ImageIO::loadImage(inputPath, pixelData)) {
        std::cerr << "\033[1;31m[ERROR]\033[0m Failed to load input image.\n";
        return;
    }

    switch (methodChoice) {
        case 1: setErrorFunction(ErrorMeasurement::variance); break;
        case 2: setErrorFunction(ErrorMeasurement::mad); break;
        case 3: setErrorFunction(ErrorMeasurement::maxPixelDifference); break;
        case 4: setErrorFunction(ErrorMeasurement::entropy); break;
        case 5: setErrorFunction(ErrorMeasurement::ssim); break;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    const std::string tempPath = "temp_result.jpg";
    std::vector<std::vector<std::vector<Color>>> gifFrames;
    Quadtree* tree = compress(pixelData, targetCompression, tempPath, ImageIO::getFileSize(inputPath),
                              gifPath.empty() ? nullptr : &gifFrames);

    ImageIO::saveImage(outputPath, tree, drawOutline);

    if (!gifPath.empty()) {
        for (int d = 1; d <= tree->maxDepth(); ++d) {
            gifFrames.push_back(tree->renderAtDepth(d));
        }
    
        if (SaveGif::saveGIF(gifPath, gifFrames, 100)) {
            std::cout << "\033[1;36m[OUTPUT]\033[0m GIF saved at: " << gifPath << "\n";
        } else {
            std::cerr << "\033[1;31m[ERROR]\033[0m Failed to save GIF.\n";
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double execTime = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "\n\033[1;36m[OUTPUT]\033[0m ========= COMPRESSION REPORT =========\n";
    std::cout << "\033[1;36m[OUTPUT]\033[0m Execution time        : " << execTime << " ms\n";
    std::cout << "\033[1;36m[OUTPUT]\033[0m Original image size   : " << ImageIO::getFileSize(inputPath) / 1024.0 << " KB\n";
    std::cout << "\033[1;36m[OUTPUT]\033[0m Compressed image size : " << ImageIO::getFileSize(outputPath) / 1024.0 << " KB\n";
    std::cout << "\033[1;36m[OUTPUT]\033[0m Compression percentage: "
              << (1.0 - ImageIO::getFileSize(outputPath) / static_cast<double>(ImageIO::getFileSize(inputPath))) * 100.0 << "%\n";
    std::cout << "\033[1;36m[OUTPUT]\033[0m Tree depth            : " << tree->maxDepth() << "\n";
    std::cout << "\033[1;36m[OUTPUT]\033[0m Total nodes           : " << tree->countNodes() << "\n";
    std::cout << "\033[1;36m[OUTPUT]\033[0m Output image path     : " << outputPath << "\n";

    delete tree;
    std::cin.ignore();
}