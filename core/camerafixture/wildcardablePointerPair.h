#ifndef WILDCARDABLEPOINTERPAIR_H
#define WILDCARDABLEPOINTERPAIR_H

#include <unordered_map>

namespace corecvs {

template<typename U, typename V>
class WildcardablePointerPair
{
public:
#if !defined(WIN32) || (_MSC_VER >= 1900) // Sometime in future (when we switch to VS2015 due to https://msdn.microsoft.com/library/hh567368.apx ) we will get constexpr on windows
    static constexpr U* UWILDCARD = nullptr;
    static constexpr V* VWILDCARD = nullptr;
#else
    static U* const UWILDCARD;
    static V* const VWILDCARD;
#endif
    typedef U* UTYPE;
    typedef V* VTYPE;

    WildcardablePointerPair(U* u = UWILDCARD, V* v = VWILDCARD) : u(u), v(v)
    {
    }

    bool isWildcard() const
    {
        return u == UWILDCARD || v == VWILDCARD;
    }

    // Yes, this is NOT transitive (and you should use wildcarded wpps only for indexing not for insertion)
    bool operator== (const WildcardablePointerPair<U, V> &wpp) const
    {
        return (u == UWILDCARD || wpp.u == UWILDCARD || u == wpp.u) &&
               (v == VWILDCARD || wpp.v == VWILDCARD || v == wpp.v);
    }

    // This operator IS transitive
    bool operator< (const WildcardablePointerPair<U, V> &wpp) const
    {
        return u == wpp.u ? v < wpp.v : u < wpp.u;
    }

    U* u;
    V* v;
};

} //namespace corecvs

namespace std {

template<typename U, typename V>
struct hash<corecvs::WildcardablePointerPair<U, V>>
{
    size_t operator() (const corecvs::WildcardablePointerPair<U, V> &wpp) const
    {
        return std::hash<U*>()(wpp.u) ^ (31 * std::hash<V*>()(wpp.v));
    }
};

} // namespace std


#endif // WILDCARDABLEPOINTERPAIR_H
