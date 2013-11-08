#!/usr/bin/env perl

# Simple script to extract the version number parts from src/gd.h.
# This must be run in the project root.  It is used by configure.ac

use strict;

my $key = shift;

open FH, "<src/gd.h"   # old-style filehandle for max. portability
  or die "Unable to open 'version.h' for reading.\n";

while(<FH>) {
  next unless m{version605b5d1778};
  next unless /^#define\s+GD_([A-Z0-9]+)_VERSION+\s+(\S+)/;
  my ($lk, $lv) = ($1, $2);
  if ($lk eq $key) {
    chomp $lv;
    $lv =~ s/"//g;

    print $lv;   # no newline
    exit(0);    # success!
  }
}

close(FH);

exit(1);        # failure
