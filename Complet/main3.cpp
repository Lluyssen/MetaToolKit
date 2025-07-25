#include "TypeList.hpp"
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <vector>

struct IntWrapper
{
    static void print()
    {
        std::cout << "int" << std::endl;
    }
};

struct FloatWrapper
{
    static void print()
    {
        std::cout << "float" << std::endl;
    }
};

struct StringWrapper
{
    static void print()
    {
        std::cout << "string" << std::endl;
    }
};

struct SkipWrapper {};

template <typename T>
struct HasPrint
{
    template <typename U>
    static auto test (int) -> decltype(U::print(), std::true_type{});

    template<typename>
    static auto test (...) -> std::false_type;

    static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename T>
struct WrapperHandler
{
    using type = T;
};

int main(void)
{
    using MyTypes = TypeList<IntWrapper, FloatWrapper, StringWrapper>;

    //compile time error
    //using MyTypes = TypeList<IntWrapper, FloatWrapper, StringWrapper, SkipWrapper>;

    std::cout << "--- StaticForEach avec print ---" << std::endl;
    StaticForEachFiltered<MyTypes, HasPrint>([](auto tag) {
        using T = typename decltype(tag)::type;
        T::print();
    });

    std::cout << "\n--- TypeSxitch (dispatch à l'index) ---" << std::endl;
    TypeSwitch<MyTypes>(1, [](auto tag) {
        using T = typename decltype(tag)::type;
        if constexpr (HasPrint<T>::value)
            T::print();
        else
            std::cout << "No Print" << std::endl;
    });

    std::cout << "---\n IndexOf StringWrapper ---" << std::endl;
    using Transformed = typename Transform<MyTypes, WrapperHandler>::type;
    StaticForEach<Transformed>([](auto tag) {
        using T = typename decltype(tag)::type;
        if constexpr (HasPrint<T>::value)
            T::print();
    });
    return 0;
}
