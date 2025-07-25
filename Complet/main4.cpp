#include <tuple>
#include <string>
#include <iostream>
#include <type_traits>
#include "TypeList.hpp"
#include <cstring>

struct Inner
{
    int id;
    float weight;
    bool operator==(const Inner& other) const
    {
        return id == other.id && (abs(weight - other.weight) < 1E-6 * weight);
    }

    using Types = TypeList<int, float>;
    static auto tie(const Inner& obj)
    {
        return std::tie(obj.id, obj.weight);
    }
};

struct pentium
{
    int id;
    float f;

    using Types = TypeList<int, float>;
    static auto tie(const pentium& obj)
    {
        return std::tie(obj.id, obj.f);
    }
};

struct amd
{
    pentium p;
    int id;

    using Types = TypeList<pentium, int>;
    static auto tie(const amd& obj)
    {
        return std::tie(obj.p, obj.id);
    }
};

struct intel
{
    std::string s;
    amd a;

    using Types = TypeList<std::string, amd>;
    static auto tie(const intel& obj)
    {
        return std::tie(obj.s, obj.a);
    }
};

struct A
{
    Inner inner;
    std::string s;

    using Types = TypeList<Inner, std::string>;
    static auto tie(const A& obj)
    {
        return std::tie(obj.inner, obj.s);
    }
};

struct B
{
    int id;
    float weight;
    std::string z;

    using Types = TypeList<int, float, std::string>;
    static auto tie(const B& obj)
    {
        return std::tie(obj.id, obj.weight, obj.z);
    }
};

struct C
{
    int i;
    float f;
    std::string s;

    using Types = TypeList<int, float, std::string>;
    static auto tie(const C& obj)
    {
        return std::tie(obj.i, obj.f, obj.s);
    }
};

int main(void)
{
    //recursive
    A a1{{1, 2.5f}, "VICTOR"};
    A a2{{1, 2.555555f}, "VICTOR"};
    std::cout << "a1 == a1: " << std::boolalpha << structEqual(a1, a1) << std::endl;
    std::cout << "a1 == a2: " << std::boolalpha << structEqual(a1, a2) << std::endl;

    //classic
    B b1{42, 3.14f, "VICTOR"};
    B b2{42, 3.14f, "THEGREAT"};
    C c1{42, 3.14f, "VICTOR"};
    C c2{42, 3.14f, "THEGREAT"};
    std::cout << "b1 == b1: " << std::boolalpha << structEqual(b1, b1) << std::endl;
    std::cout << "b1 == b2: " << std::boolalpha << structEqual(b1, b2) << std::endl;
    std::cout << "c1 == c2: " << std::boolalpha << structEqual(c1, c2) << std::endl;
    std::cout << "c1 == b1: " << std::boolalpha << structEqual(c1, b1) << std::endl;
    std::cout << "c1 == b2: " << std::boolalpha << structEqual(c1, b2) << std::endl;
    std::cout << "i1 == i1: " << std::boolalpha << structEqual(b1, b1) << std::endl;

    //recursive recursive
    intel i1{"intel", {{10, 14.0f}, 10}};
    intel i2{"intel", {{10, 14.4444f}, 10}};
    std::cout << "i1 == i1: " << std::boolalpha << structEqual(i1, i1) << std::endl;
    std::cout << "i1 == i2: " << std::boolalpha << structEqual(i1, i2) << std::endl;
    return 0;
}