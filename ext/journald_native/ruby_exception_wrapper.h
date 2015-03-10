#ifndef RUBY_EXCEPTION_WRAPPER_H
#define RUBY_EXCEPTION_WRAPPER_H

#include <ruby.h>

#include <functional>
#include <type_traits>
#include <utility>

#include "cpp14shiv.h" // make this c++14 code c++11 compatible; remove for c++14

// wrap functions, prototype required, i.e. RXW_WRAPPED_METHOD_FUNC(native_send, int, VALUE*, VALUE)
// functions can be inline
#define RXW_WRAPPED_METHOD_FUNC(method, ...) ((VALUE (*)(ANYARGS))::ruby_exception_wrapper::method_wrapper_call<decltype(&method), method, __VA_ARGS__>)

namespace ruby_exception_wrapper {

    class RbWrappedException: public std::exception {
        VALUE ruby_exception;
    public:
        RbWrappedException(VALUE e): ruby_exception(e) {};
        VALUE getRubyException() { return ruby_exception; }
    };

    // callback for rb_rescue2 to catch ruby exception and wrap it by RbWrappedException
    VALUE rethrow_as_cpp(VALUE put_exception_here_ptr, VALUE exception);

    namespace {
        // do real call of function from template
        template <typename FuncPointer, typename... Args>
        inline VALUE call_wrapper_tuple_impl(FuncPointer& fp, Args... args)
        {
            // only pointers and VALUE permitted
            // VALUE guaranteed to be able to contain pointer
            return reinterpret_cast<VALUE>(fp(args...));
        }

        // unpack params to do real call
        template <typename CallTuple, std::size_t... Is>
        inline VALUE call_wrapper_tuple_unpack(CallTuple& call_tuple, std::index_sequence<Is...>)
        {
            return call_wrapper_tuple_impl(std::get<Is>(call_tuple)...);
        }

        // callback for rb_rescue2 for no exceptions
        template <typename CallTuple>
        VALUE call_wrapper_tuple(VALUE v_ct) {
            CallTuple call_tuple = std::move(*reinterpret_cast<CallTuple*>(v_ct));

            return call_wrapper_tuple_unpack(call_tuple, std::make_index_sequence<std::tuple_size<CallTuple>::value> {});
        };

        // safely call function
        template <typename Func, typename... Args>
        inline auto do_raisable_call(Func f, Args... args) -> decltype(f(args...))
        {
            typedef std::function<typename std::remove_pointer<Func>::type> FuncPointer;
            typedef std::tuple<FuncPointer, Args...> CallTuple;

            FuncPointer fp = f;

            CallTuple call_tuple = std::make_tuple(fp, args...);

            VALUE exception = 0; // get raised exception if any

            VALUE result = rb_rescue2(
                    RUBY_METHOD_FUNC(call_wrapper_tuple<CallTuple>), reinterpret_cast<VALUE>(&call_tuple),
                    RUBY_METHOD_FUNC(rethrow_as_cpp), reinterpret_cast<VALUE>(&exception),
                    rb_eException, Qfalse
            );

            if (exception) { // nonzero here if rescue called unless some pervert throws Qfalse
                throw RbWrappedException(exception);
            }

            return reinterpret_cast<decltype(f(args...))>(result);
        }
    }

    template <typename Func, typename... Args>
    inline auto ruby_raisable_call(Func f, Args... args) -> decltype(f(args...))
    {
        static_assert(
                std::is_same<decltype(f(args...)), VALUE>::value || std::is_pointer<decltype(f(args...))>::value,
                "Only for funcs returning VALUE or pointer"
        );

        auto result = do_raisable_call(f, args...);

        return result;
    }

    template <typename Func, Func func, typename... Args>
    VALUE method_wrapper_call(Args... args)
    {
        try {
            return func(args...);
        } catch(RbWrappedException &e) {
            rb_exc_raise(e.getRubyException());
        }
    }

}

#endif
