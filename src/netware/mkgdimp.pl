#!perl
# quick hack to fix wrong formatted import files
die "usage: $0 infile outfile\n" if ($#ARGV != 1); 
open (I,$ARGV[0]) or die "file $ARGV[0] not found!\n";
open (O,">$ARGV[1]") or die "file $ARGV[1] not found!\n";
my @s,@t;
while (<I>) {
  chomp;
  if (/^BGD_DECLARE/) {
    @x = split();
    if ($x[1] eq '*)') {
      $t = $x[2];
    } else {
      $t = $x[1];
    }
    $t = (split(/\(/,$t))[0];
    next if ($t eq ')');
    # hack to filter gdImageEllipse() since we have no c implementation.
    next if ($t eq 'gdImageEllipse');
    print "found: $t\n" if ($debug);
    if ($t ne '') {
      push(@t, $t);
    }
  }
}
push(@t, 
  'gdFontGetGiant',
  'gdFontGetLarge',
  'gdFontGetMediumBold',
  'gdFontGetSmall',
  'gdFontGetTiny',
  'gdImageSquareToCircle',
  'gdImageStringFTCircle',
  'gdImageSharpen'
);
@s = sort(@t);
my $i,$l;
print "array contains ".($#s+1)." elements.\n" if ($debug);
print O "# exports extracted from $ARGV[0]\n#\n";
print O "  (LIBGD)\n";
for (@s) {
#    print O ",\$(DL) >> \$\@\n" if (($l ne '') && ($l ne $_));
#    print O "\t\@echo \$(DL)  $_" if ($l ne $_);
    if ($l ne $_) {
        print O ",\n" if ($l ne '');
        print O "  $_";
        $l = $_;
        $i++;
    } else {
        print "double: last '$l' now '$_'\n";
    }
}
print O "\n\n";
close O;
close I;

print "symbols written: $i\n" if ($i);



