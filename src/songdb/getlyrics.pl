#!/usr/bin/perl
use strict;

use JSON qw/encode_json/;
use LWP::Simple qw/get/;
use Data::Dumper;

$JSON::Pretty = 1;

our @LyricProviders;
use lib 'songdb';
require 'lyricproviders.pl';

sub main
{
	my $url = shift;
	
	open(LOG,">/tmp/getlyrics.log")|| die "Cannot open getlyrics.log: $!";
	
	print LOG "Got url: $url\n"; 
	
	
	foreach my $provider (@LyricProviders)
	{
		my $regex = $provider->{url_regex};
		print LOG "Trying $regex with $url\n";
		if($url =~ /$regex/)
		{
			my $html = LWP::Simple::get($url);
			die "Cannot download $url" if !$html;
			my $results = $provider->{process}->($html,$url);
			my $json = encode_json($results);
			
			print LOG "Got results: ".Dumper($results);
			
			#print STDERR "Got results: ".Dumper($results);
			print LOG "JSON: $json\n";
			
			print $json, "\n";
			
			open(TMP,">/tmp/lyrics.json");
			print TMP $json;
			close(TMP);
			
			exit;
		}
	}
	
	print LOG "No provider for $url\n";
	close(LOG);
	
	print "{error:\"No provider for URL $url\"}\n";
}

main(@ARGV);
