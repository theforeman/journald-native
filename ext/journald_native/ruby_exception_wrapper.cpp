#include "ruby_exception_wrapper.h"

namespace ruby_exception_wrapper {

// callback for rb_rescue2 to catch ruby exception and wrap it by RbWrappedException
VALUE rethrow_as_cpp(VALUE put_exception_here_ptr, VALUE exception)
{
    // cannot actually throw here, just pass the exception
    VALUE* excptr = reinterpret_cast<VALUE*>(put_exception_here_ptr);

    *excptr = exception;

    return exception;
}

}
