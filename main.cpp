#include "meta_toolkit.hpp"
#include <iostream>
#include <string>

struct Vec3 {
    float x;
    float y;
    float z;
    static auto tie(const Vec3& self) { return std::tie(self.x, self.y, self.z); }
};

struct Transform {
    Vec3 pos;
    Vec3 rot;
    Vec3 scale;
    static auto tie(const Transform& self) { return std::tie(self.pos, self.rot, self.scale); }
};

int main() {
    Transform a{{1,2,3}, {0,0,0}, {1,1,1}};
    Transform b{{1,2,3}, {0,0,0}, {1,1,1}};
    Transform c{{1,0,0}, {0,0,0}, {1,1,1}};

    std::cout << std::boolalpha;
    std::cout << "a == b ? " << compareValue(a, b) << "\n";
    std::cout << "a == c ? " << compareValue(a, c) << "\n";
}