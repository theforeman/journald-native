#ifndef JOURNALD_NATIVE_SD_JOURNAL_H
#define JOURNALD_NATIVE_SD_JOURNAL_H

#ifdef __linux__

    /* do the real stuff */

    #include "extconf.h"

    /* check for extconf results */

    #ifndef HAVE_SYSTEMD_SD_JOURNAL_H
        #error Cannot include <systemd/sd-journal.h>. Please use linux version with systemd-journal installed
    #endif

    #ifndef HAVE_SD_JOURNAL_PRINT
        #error Required function sd_journal_print is missing
    #endif

    #ifndef HAVE_SD_JOURNAL_SENDV
        #error Required function sd_journal_sendv is missing
    #endif

    #ifndef HAVE_SD_JOURNAL_PERROR
        #error Required function sd_journal_perror is missing
    #endif

    /* Do not add C line and file to the log messages */
    #define SD_JOURNAL_SUPPRESS_LOCATION
    /* include systemd-journal headers */
    #include <systemd/sd-journal.h>

#else

    #warning Compiling dummy version of the gem for non-Linux OS

    #include <stdlib.h>

    /* use dummy */
    #define JOURNALD_NATIVE_SYSTEMD_JOURNAL_DUMMY

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

    extern "C" {
        int sd_journal_print(int priority, const char *format, ...);
        int sd_journal_sendv(const struct iovec *iov, int n);
        int sd_journal_perror(const char *message);
    }

#endif

#endif
