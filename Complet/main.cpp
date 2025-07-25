#include "TypeList.hpp"
#include <iostream>
#include <type_traits>
#include <vector>

template <typename T>
struct AddPointer
{
    using type = T*;
};

int main(void)
{
    using MyTypes = TypeList<int, float, double, std::string>;

    std::cout << "--- StaticForEach ---" << std::endl;
    StaticForEach<MyTypes>([](auto tag) {
        using T = typename decltype(tag)::type;
        std::cout << typeid(T).name() << std::endl;
    });
    struct Position {float x,y;};
    struct Velocity {
        float dx;
        float dy;
        using Types = TypeList<float, float>;
    };
    using Components = TypeList<Position, Velocity>;
    StaticForEach<Components>([](auto tag) {
        using Component = typename decltype(tag)::type;
        std::cout << "Component : " << typeid(Component).name() << std::endl;
        if constexpr (HasTypes<Component>::value)
        {
            std::cout << " has sub-types : " << std::endl;
            StaticForEach<typename TypeListFrom<Component>::type>([](auto subTag) {
                using subType = typename decltype(subTag)::type;
                std::cout << "   - " << typeid(subType).name() << std::endl;
            });
        }
    });

    std::cout << "\n--- StaticForEachWithIndex ---" << std::endl;
    StaticForEachWithIndex<MyTypes>([](auto tag, size_t i) {
        using T = typename decltype(tag)::type;
        std::cout << i << " " << typeid(T).name() << std::endl;
    });

    std::cout << "\n--- StaticForEachWithEarlyStop ---" << std::endl;
    StaticForEachWithEarlyStop<MyTypes>([](auto tag, bool& stop) {
        using T = typename decltype(tag)::type;
        std::cout << "Vist: " << typeid(T).name() << std::endl;
        if constexpr (std::is_same_v<T, float>)
            stop = true;
    });
    std::cout << "\n--- StaticForEachFiltered ---" << std::endl;
    StaticForEachFiltered<MyTypes, std::is_integral>([](auto tag) {
        using T = typename decltype(tag)::type;
        std::cout << "Integral: " << typeid(T).name() << std::endl;
    });

    std::cout << "\n--- TypeListContains ---" << std::endl;
    //static_assert(TypeListContains<int, MyTypes>::value, "PRESENT"); //permet de crash directement a la compil
    //static_assert(TypeListContains<char, MyTypes>::value, "NOT PRESENT");
    std::cout << "Countains int : " << TypeListContains<int, MyTypes>::value << std::endl;
    std::cout << "Countains std::string : " << TypeListContains<std::string, MyTypes>::value << std::endl;
    std::cout << "Countains char : " << TypeListContains<char, MyTypes>::value << std::endl;

    std::cout << "\n--- HasValueType ---" << std::endl;
    std::cout << "std::vector<int> : " << HasValueType<std::vector<int>>::value << std::endl;
    std::cout << "int : " << HasValueType<int>::value << std::endl;

    std::cout << "\n--- IndexOf ---" << std::endl;
    std::cout << "IndexOf<int>: " << IndexOf<int, MyTypes>::value << std::endl;
    std::cout << "IndexOf<double>: " << IndexOf<double, MyTypes>::value << std::endl;
    std::cout << "IndexOf<char>: " << IndexOf<char, MyTypes>::value << std::endl;

    std::cout << "\n--- TypeListFrom ---" << std::endl;
    struct A {using Types = TypeList<int, float>;};
    struct B {};
    std::cout << "HasTypes<A> : " << HasTypes<A>::value << std::endl;
    std::cout << "HasTypes<B> : " << HasTypes<B>::value << std::endl;
    std::cout << "A contains int: " << TypeListContains<int, TypeListFrom<A>::type>::value <<std::endl;
    std::cout << "B contains int: " << TypeListContains<int, TypeListFrom<B>::type>::value <<std::endl;

    std::cout << "\n--- TypeSwitch ---" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        try
        {
            TypeSwitch<MyTypes>(i, [i](auto tag) {
                using T = typename decltype(tag)::type;
                std::cout << "Index " << i << " -> " << typeid(T).name() << std::endl;
            });
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    std::cout << "\n--- TypeSwitch FallBack ---" << std::endl;
    for (int i = 0; i < 5; i++)
    {
        TypeSwitch<MyTypes>(i, TypeTag<void>{}, [i](auto tag) {
            using T = typename decltype(tag)::type;
            if constexpr (std::is_same_v<T, void>)
                std::cout << "Index " << i << " -> fallback" << std::endl;
            else
                std::cout << "Index " << i << " -> " << typeid(T).name() << std::endl;
        });
    }

    std::cout << "\n--- TypeSwitch Return ---" << std::endl;
    for (int i = 0; i < 5; i++)
    {
        int code = TypeSwitchReturn<MyTypes>(i, -1, [](auto tag) -> int {
            using T = typename decltype(tag)::type;
            if constexpr (std::is_same_v<T, int>)
                return 10;
            if constexpr (std::is_same_v<T, float>)
                return 20;
            if constexpr (std::is_same_v<T, std::string>)
                return 30;
            return -1;
        });
        std::cout << "Index " << i << " -> code :" << code << std::endl;
    }

    std::cout << "\n--- Concat ---" << std::endl;
    using C = TypeList<int, int, double>;
    using D = Concat<C, MyTypes>::type;
    StaticForEach<D>([](auto tag) {
        using T = typename decltype(tag)::type;
        std::cout << " - " << typeid(T).name() << std::endl;
    });

    std::cout << "\n--- GetTypeAt ---" << std::endl;
    using SecondType = GetTypeAt<1, D>::type;
    std::cout << "GetTypeAt<1> = " << typeid(SecondType).name() << std::endl;

    std::cout << "\n--- StaticMap ---" << std::endl;
    using MyTypesWithOutPointer = TypeList<int, float, char>;
    using MyTypesWithPointers = StaticMapT<MyTypesWithOutPointer, AddPointer>; // => TypeList<int*, float*, char*>

    return 0;
}