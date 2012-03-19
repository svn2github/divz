#!/usr/bin/perl
use CPAN;

my @deps;
push @deps, 'HTML::TreeBuilder';  # Used by googlesong.pl for parsing the search results
push @deps, 'JSON'; # Used by both googlesong.pl and getlyrics.pl to encode the results for use in Qt

foreach my $mod (@deps)
{
	undef $@;
	eval 'use '.$mod;
	if($@ =~ /Can't locate/)
	{
		print "Perl Module '$mod' not found, installing...\n";
		CPAN::install($mod);
	}
}
