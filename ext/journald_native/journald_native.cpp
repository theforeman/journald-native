#include "journald_native.h"
#include "sd_journal.h"

#include <memory>

#include "cpp14shiv.h" // make this c++14 code c++11 compatible; remove for c++14

namespace journald_native {

/* initializers */
void init_modules()
{
    VALUE mJournald = rb_define_module("Journald");
    VALUE mNative   = rb_define_module_under(mJournald, "Native");

    init_constants(mJournald); // add constants to Journald
    init_methods(mNative);     // add methods to Journald::Native
}

void init_constants(VALUE module)
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

void init_methods(VALUE module)
{
    rb_define_singleton_method(module, "print",  RUBY_METHOD_FUNC(native_print),  2);
    rb_define_singleton_method(module, "send",   RUBY_METHOD_FUNC(native_send),  -1); /* -1 to pass as C array */
    rb_define_singleton_method(module, "perror", RUBY_METHOD_FUNC(native_perror), 1);
}

/* methods */
VALUE native_print(VALUE v_self, VALUE v_priority, VALUE v_message)
{
    int  priority = NUM2INT(v_priority);
    auto message  = create_safe_string(v_message); // ruby exception here

    int  result   = sd_journal_print(priority, "%s", message.c_str());

    return INT2NUM(result);
}

VALUE native_send(int argc, VALUE* argv, VALUE self)
{
    for (int i = 0; i < argc; i++) {
        StringValue(argv[i]); // ruby exception here
    }

    auto msgs = std::make_unique<iovec[]>(argc);

    for (int i = 0; i < argc; i++) {
        VALUE v = argv[i];

        msgs[i].iov_base = RSTRING_PTR(v);
        msgs[i].iov_len  = RSTRING_LEN(v);
    }

    int result = sd_journal_sendv(msgs.get(), argc);

    return INT2NUM(result);
}

VALUE native_perror(VALUE v_self, VALUE v_message)
{
    auto message = create_safe_string(v_message); // ruby exception here

    int  result  = sd_journal_perror(message.c_str());

    return INT2NUM(result);
}

/**
 * Remove zeros from string and ensure it's zero-terminated
 */
std::string create_safe_string(VALUE v_string)
{
    /* convert to string */
    StringValue(v_string); // ruby exception here
    // raising any ruby exception will not run any of the C++ destructors so get all required Ruby data first,
    // then use some objects

    std::string safe_str = "";

    char * str = RSTRING_PTR(v_string);
    size_t len = RSTRING_LEN(v_string);

    for (size_t i = 0; i < len; i++) {
        if (str[i]) {
            safe_str += str[i];
        }
    }

    return safe_str;
}

}
