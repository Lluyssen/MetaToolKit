#pragma once

#include <tuple>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <stdexcept>

// -- TypeTag ---
template <typename T>
struct TypeTag
{
    using type = T;
};

// --- TypeFallback<T> ---
template <typename T>
inline constexpr TypeTag<T>
TypeFallBack{};

// --- TypeList ---
template <typename... Ts>
struct TypeList {};

// --- Length ---
// taille d'une TypeList
template <typename List>
struct Length;

template <typename... Ts>
struct Length<TypeList<Ts...>>
{
    static constexpr std::size_t value = sizeof...(Ts);
};

// --- detection Tie ---
// vérifie si T fournit une métgode statique tie(T&)

// hasNew tie more accurate ?
template <typename T>
using removeCvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template <typename, typename = void>
struct HasTie : std::false_type {};

template <typename T>
struct HasTie<T, std::void_t<decltype(removeCvref_t<T>::tie(std::declval<removeCvref_t<T>&>()))>> : std::true_type {};

template <typename T>
constexpr bool hasTie = HasTie<T>::value;

// --- tie helper ---
// récupérer la fnc static Tie d'un T

template <typename T>
constexpr auto getTie(T& obj)
{
    return T::tie(obj);
}

template <typename T>
constexpr auto getTie(const T& obj)
{
    return T::tie(obj);
}

// --- StaticForEach ---
// Appelle Func pour chaque TypeTag<T> de la TypeList
template <typename... Ts, typename Func>
void StaticForEachImpl(TypeList<Ts...>, Func&& func)
{
    (func(TypeTag<Ts>{}), ...);
}

template <typename List, typename Func>
void StaticForEach(Func&& func)
{
    StaticForEachImpl(List{}, std::forward<Func>(func));
}

// --- StaticForEachWithIndex ---
// Appelle Func pour chaque TypeTag<T> de la TypeList et lui passe un Index

template <typename Func, typename... Ts>
void StaticForEachWithIndexImp(Func&& func, TypeList<Ts...>)
{
    std::size_t index = 0;
    (void)std::initializer_list<int>{(func(TypeTag<Ts>{}, index++), 0) ...};
}

template <typename List, typename Func>
void StaticForEachWithIndex(Func&& func)
{
    StaticForEachWithIndexImp(std::forward<Func>(func), List{});
}

// --- StaticForEachFiltered ---
// Applique une Func uniquement si Predicate<T> return true

template <template<typename> class Predicate, typename Func, typename ...Ts>
void StaticForEachFiltered(TypeList<Ts...>, Func&& func)
{
    (void(Predicate<Ts>::value ? (func(TypeTag<Ts>{}), 0) : 0), ...);
}

template <typename List, template<typename> class Predicate, typename Func>
void StaticForEachFiltered(Func&& func)
{
    StaticForEachFiltered<Predicate>(List{}, std::forward<Func>(func));
}

// --- StaticForEachWithEarlyStop ---
// Applique une Func avec possibilité d'arret si stop == true

template <typename List, typename Func>
struct StaticForEachStopHelper;

template <typename... Ts, typename Func>
struct StaticForEachStopHelper<TypeList<Ts...>, Func>
{
    static void apply(Func&& func)
    {
        bool stop = false;
        (void)([](...) {} ((stop ? 0 : (func(TypeTag<Ts>{}, stop), 0))...));
    }
};

template <typename List, typename Func>
void StaticForEachWithEarlyStop(Func&& func)
{
    StaticForEachStopHelper<List, Func>::apply(std::forward<Func>(func));
}

// --- TypeListContains ---
// Verifie Si un type est contenu dans une TypeList

template <typename T, typename List>
struct TypeListContains;

template <typename T>
struct TypeListContains<T, TypeList<>> : std::false_type {};

template <typename T, typename Head, typename... Tail>
struct TypeListContains<T, TypeList<Head, Tail...>> : std::conditional_t<std::is_same_v<T, Head>, std::true_type, TypeListContains<T, TypeList<Tail...>>> {};

// --- HasValueType ---
// Verifie si T possède un value_type

