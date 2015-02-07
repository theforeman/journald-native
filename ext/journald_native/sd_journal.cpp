#include "sd_journal.h"

#ifdef JOURNALD_NATIVE_SYSTEMD_JOURNAL_DUMMY

extern "C" int sd_journal_print(int priority, const char *format, ...) { return 0; }
extern "C" int sd_journal_sendv(const struct iovec *iov, int n)        { return 0; }
extern "C" int sd_journal_perror(const char *message)                  { return 0; }

#endif
