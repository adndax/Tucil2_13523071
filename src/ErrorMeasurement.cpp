#include "ErrorMeasurement.hpp"
#include <cmath>
#include <vector>

double ErrorMeasurement::variance(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height) {
    int count = width * height;
    double sumR = 0, sumG = 0, sumB = 0;

    for (int i = y; i < y + height; i++)
        for (int j = x; j < x + width; j++) {
            sumR += pixels[i][j].r;
            sumG += pixels[i][j].g;
            sumB += pixels[i][j].b;
        }

    double avgR = sumR / count;
    double avgG = sumG / count;
    double avgB = sumB / count;

    double var = 0.0;
    for (int i = y; i < y + height; i++)
        for (int j = x; j < x + width; j++) {
            var += std::pow(pixels[i][j].r - avgR, 2)
                 + std::pow(pixels[i][j].g - avgG, 2)
                 + std::pow(pixels[i][j].b - avgB, 2);
        }

    return var / count;
}

double ErrorMeasurement::mad(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height) {
    int count = width * height;
    double sumR = 0, sumG = 0, sumB = 0;

    for (int i = y; i < y + height; i++)
        for (int j = x; j < x + width; j++) {
            sumR += pixels[i][j].r;
            sumG += pixels[i][j].g;
            sumB += pixels[i][j].b;
        }

    double avgR = sumR / count;
    double avgG = sumG / count;
    double avgB = sumB / count;

    double mad = 0.0;
    for (int i = y; i < y + height; i++)
        for (int j = x; j < x + width; j++) {
            mad += std::abs(pixels[i][j].r - avgR)
                 + std::abs(pixels[i][j].g - avgG)
                 + std::abs(pixels[i][j].b - avgB);
        }

    return mad / count;
}

double ErrorMeasurement::maxPixelDifference(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height) {
    int minR = 255, minG = 255, minB = 255;
    int maxR = 0, maxG = 0, maxB = 0;
    for (int i = y; i < y + height; i++)
        for (int j = x; j < x + width; j++) {
            const Color& c = pixels[i][j];
            if (c.r < minR) minR = c.r;
            if (c.g < minG) minG = c.g;
            if (c.b < minB) minB = c.b;
            if (c.r > maxR) maxR = c.r;
            if (c.g > maxG) maxG = c.g;
            if (c.b > maxB) maxB = c.b;
        }
    double diffR = maxR - minR;
    double diffG = maxG - minG;
    double diffB = maxB - minB;
    return (diffR + diffG + diffB) / 3.0;
}

double ErrorMeasurement::entropy(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height) {
    const int count = width * height;
    std::vector<int> histR(MAX_COLOR, 0), histG(MAX_COLOR, 0), histB(MAX_COLOR, 0);
    
    for (int i = y; i < y + height; ++i)
        for (int j = x; j < x + width; ++j) {
            const Color& c = pixels[i][j];
            ++histR[c.r];
            ++histG[c.g];
            ++histB[c.b];
        }
    
    auto computeEntropyChannel = [count](const std::vector<int>& hist) -> double {
        double ent = 0.0;
        for (int i = 0; i < MAX_COLOR; ++i) {
            if (hist[i] > 0) {
                double p = static_cast<double>(hist[i]) / count;
                ent -= p * std::log2(p);
            }
        }
        return ent;
    };
    
    return (computeEntropyChannel(histR) + computeEntropyChannel(histG) + computeEntropyChannel(histB)) / 3.0;
}

double ErrorMeasurement::ssim(const std::vector<std::vector<Color>>& pixels, int x, int y, int width, int height) {
    const double L = 255.0;
    const double C1 = (0.01 * L) * (0.01 * L);
    const double C2 = (0.03 * L) * (0.03 * L);
    int N = width * height;

    if (N == 0) return 1.0;

    std::vector<double> weights = {0.2125, 0.7154, 0.0721}; 
    double totalWeight = weights[0] + weights[1] + weights[2];

    double totalSSIM = 0.0;

    for (int channel = 0; channel < 3; ++channel) {
        double mu = 0.0;
        double sigma = 0.0;

        for (int i = y; i < y + height; ++i) {
            for (int j = x; j < x + width; ++j) {
                double val = 0.0;
                if (channel == 0) val = pixels[i][j].r;
                else if (channel == 1) val = pixels[i][j].g;
                else if (channel == 2) val = pixels[i][j].b;
                mu += val;
            }
        }
        mu /= N;

        for (int i = y; i < y + height; ++i) {
            for (int j = x; j < x + width; ++j) {
                double val = 0.0;
                if (channel == 0) val = pixels[i][j].r;
                else if (channel == 1) val = pixels[i][j].g;
                else if (channel == 2) val = pixels[i][j].b;
                sigma += (val - mu) * (val - mu);
            }
        }
        sigma /= N;

        double mu_y = mu;
        double sigma_y = 0.0;
        double covariance = 0.0;

        double numerator = (2 * mu * mu_y + C1) * (2 * covariance + C2);
        double denominator = (mu * mu + mu_y * mu_y + C1) * (sigma + sigma_y + C2);
        double ssim = (denominator == 0.0) ? 1.0 : numerator / denominator;

        totalSSIM += ssim * weights[channel];
    }

    double ssim_avg = totalSSIM / totalWeight;
    return 1.0 - ssim_avg;
}