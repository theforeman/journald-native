/* dummy code to be used in sd_journal.h on non-linux system */

#include <stdlib.h>

/* syslog constants */
#define LOG_EMERG   0
#define LOG_ALERT   1
#define LOG_CRIT    2
#define LOG_ERR     3
#define LOG_WARNING 4
#define LOG_NOTICE  5
#define LOG_INFO    6
#define LOG_DEBUG   7

/* iovec */
struct iovec {
    void  *iov_base;    /* Starting address */
    size_t iov_len;     /* Number of bytes to transfer */
};

inline int sd_journal_print(int priority, const char *format, ...)  { return 0; }
inline int sd_journal_sendv(const struct iovec *iov, int n)         { return 0; }
inline int sd_journal_perror(const char *message)                   { return 0; }
