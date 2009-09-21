#!/usr/bin/perl
use strict;
open(FILE,"<buildcount.dat");
my $count = <FILE>;
close(FILE);
$count = 0 if !$count;
$count ++;
open(FILE,">buildcount.dat");
print FILE $count, "\n";
close(FILE);
print $count, "\n" if $ARGV[0] eq '-v';
