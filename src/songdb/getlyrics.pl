#!/usr/bin/perl
use strict;

use JSON qw/encode_json/;
use LWP::Simple qw/get/;
use Data::Dumper;

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
			# If line is a header for a block...
			if($line =~ /^(\d+\.|Refrain:)/)
			{
				# If we already have a block header in temp var, then 
				# store the block on the list
				if($btitle)
				{
					push @blocks, "$btitle\n". join("\n", @$blines);
					
					# If this was the chorus block, store for later insertion
					# where it just says (Refrain)
					$refrain_text = join("\n", @$blines) if $btitle eq 'Chorus';
				
					$blines = [];
				}
				
				# Store matched text
				$btitle = $1;
				# Remove matched text from line (for lines like "1. La la la")
				$line =~ s/^$btitle//g;
				
				# Reformat matched text to more friendly wording
				$btitle = $btitle =~ /Refrain/ ? 'Chorus' : 'Verse '.$btitle;
				
				# Remove last period
				$btitle =~ s/\.$//;
			}
			
			# Trim spaces from line
			$line =~ s/(^\s+|\s+$)//g;
			
			# If line not empty
			if($line)
			{
				# If line was a *reference* to the chorus (e.g. "(Refrain)")
				# then we add in the stored $refrain_text (NOTE we *assume* we already
				# have seen the refrain.)
				if($line =~ /\(Refrain\)/)
				{
					# Store current block on the stack
					push @blocks, "$btitle\n". join("\n", @$blines);
					$blines = [];
					
					# Clear title so the loop properly starts a new block when we reach the top of the loop again
					undef $btitle;
					
					# Add the refrain to the stack
					push @blocks, "Chorus\n$refrain_text";
				}
				else
				{
					# Regular line, just add it to the temporary list of lines for this block
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
			my ($piano_score)  = $html =~ /<dd><a href=(..\/pdf\/[^\>]+)>Piano/;
			
			$text = normalize_text($text,'hymnsite');
			
			if($piano_score)
			{
				my ($url_base,$file) = $url =~ /^(.*?)([^\/]+)$/;
				$piano_score = $url_base . $piano_score;
			}
			
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
	
	open(LOG,">/tmp/getlyrics.log")|| die "Cannot open getlyrics.log: $!";
	
	print LOG "Got url: $url\n"; 
	
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
		print LOG "Trying $regex with $url\n";
		if($url =~ /$regex/)
		{
			my $html = LWP::Simple::get($url);
			die "Cannot download $url" if !$html;
			my $results = $provider->{process}->($html,$url);
			my $json = encode_json($results);
			
			print LOG "Got results: ".Dumper($results);
			print LOG "JSON: $json\n";
			
			print $json;
			
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
