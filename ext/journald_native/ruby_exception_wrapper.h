#ifndef RUBY_EXCEPTION_WRAPPER_H
#define RUBY_EXCEPTION_WRAPPER_H

#include <ruby.h>

#include <functional>
#include <type_traits>
#include <utility>

namespace ruby_exception_wrapper {

    class RbWrappedException: public std::exception {
        VALUE ruby_exception;
    public:
        RbWrappedException(VALUE e): ruby_exception(e) {};
        VALUE getRubyException() { return ruby_exception; }
    };

    // callback for rb_rescue2 to catch ruby exception and wrap it by RbWrappedException
    VALUE rethrow_as_cpp(VALUE unused, VALUE exception);

    namespace {
        // do real call of function from template for func with params
        template <typename FuncPointer, typename... Args>
        inline VALUE call_wrapper_tuple_impl(FuncPointer& fp, Args... args)
        {
            return fp(args...);
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

            auto result = rb_rescue2(
                    RUBY_METHOD_FUNC(call_wrapper_tuple<CallTuple>), reinterpret_cast<VALUE>(&call_tuple),
                    RUBY_METHOD_FUNC(rethrow_as_cpp), Qnil,
                    rb_eException, Qfalse
            );

            return result;
        }
    }

    template <typename Func, typename... Args>
    inline auto ruby_raisable_call(Func f, Args... args) -> decltype(f(args...))
    {
        static_assert(std::is_same<decltype(f(args...)), VALUE>::value, "Currently only for funcs returning VALUE");

        auto result = do_raisable_call(f, args...);

        return result;
    }

}

#endif
