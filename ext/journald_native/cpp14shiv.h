/*
    unique_ptr from http://isocpp.org/files/papers/N3656.txt
    index sequence from gcc 4.9 sources
*/

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace std {
    template<class T> struct _Unique_if {
        typedef unique_ptr<T> _Single_object;
    };

    template<class T> struct _Unique_if<T[]> {
        typedef unique_ptr<T[]> _Unknown_bound;
    };

    template<class T, size_t N> struct _Unique_if<T[N]> {
        typedef void _Known_bound;
    };

    template<class T, class... Args>
        typename _Unique_if<T>::_Single_object
        make_unique(Args&&... args) {
            return unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

    template<class T>
        typename _Unique_if<T>::_Unknown_bound
        make_unique(size_t n) {
            typedef typename remove_extent<T>::type U;
            return unique_ptr<T>(new U[n]());
        }

    template<class T, class... Args>
        typename _Unique_if<T>::_Known_bound
        make_unique(Args&&...) = delete;

    /// Class template integer_sequence
    template<typename _Tp, _Tp... _Idx>
    struct integer_sequence
    {
        typedef _Tp value_type;
        static constexpr size_t size() { return sizeof...(_Idx); }
    };

    template<typename _Tp, _Tp _Num,
            typename _ISeq = typename _Build_index_tuple<_Num>::__type>
    struct _Make_integer_sequence;

    template<typename _Tp, _Tp _Num,  size_t... _Idx>
    struct _Make_integer_sequence<_Tp, _Num, _Index_tuple<_Idx...>>
    {
        static_assert( _Num >= 0,
                "Cannot make integer sequence of negative length" );

        typedef integer_sequence<_Tp, static_cast<_Tp>(_Idx)...> __type;
    };

    /// Alias template make_integer_sequence
    template<typename _Tp, _Tp _Num>
    using make_integer_sequence
    = typename _Make_integer_sequence<_Tp, _Num>::__type;

    /// Alias template index_sequence
    template<size_t... _Idx>
    using index_sequence = integer_sequence<size_t, _Idx...>;

    /// Alias template make_index_sequence
    template<size_t _Num>
    using make_index_sequence = make_integer_sequence<size_t, _Num>;

    /// Alias template index_sequence_for
    template<typename... _Types>
    using index_sequence_for = make_index_sequence<sizeof...(_Types)>;
}
