#include "journald_native.h"

/* initialize ruby extension */
extern "C" void Init_journald_native()
{
    journald_native::init_modules();
}
