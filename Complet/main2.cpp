#include "TypeList.hpp"
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <vector>

struct Normalize
{
    void operator()(std::string& data) const
    {
        data += " [Normalized]";
    }
};

struct Compress
{
    void operator()(std::string& data) const
    {
        data += " [Compressed]";
    }
};

struct Encrypt
{
    void operator()(std::string& data) const
    {
        data += " [Encrypt]";
    }
};

using Pipeline = TypeList<Normalize, Compress, Encrypt>;

template<typename T>
using AcceptStringRef = std::is_invocable<T, std::string&>;

int main(void)
{
    std::cout << "--- Full Pipeline Execution ---" << std::endl;
    std::string input = "SensitiveData";
    StaticForEach<Pipeline>([&](auto tag) {
        using Step = typename decltype(tag)::type;
        Step{}(input);
    });
    std::cout << "Result: " << input << std::endl;

    std::cout << "\n--- Dispatch Step Dynamically ---" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::string tmp = "Data";
        TypeSwitch<Pipeline>(i, TypeFallBack<void>, [&](auto tag) {
            using Step = typename decltype(tag)::type;
            if constexpr (std::is_same_v<Step, void>)
                std::cout << "index " << i << " : invalid" << std::endl;
            else
            {
                Step{}(tmp);
                std::cout << "index " << i << " : " << tmp << std::endl;
            }
        });
    }

    std::cout << "\n--- Filtered Steps (accepts std::string&)" << std::endl;
    StaticForEachFiltered<Pipeline, AcceptStringRef>([](auto tag) {
        using Step = typename decltype(tag)::type;
        std::cout << "Step: " << typeid(Step).name() << std::endl;
    });
    return 0;
}