#ifndef JOURNALD_NATIVE_JOURNALD_NATIVE_H
#define JOURNALD_NATIVE_JOURNALD_NATIVE_H

#include <string>
#include <ruby.h>

namespace journald_native {
    /* initializers */
    void init_modules();
    void init_constants(VALUE module);
    void init_methods(VALUE module);

    /* methods */
    VALUE native_print(VALUE self, VALUE priority, VALUE message);
    VALUE native_send(int argc, VALUE* argv, VALUE self);
    VALUE native_perror(VALUE self, VALUE message);

    /* aux */
    std::string create_safe_string(VALUE string);
}

#endif // JOURNALD_NATIVE_JOURNALD_NATIVE_H
