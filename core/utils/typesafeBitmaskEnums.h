#ifndef TYPESAFEBITMASKENUMS
#define TYPESAFEBITMASKENUMS

#include <type_traits>

/*
 * Some magic for strong-typed bit-mask enums
 */
namespace corecvs
{
template<typename E>
struct is_bitmask : std::false_type {};

template<typename E>
typename std::enable_if<is_bitmask<E>::value, E>::type operator & (const E &lhs, const E &rhs)
{
    typedef typename std::underlying_type<E>::type U;
    return static_cast<E>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

template<typename E>
typename std::enable_if<is_bitmask<E>::value, E>::type operator ~ (const E &lhs)
{
    typedef typename std::underlying_type<E>::type U;
    return static_cast<E>(~static_cast<U>(lhs));
}


template<typename E>
typename std::enable_if<is_bitmask<E>::value, E>::type operator | (const E &lhs, const E &rhs)
{
    typedef typename std::underlying_type<E>::type U;
    return static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

// Unfortunately, it's impossible to add cast to bool to enum class, so let's use !! semantic
template<typename E>
typename std::enable_if<is_bitmask<E>::value, bool>::type operator !(const E &lhs)
{
    typedef typename std::underlying_type<E>::type U;
    return !static_cast<bool>(static_cast<U>(lhs));
}

template<typename E>
typename std::enable_if<std::is_enum<E>::value, typename std::underlying_type<E>::type>::type asInteger(E e)
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}
}

#endif
