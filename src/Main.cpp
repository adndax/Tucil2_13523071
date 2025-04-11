// src/main.cpp

#include <iostream>
#include "ImageCompressor.hpp"

int main() {
    std::cout << "============================================\n";
    std::cout << "           Welcome to QuaQua Image          \n";
    std::cout << "       Quadtree -- Divide and Conquer       \n";
    std::cout << "============================================\n\n";

    try {
        ImageCompressor compressor;
        compressor.run();
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}