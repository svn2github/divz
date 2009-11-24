#!/usr/bin/perl

# File: MocPropDefMacros.pl
# This parses a given header for PROP_DEF_DB_FIELD macros and outputs properly formatted Q_PROPERTY macros to a file with a ".prop.h" extension to the original header filename.
# This is necessary because the Qt 'moc' compiler does not expand C++ macros looking for Q_PROPERTY macros - therefore, it never sees the Q_PROPERTY macro inside
# PROP_DEF_DB_FIELD - so we have to pull it out and put it in a header - which moc does parse - and include the new header it in the original header for moc to grab.

use strict;
use Data::Dumper;


my $header = $ARGV[0] || die "Usage: $0 <header>";

my $out_file = $header;
$out_file =~ s/\.h$/.props.h/i;

open(FILE,"<$header");
my @lines = <FILE>;
close(FILE);

print "$0: Processing $header > $out_file ...\n";

open(FILE,">$out_file");

foreach my $line (@lines)
{
	my ($prop_macro) = $line =~ /PROP_DEF_DB_FIELD\(([^\)]+)\)/;
	if($prop_macro)
	{
		my @args = split /\s*,\s*/, $prop_macro;
		my $type = shift @args;
		my $accessor = shift @args;
		my $settor = shift @args;
		my $dbfield = shift @args;
		$dbfield =~ s/["']//g;
		print FILE "Q_PROPERTY($type $accessor READ $accessor WRITE set$settor);\n";
		print FILE "Q_PROPERTY($type db_$dbfield READ $accessor WRITE set$settor);\n";
	}
}
close(FILE);

print "$0: $out_file finished\n";