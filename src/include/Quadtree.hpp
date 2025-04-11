#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <vector>
#include "Colors.hpp"

class QuadtreeNode {
public:
    int x, y;
    int width, height;
    int depth; 
    Color color;
    bool is_leaf;
    double error;

    QuadtreeNode* children[4];

    QuadtreeNode(int x, int y, int width, int height);
    ~QuadtreeNode();
};

class Quadtree {
private:
    QuadtreeNode* root;
    int width;
    int height;

    int countNodes(const QuadtreeNode* node) const;
    int maxDepth(const QuadtreeNode* node) const;

public:
    Quadtree(QuadtreeNode* root, int width, int height);
    ~Quadtree();

    QuadtreeNode* getRoot() const;
    int getWidth() const;
    int getHeight() const;

    int countNodes() const;
    int maxDepth() const;

    std::vector<std::vector<Color>> renderToPixels() const;
    std::vector<std::vector<Color>> renderAtDepth(int depthLevel) const;
};

#endif 