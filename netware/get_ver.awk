#!awk
# awk script which fetches libgd version number and string from input file
# and writes them to STDOUT. Here you can get an awk version for Win32:
# http://www.gknw.net/development/prgtools/awk.zip
# $Id$
#
BEGIN {
    while ((getline < ARGV[1]) > 0) {
      if (match ($0, /^GDLIB_MAJOR=([0-9]*)$/)) {
        split($1, t, "=");
        v_maj = t[2];
      }
      if (match ($0, /^GDLIB_MINOR=([0-9]*)$/)) {
        split($1, t, "=");
        v_min = t[2];
      }
      if (match ($0, /^GDLIB_REVISION=([0-9]*)$/)) {
        split($1, t, "=");
        v_rev = t[2];
      }
    }
    libgd_ver = v_maj "," v_min "," v_rev;
    libgd_ver_str = v_maj "." v_min "." v_rev;
    print "LIBGD_VERSION = " libgd_ver "";
    print "LIBGD_VERSION_STR = " libgd_ver_str "";
}


