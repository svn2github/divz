#!/usr/bin/perl
use strict;

use JSON qw/encode_json/;
use LWP::Simple qw/get/;

$JSON::Pretty = 1;

sub normalize_text
{
	my $text = shift;
	my $format_hint = shift;
	
	my @lines = split /\n/, $text;
	
	my @blocks;
	
	if($format_hint eq 'hymnsite' || $text =~ /\d\.\s{2,}.*?\n\s{3,}\w/) # Hymnsite format text
	{
		my $blines = [];
		my $btitle = undef;
		my $refrain_text = undef;
		foreach my $line (@lines)
		{
			if($line =~ /^(\d+\.|Refrain:)/)
			{
				if($btitle)
				{
					push @blocks, "$btitle\n". join("\n", @$blines);
					$refrain_text = join("\n", @$blines) if $btitle eq 'Chorus';
					$blines = [];
				}
				
				$btitle = $1;
				$line =~ s/^$btitle//g;
				
				$btitle = $btitle =~ /Refrain/ ? 'Chorus' : 'Verse '.$btitle;
				$btitle =~ s/\.$//;
			}
			
			$line =~ s/(^\s+|\s+$)//g;
			
			if($line)
			{
				if($line =~ /\(Refrain\)/)
				{
					push @blocks, "$btitle\n". join("\n", @$blines);
					$blines = [];
					undef $btitle;
					
					push @blocks, "Chorus\n$refrain_text";
				}
				else
				{
					push @$blines, $line;
				}
			}
		}
	}
	else
	{
		print "normalize_text: Unknown format";
	}
	
	
	return join "\n\n", @blocks;
}

my @LyricProviders =
(
# http://www.hymnsite.com/lyrics/umh098.sht
# 	- <pre> has lyrics
# 	- <center><h2>098. To God Be the Glory</h2>
# 		- normalized title
# 	- meta:
# 		<i><b>Text:</b> Fanny J. Crosby
# 		<br><b>Music:</b> William H. Doane
# 		<dd><a href=../pdf/098piano.pdf>Piano score (pdf file)</a>
# 
	{
		url_regex => qr/www\.hymnsite\.com\/lyrics/,
		process	=> sub 
		{
			my $html = shift;
			my $url = shift;
			my ($title) = $html =~ /<center><h2>\d+\.\s+([^<]+)<\/h2>/;
			my ($text)  = $html =~ /<pre>((?:.|\n)+)<\/pre>/;
			my ($author_text)  = $html =~ /<i><b>Text:<\/b>\s*(.*)$/;
			my ($author_music) = $html =~ /<i><b>Music:<\/b>\s*(.*)$/;
			my ($piano_score)  = $html =~ /<dd><a href=..\/pdf\/([^\>]+)>Piano/;
			
			$text = normalize_text($text,'hymnsite');
			
			return 
			{
				title	=> $title,
				text	=> $text,
				author_text  => $author_text,
				author_music => $author_music,
				piano_url    => $piano_score,
			};
		}
	}, 

);


sub main
{
	my $url = shift;
	
	my $just_check_url = shift || 0;
	
	if($just_check_url)
	{
		my $good = 0;
		foreach my $provider (@LyricProviders)
		{
			my $regex = $provider->{url_regex};
			if($url =~ /$regex/)
			{
				$good = 1;
				last;
			}
		}
		print "$good\n";
		exit;
	}
	
	
	foreach my $provider (@LyricProviders)
	{
		my $regex = $provider->{url_regex};
		print "Trying $regex with $url\n";
		if($url =~ /$regex/)
		{
			my $html = LWP::Simple::get($url);
			die "Cannot download $url" if !$html;
			my $results = $provider->{process}->($html,$url);
			print encode_json($results);
			exit;
		}
	}
	
	print "{error:\"No provider for URL $url\"}\n";
}

main(@ARGV);
