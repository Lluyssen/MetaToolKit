#include <map>
#include <typeinfo>
#include <any>
#include <typeindex>
#include <iostream>

template <typename T>
struct TypeTag
{
    using type = T;
};

class A
{
    public:
    ~A() {std::cout << "AAAAA" << std::endl;}
};

class B : public A
{
    public:
    ~B() {std::cout << "BBBBB" << std::endl;}
};

int main(void)
{
    std::map<std::string, std::any> map;
    map[typeid(A).name()] = TypeTag<A>{};
    map[typeid(B).name()] = TypeTag<B>{};

    auto cast = std::any_cast<TypeTag<A>>(map[typeid(A).name()]);
    std::cout << typeid(cast).name() << std::endl;
    std::cout << typeid(A).name() << std::endl;

    TypeTag<int> test{};
    using T = typename decltype(test)::type;
    T i = 10;
    std::cout << i << std::endl;
    std::cout << typeid(T).name() << std::endl;
    return 0;
}