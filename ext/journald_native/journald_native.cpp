#include "journald_native.h"
#include "sd_journal.h"

#include <memory>

#include "ruby_exception_wrapper.h"

namespace journald_native {

// just a short alias for ruby_raisable_call()
template <typename Func, typename... Args>
inline auto r(Func f, Args... args) -> decltype(f(args...))
{
    ruby_exception_wrapper::ruby_raisable_call(f, args...);
}

/* initializers */
inline void init_constants(VALUE module);
inline void init_methods(VALUE module);

/* methods */
VALUE native_print(VALUE self, VALUE priority, VALUE message);
VALUE native_send(int argc, VALUE* argv, VALUE self);
VALUE native_perror(VALUE self, VALUE message);
inline VALUE native_print_impl(VALUE v_self, VALUE v_priority, VALUE v_message);
inline VALUE native_send_impl(int argc, VALUE* argv, VALUE self);
inline VALUE native_perror_impl(VALUE v_self, VALUE v_message);

/* aux */
std::string create_safe_string(VALUE string); // throws ruby exceptions

/* initializers */
void init_modules()
{
    // no nontrivial destructors during initialization, no need for ruby catch

    VALUE mJournald = rb_define_module("Journald");
    VALUE mNative   = rb_define_module_under(mJournald, "Native");

    init_constants(mJournald); // add constants to Journald
    init_methods(mNative);     // add methods to Journald::Native
}

inline void init_constants(VALUE module)
{
    rb_define_const(module, "LOG_EMERG",   INT2NUM(LOG_EMERG));    /* system is unusable */
    rb_define_const(module, "LOG_ALERT",   INT2NUM(LOG_ALERT));    /* action must be taken immediately */
    rb_define_const(module, "LOG_CRIT",    INT2NUM(LOG_CRIT));     /* critical conditions */
    rb_define_const(module, "LOG_ERR",     INT2NUM(LOG_ERR));      /* error conditions */
    rb_define_const(module, "LOG_WARNING", INT2NUM(LOG_WARNING));  /* warning conditions */
    rb_define_const(module, "LOG_NOTICE",  INT2NUM(LOG_NOTICE));   /* normal but significant condition */
    rb_define_const(module, "LOG_INFO",    INT2NUM(LOG_INFO));     /* informational */
    rb_define_const(module, "LOG_DEBUG",   INT2NUM(LOG_DEBUG));    /* debug-level messages */
}

inline void init_methods(VALUE module)
{
    rb_define_singleton_method(module, "print",  RUBY_METHOD_FUNC(native_print),  2);
    rb_define_singleton_method(module, "send",   RUBY_METHOD_FUNC(native_send),  -1); /* -1 to pass as C array */
    rb_define_singleton_method(module, "perror", RUBY_METHOD_FUNC(native_perror), 1);
}

VALUE native_print(VALUE v_self, VALUE v_priority, VALUE v_message)
{
    try {
        return native_print_impl(v_self, v_priority, v_message);
    } catch(ruby_exception_wrapper::RbWrappedException &e) {
        rb_exc_raise(e.getRubyException());
    }
}

VALUE native_send(int argc, VALUE* argv, VALUE v_self)
{
    try {
        return native_send_impl(argc, argv, v_self);
    } catch(ruby_exception_wrapper::RbWrappedException &e) {
        rb_exc_raise(e.getRubyException());
    }
}

VALUE native_perror(VALUE v_self, VALUE v_message)
{
    try {
        return native_perror_impl(v_self, v_message);
    } catch(ruby_exception_wrapper::RbWrappedException &e) {
        rb_exc_raise(e.getRubyException());
    }
}

/* methods */
inline VALUE native_print_impl(VALUE v_self, VALUE v_priority, VALUE v_message)
{
    int  priority = NUM2INT(v_priority);
    auto message  = create_safe_string(v_message); // ruby exception here

    int  result   = sd_journal_print(priority, "%s", message.c_str());

    return INT2NUM(result);
}

inline VALUE native_send_impl(int argc, VALUE* argv, VALUE v_self)
{
    auto msgs = std::make_unique<iovec[]>((size_t)argc);

    for (int i = 0; i < argc; i++) {
        VALUE v = argv[i];

        r(rb_string_value, &v);

        msgs[i].iov_base = (char *)RSTRING_PTR(v);
        msgs[i].iov_len  = (size_t)RSTRING_LEN(v);
    }

    int result = sd_journal_sendv(msgs.get(), argc);

    return INT2NUM(result);
}

inline VALUE native_perror_impl(VALUE v_self, VALUE v_message)
{
    auto message = create_safe_string(v_message); // ruby exception here

    int  result  = sd_journal_perror(message.c_str());

    return INT2NUM(result);
}

/**
 * Remove zeros from the string
 */
std::string create_safe_string(VALUE v_string)
{
    /* convert to string */
    r(rb_string_value, &v_string);

    char*  str = (char *)RSTRING_PTR(v_string);
    size_t len = (size_t)RSTRING_LEN(v_string);

    std::string safe_str;
    safe_str.reserve(len);

    for (size_t i = 0; i < len; i++) {
        if (str[i]) {
            safe_str += str[i];
        }
    }

    return safe_str;
}

}
