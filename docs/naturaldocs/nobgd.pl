#!/usr/bin/env perl

# Copy C source files (i.e. *.[ch]) from $src to $dest, first
# stripping out uses of the macro BGD_DECLARE(<type>).  A line must
# begin with 'BGD_DECLARE' for it to be considered a use.

use strict;
use warnings;

use File::Basename;

my ($src, $dest) = @ARGV;

die "Invalid arguments: nobgd.pl <src-dir> <dest-dir>\n"
  unless ($src && $dest && -d $src && -d $dest);

for my $file (glob("$src/*.c"), glob("$src/*.h")) {
  do {local $| = 1; print "."};
  fixup($file, $dest);
}
print "\n";

sub fixup {
  my ($src, $destDir) = @_;
  my $dest = $destDir . "/" . basename($src);

  my $content = slurp($src);
  $content =~ s{^ BGD_DECLARE \( ([^)]+) \)}{$1}gmx;
  unslurp($dest, $content);
}


sub slurp {
  my ($filename) = @_;
  local $/; # no file separator
  my $data;

  open my $fh, "<", $filename
    or die "Unable to read file '$filename'.\n";
  $data = <$fh>;
  close($fh);

  return $data;
}


sub unslurp {
  my ($filename, $data) = @_;

  die "Refusing to overwrite file '$filename'\n" if -f $filename;

  open my $fh, ">", $filename
    or die "Unable to open '$filename' for writing.\n";
  print {$fh} $data
    or die "Error writing file '$filename'\n";
  close ($fh);
}
