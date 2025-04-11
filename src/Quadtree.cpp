#include "Quadtree.hpp"
#include <algorithm>

QuadtreeNode::QuadtreeNode(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height), depth(0),
      color(0, 0, 0), is_leaf(false), error(0.0) {
    for (int i = 0; i < 4; ++i)
        children[i] = nullptr;
}

QuadtreeNode::~QuadtreeNode() {
    for (int i = 0; i < 4; ++i) {
        delete children[i];
        children[i] = nullptr;
    }
}

Quadtree::Quadtree(QuadtreeNode* root, int width, int height)
    : root(root), width(width), height(height) {}

Quadtree::~Quadtree() {
    delete root;
}

static void fillBlock(std::vector<std::vector<Color>>& pixels, QuadtreeNode* node) {
    if (!node) return;

    if (node->is_leaf) {
        for (int i = node->y; i < node->y + node->height; ++i)
            for (int j = node->x; j < node->x + node->width; ++j)
                pixels[i][j] = node->color;
    } else {
        for (int i = 0; i < 4; ++i)
            fillBlock(pixels, node->children[i]);
    }
}

std::vector<std::vector<Color>> Quadtree::renderToPixels() const {
    std::vector<std::vector<Color>> result(height, std::vector<Color>(width));
    fillBlock(result, root);
    return result;
}

static void drawWithDepth(QuadtreeNode* node, int targetDepth, std::vector<std::vector<Color>>& canvas) {
    if (!node) return;

    if (node->depth == targetDepth || node->is_leaf) {
        for (int i = node->y; i < node->y + node->height; ++i)
            for (int j = node->x; j < node->x + node->width; ++j)
                canvas[i][j] = node->color;
    } else {
        for (int i = 0; i < 4; ++i)
            drawWithDepth(node->children[i], targetDepth, canvas);
    }
}

std::vector<std::vector<Color>> Quadtree::renderAtDepth(int depthLevel) const {
    std::vector<std::vector<Color>> result(height, std::vector<Color>(width, Color(200, 200, 200)));;
    drawWithDepth(root, depthLevel, result);
    return result;
}

int Quadtree::countNodes(const QuadtreeNode* node) const {
    if (!node) return 0;
    if (node->is_leaf) return 1;

    int total = 1;
    for (int i = 0; i < 4; ++i)
        total += countNodes(node->children[i]);
    return total;
}

int Quadtree::maxDepth(const QuadtreeNode* node) const {
    if (!node || node->is_leaf) return 1;

    int depth = 0;
    for (int i = 0; i < 4; ++i)
        depth = std::max(depth, maxDepth(node->children[i]));
    return depth + 1;
}

int Quadtree::countNodes() const {
    return countNodes(root);
}

int Quadtree::maxDepth() const {
    return maxDepth(root);
}

QuadtreeNode* Quadtree::getRoot() const {
    return root;
}

int Quadtree::getWidth() const {
    return width;
}

int Quadtree::getHeight() const {
    return height;
}