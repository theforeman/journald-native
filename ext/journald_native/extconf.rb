require 'mkmf'

LIBDIR      = RbConfig::CONFIG['libdir']
INCLUDEDIR  = RbConfig::CONFIG['includedir']

HEADER_DIRS = [INCLUDEDIR]
LIB_DIRS    = [LIBDIR]

dir_config('systemd', HEADER_DIRS, LIB_DIRS)

$CFLAGS = '-std=c11'

def have_funcs
  # check functions. redefine const list in sd_journal.h if changed
  %w(sd_journal_print sd_journal_sendv sd_journal_perror).inject(true) do |have_funcs, func|
    have_funcs && have_func(func)
  end
end

# check headers
have_header('systemd/sd-journal.h')

# first try to find funcs in systemd
have_library('systemd')

unless have_funcs
  have_library('systemd-journal') # try to fall back to systemd-journal if older systemd
  have_funcs
end

create_header
create_makefile('journald_native')
