#include "ruby_exception_wrapper.h"

namespace ruby_exception_wrapper {

// callback for rb_rescue2 to catch ruby exception and wrap it by RbWrappedException
VALUE rethrow_as_cpp(VALUE unused, VALUE exception)
{
    rb_gc_register_address(&exception);
    throw RbWrappedException(exception);
}

}