template <typename T>
struct HasValueType
{
    private:
        template<typename U> 
        static std::true_type test(typename U::value_type*);
        template<typename>
        static std::false_type test(...);
    public:
        static constexpr bool value = decltype(test<T>(nullptr))::value;
};

// --- IndexOf ---
// renvoie lindex d'un type dans une TypeList ou -1 si absent

template <typename T, typename List>
struct IndexOf;

template <typename T>
struct IndexOf<T, TypeList<>>
{
    static constexpr int value = -1;
};

template <typename T, typename Head, typename... Tail>
struct IndexOf<T, TypeList<Head, Tail...>>
{
    private:
        static constexpr int tail = IndexOf<T, TypeList<Tail...>>::value;
    public:
        static constexpr int value = std::is_same_v<T, Head> ? 0 : (tail == -1 ? -1 : 1 + tail);
};


// --- HasTypes ---
// vérifie si T posséde une T:Types

template <typename, typename = void>
struct HasTypes : std::false_type {};

template <typename T>
struct HasTypes <T, std::void_t<typename T::Types>> : std::true_type {};

// --- TypeListFrom ---
//Construit une TypeList depuis un type possédant T::Types

template <typename T, bool = HasTypes<T>::value>
struct TypeListFrom;

template <typename T>
struct TypeListFrom<T, true>
{
    using type = typename T::Types;
};

template <typename T>
struct TypeListFrom<T, false>
{
    using type = TypeList<>;
};

// --- TypeSwitch ---
// Dispatch en appelant fun(TypeTag<T>) selon l'index donné runtimerror si hors borne

template <typename List>
struct TypeSwitchImpl;

