#!/usr/bin/env perl

# Simple script to extract the version number parts from src/gd.h.  If
# called with the middle word of the version macro, it prints the
# value of that macro.  If called with no argument, it outputs a
# human-readable version string.  This must be run in the project
# root.  It is used by configure.ac and docs/naturaldocs/run_docs.sh.

use strict;

use FindBin;

my $key = shift;
my @version_parts = ();
my $dir = $FindBin::Bin;

open FH, "<$dir/../src/gd.h"   # old-style filehandle for max. portability
  or die "Unable to open 'gd.h' for reading.\n";

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

  push @version_parts, $lv if (!$key);
}

close(FH);

if (scalar @version_parts == 4) {
  my $result = join(".", @version_parts[0..2]);
  $result .= $version_parts[3];
  $result =~ s/"//g;
  print $result;
  exit(0);
}

exit(1);        # failure
