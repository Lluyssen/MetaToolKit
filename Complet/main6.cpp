#include "TypeList.hpp"

#include <iostream>

struct Login {std::string user = "admin";};
struct Logout {int id = 42;};
struct Ping {int code = 200;};

using MessageTypes = TypeList<Login, Logout, Ping>;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

template <typename TList>
struct Dispatcher
{
    template <typename Func>
    static void dispatch(const std::string& typName, Func&& func)
    {
        bool matched = false;
        StaticForEach<TList>([&]<typename Tag>(Tag) {
            using T = typename Tag::type;
            if (typName == typeid(T).name())
            {
                func(T{});
                matched = true;
            }
        });
        if (!matched)
            throw std::runtime_error("Unknow message type: " + typName);
    }
};

#pragma GCC diagnostic pop

void handle(Login l)
{
    std::cout << "Login: " << l.user << std::endl;
}

void handle(Logout l)
{
    std::cout << "Logout: " << l.id << std::endl;
}

void handle(Ping l)
{
    std::cout << "Ping: " << l.code << std::endl;
}

int main(void)
{
    std::string typeName = typeid(Login).name();

    Dispatcher<MessageTypes>::dispatch(typeName, [](auto msg) {
       handle(msg);
    });
    return 0;
}