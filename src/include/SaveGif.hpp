#include <string>
#include <vector>
#include "Colors.hpp"

class SaveGif {
    public:
        static bool saveGIF(const std::string &gifPath, const std::vector<std::vector<std::vector<Color>>> &frames, int delayMs);
};