template <typename Head, typename... Tail>
struct TypeSwitchImpl<TypeList<Head, Tail...>>
{
    template <typename Func>
    static void dispatch(int index, Func&& func)
    {
        if (index == 0)
            func(TypeTag<Head>{});
        else
            TypeSwitchImpl<TypeList<Tail...>>::dispatch(index - 1, std::forward<Func>(func));
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
void TypeSwitch(int index, Func&& func)
{
    TypeSwitchImpl<List>::dispatch(index, std::forward<Func>(func));
}

// --- TypeSwitch Fallback---
// identique a TypeSwitch avec Apelle de function Fallback si l'index est hors borne

template <typename List>
struct TypeSwitchFallBackImpl;

template <typename Head, typename... Tail>
struct TypeSwitchFallBackImpl<TypeList<Head, Tail...>>
{
    template <typename Func, typename Fallback>
    static void dispatch(int index, Fallback fallback, Func&& func)
    {
        if (index == 0)
            func(TypeTag<Head>{});
        else
            TypeSwitchFallBackImpl<TypeList<Tail...>>::dispatch(index - 1, fallback, std::forward<Func>(func));
    }
};

template <>
struct TypeSwitchFallBackImpl<TypeList<>>
{
    template <typename Func, typename Fallback>
    static void dispatch(int, Fallback fallback, Func&& func)
    {
       func(fallback);
    }
};

template <typename List, typename Func, typename Fallback>
void TypeSwitch(int index, Fallback fallback, Func&& func)
{
    TypeSwitchFallBackImpl<List>::dispatch(index, fallback, std::forward<Func>(func));
}

// --- TypeSwitch Return ---
// version avec retour de valeur + fallback

template <typename List>
struct TypeSwitchReturnImpl;

template <typename Head, typename... Tail>
struct TypeSwitchReturnImpl<TypeList<Head, Tail...>>
{
    template <typename Func, typename R>
    static R dispatch(int index, R fallback, Func&& func)
    {
        if (index == 0)
            return func(TypeTag<Head>{});
        else
            return TypeSwitchReturnImpl<TypeList<Tail...>>::dispatch(index - 1, fallback, std::forward<Func>(func));
    }
};

template <>
struct TypeSwitchReturnImpl<TypeList<>>
{
    template <typename Func, typename R>
    static R dispatch(int, R fallback, Func&&)
    {
        return fallback;
    }
};

template <typename List, typename R, typename Func>
R TypeSwitchReturn(int index, R fallback, Func&& func)
{
    return TypeSwitchReturnImpl<List>::dispatch(index, fallback, std::forward<Func>(func));
}

// --- Concat List ---
// concaténe deux TypeList

template <typename L1, typename L2>
struct Concat;

template <typename... T1, typename... T2>
struct Concat<TypeList<T1...>, TypeList<T2...>>
{
    using type = TypeList<T1..., T2...>;
};

// --- GetTypeAt ---
// Accéde au type à l'index d'une typeList

template <std::size_t Index, typename List>
struct GetTypeAt
{
    static_assert(Index < Length<List>::value, "GetTypeAt : index out of range");
};

template <std::size_t Index, typename Head, typename... Tail>
struct GetTypeAt<Index, TypeList<Head, Tail...>>
{
    static_assert(Index < 1 + sizeof...(Tail), "GetTypeAt : index out of range");
    using type = typename GetTypeAt<Index - 1, TypeList<Tail...>>::type;
};

template <typename Head, typename... Tail>
struct GetTypeAt<0, TypeList<Head, Tail...>>
{
    using type = Head;
};

// --- Transform ---
// Applique d'une MetaFunc a cjauqe type d'une TypeList

template <typename List, template<typename> class MetaFunc>
struct Transform;

template <template<typename> class MetaFunc, typename... Ts>
struct Transform<TypeList<Ts...>, MetaFunc>
{
    using type = TypeList<typename MetaFunc<Ts>::type...>;
};

//COMPARAISON DE DEUX STRUCTURES EN UTILISANT STD::LIST

// verifie si T contientune Types interne donc est un type strucutré récursive
template <typename T, typename = void>
struct IsRecusif : std::false_type {};

template <typename T>
struct IsRecusif<T, std::void_t<typename T::Types>> : std::true_type {};

// recursive comparaison
template <typename AType, typename BType>
bool recursiveCompare(const AType&, const BType&);

// permet de check si operator== existe dans les structures
// use "requires" in c++20
template <typename T, typename = void>
struct hasEqualOperator : std::false_type {};

template <typename T>
struct hasEqualOperator<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> : std::true_type {};

// compare value call recusive si necessaire
template <typename AType, typename BType>
bool compareValue(const AType& a, const BType& b)
{
    if constexpr (HasTie<AType>::value && HasTie<BType>::value)
        return structEqual(a, b);
    else if constexpr (std::is_same<AType, BType>::value && hasEqualOperator<AType>::value)
        return a == b;
    else
    {
        static_assert("No tie and no operator==");
        return false;
    }
}

// recursive conparaison
template <typename AType, typename BType>
bool structEqual(const AType& a, const BType& b)
{
    using ATypes = typename AType::Types;
    using BTypes = typename BType::Types;
    if constexpr (!std::is_same_v<ATypes, BTypes>)
        return false;
    bool result = true;
    auto ta = AType::tie(a);
    auto tb = BType::tie(b);
    std::apply([&](const auto&... fieldA) {
        std::apply([&](const auto&... fieldB) {
            ((result &= compareValue(fieldA, fieldB)), ...);
        }, tb);
    }, ta);
    return result;
}

// --- Static Map --- 
// application d'une metaFunction MetaFunc<T>::type a chaque T d'une TypeList<Ts...>

template <typename List, template <typename> class MetaFunc>
struct StaticMap;

template <template <typename> class MetaFunc, typename... Ts>
struct StaticMap<TypeList<Ts...>, MetaFunc>
{
    using type = TypeList<typename MetaFunc<Ts>::type...>;
};

template <typename List, template <typename> class MetaFunc>
using StaticMapT = typename StaticMap<List, MetaFunc>::type;


// --- visitTupleAt

template <typename Tuple, typename Func, std::size_t... Is>
void visitTupleAtImpl(std::size_t index, const Tuple& t, Func&& fnc, std::index_sequence<Is...>)
{
    using expander = int[];
    (void)expander{0, (void(index == Is ? (void)fnc(std::get<Is>(t)) : void()), 0)...};
}

template <typename Tuple, typename Func>
void visitTupleAt(std::size_t index, const Tuple& t, Func&& func)
{
    constexpr std::size_t N = std::tuple_size<Tuple>::value;
    visitTupleAtImpl(index, t, std::forward<Func>(func), std::make_index_sequence<N>{});
}