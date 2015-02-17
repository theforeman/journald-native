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

    #include "sd_journal_dummy.h"

#endif

#endif // JOURNALD_NATIVE_SD_JOURNAL_H
