
#pragma once

#include <tuple>
#include <type_traits>
#include <utility>
#include <stdexcept>

template <typename T>
struct TypeTag 
{ 
    using type = T; 
};

template <typename... Ts>
struct TypeList {};

template <typename T, typename = void>
constexpr bool hasTie = false;

template <typename T>
constexpr bool hasTie<T, std::void_t<decltype(T::tie(std::declval<T&>()))>> = true;

template <typename T>
auto getTie(T& obj) 
{
    if constexpr (hasTie<T>) 
        return T::tie(obj);
    else 
        return std::tie();
}

template <typename T>
auto getTie(const T& obj) 
{
    if constexpr (hasTie<T>) 
        return T::tie(obj);
    else 
        return std::tie();
}

template <typename... Ts, typename Func>
void StaticForEachImpl(TypeList<Ts...>, Func&& f) 
{
    (f(TypeTag<Ts>{}), ...);
}

template <typename List, typename Func>
void StaticForEach(Func&& f) 
{
    StaticForEachImpl(List{}, std::forward<Func>(f));
}

template <typename T, typename List>
struct IndexOf;

template <typename T>
struct IndexOf<T, TypeList<>> : std::integral_constant<int, -1> {};

template <typename T, typename Head, typename... Tail>
struct IndexOf<T, TypeList<Head, Tail...>> : std::integral_constant<int, std::is_same<T, Head>::value ? 0 : (IndexOf<T, TypeList<Tail...>>::value == -1 ? -1 : 1 + IndexOf<T, TypeList<Tail...>>::value)> {};

template <typename List>
struct TypeSwitchImpl;

template <typename Head, typename... Tail>
struct TypeSwitchImpl<TypeList<Head, Tail...>> 
{
    template <typename Func>
    static void dispatch(int index, Func&& f) 
    {
        if (index == 0)
            f(TypeTag<Head>{});
        else
            TypeSwitchImpl<TypeList<Tail...>>::dispatch(index - 1, std::forward<Func>(f));
    }
};

template <>
struct TypeSwitchImpl<TypeList<>> 
{
    template <typename Func>
    static void dispatch(int, Func&&) 
    {
        throw std::runtime_error("Type index out of range");
    }
};

template <typename List, typename Func>
void TypeSwitch(int index, Func&& f) 
{
    TypeSwitchImpl<List>::dispatch(index, std::forward<Func>(f));
}

template <typename A, typename B>
bool compareValue(const A& a, const B& b);

template <typename A, typename B>
bool recursiveCompare(const A& a, const B& b) 
{
    auto ta = getTie(a);
    auto tb = getTie(b);
    bool result = true;
    std::apply([&](auto&&... fa) {
        std::apply([&](auto&&... fb) {
            ((result &= compareValue(fa, fb)), ...);
        }, tb);
    }, ta);
    return result;
}

template <typename A, typename B>
bool compareValue(const A& a, const B& b) 
{
    if constexpr (hasTie<A> && hasTie<B>)
        return recursiveCompare(a, b);
    else
        return a == b;
}

template <typename A, typename B>
bool structEqual(const A& a, const B& b) 
{
    if constexpr (hasTie<A> && hasTie<B>)
        return getTie(a) == getTie(b);
    else
        return false;
}