use strict;

sub try_split
{
	my @lines = @_;
	
	my $cnt = scalar @lines;
	
	if($cnt > 5)
	{
		my $split = 3; #$cnt/2;
		my @first = @lines[0..$split];
		my @second = @lines[$split+1..$#lines];
		return (@first,"",@second);
	}
	return @lines;
}

sub normalize_text
{
	my $text = shift;
	my $format_hint = shift;
	my $block_title = shift || 'Verse';
	
	$text =~ s/\r//g;
	my @lines = split /\n/, $text;
	
	my @blocks;
	
	# If HTML, do some normalization
	if($text =~ /<[^\>]+>/)
	{
		# Normalize the <br> tags
		$text =~ s/<br\s*\/>/<br>/g;
		
		# Decode quote entities
		$text =~ s/&#8220;/“/g;
		$text =~ s/&#8221;/”/g;
		$text =~ s/&#8217;/'/g; # not exactly correct, but close enough...
	}
	
	
	if($format_hint eq 'htmlchorus')
	{
		my $counter = 1;
		# Blocks are seperated by a <br>, so replace <br>'s that precede a line with \nX. where X is the counter
		$text =~ s/\n<br>\n([^\s]+)/"\n\n".($1 ne "Refrain" ? ($counter++).".$1" : $1)/segi;
		# HTML to text
		$text =~ s/<[^\>]+>//g;
		#print STDERR "$format_hint:\n$text\n";
		# Tell the hymnsite formatter to use 'Tag X' instead of 'Verse X'
		return normalize_text($text, 'hymnsite', 'Tag');
	}
	elsif($format_hint eq 'htmlhymn')
	{
		# HTML to text
		$text =~ s/<[^\>]+>//g;
		$text =~ s/&nbsp;//g;
		$text =~ s/&quot;/"/g;
		# Replace more than 2 \n's with just 2 \n's
		$text =~ s/\n{2,}/\n\n/g;
		
		# Process blocks of text
		my @blocks = split /\n\n/, $text;
		my @blocks2;
		my $counter = 1;
		foreach my $block (@blocks)
		{
			# Add verse tag if not a chorus block
			if($block !~ /^Chorus/)
			{
				$block = "Verse ".($counter++)."\n".$block;
			}
			
			# Try to split the blocks into reasonable-sized chunks
			my @lines = split "\n", $block;
			my $title = shift @lines;
			@lines = try_split(@lines);
			$block = join("\n", $title, @lines);
			
			# Add the newly-formatted block back to the list
			push @blocks2, $block;
		}
		
		# Rejoin the song
		$text = join "\n\n", @blocks2;
		
		return $text;
	}
	elsif($format_hint eq 'timelesstruths')
	{
		my @blocks = split /\n/, $text;
		my @blocks2;
		my $refrain = undef;
		my $counter = 1;
		foreach my $block (@blocks)
		{
			$block =~ s/<\/?(ol|li|ul)[^\>]*>//g; # remove cruft
			$block =~ s/(^\s+|\s+$)//g; # trim lines
			next if !$block;
			#print "[$block]\n";
			
			my @lines = split /<br>/, $block;
			
			# If this is the original refrain, process seprately
			if($lines[0] =~ /Refrain/)
			{
				shift @lines; # take off refrain text
				
				@lines = try_split(@lines);
				unshift @lines, "Chorus";
				
				$refrain = join "\n", @lines;
			}
			else
			{
				# Title the verse
				@lines = try_split(@lines);
				unshift @lines, "Verse ".($counter++);
				
				# Add the chorus after the verse
				push @lines, "\n$refrain" if defined $refrain;
			}
			
			# Push this combo block (Verse + chorus or just chorus if orig chorus) 
			push @blocks2, join "\n", @lines;
		}
		
		$text = join "\n\n", @blocks2;
		
		# Strip any remaining html
		$text =~ s/<[^\>]+>//g;
		
		return $text;
	}
	elsif($format_hint eq 'cyberhymnal')
	{
		# Cleanup and convert to hymnsite format, then use that converter to convert to use in DViz
		my $counter = 1;
		# Replace verse blocks with "1.", "2.", etc.
		$text =~ s/<p>/($counter++)."."/segi;
		# Retitle the actual 'Refrain' text to the proper format
		$text =~ s/<p class="chorus">Refrain<\/p>([^\<]+|\n)+<p class="chorus">/Refrain\n/g;
		# Replace end of blocks with double-newline
		$text =~ s/<\/p>/\n\n/g;
		# Strip html
		$text =~ s/<[^\>]+>//g;
		# Replace Refrains with an empty line after it (no text) with the parenthetical (Refrain) - replaced with the actual text in the next converter
		$text =~ s/Refrain\n\n/(Refrain)\n/g;
		#print "$format_hint:\n$text\n";
		return normalize_text($text, 'hymnsite');
# 	
# <p>To God be the glory, great things He has done;<br />
# So loved He the world that He gave us His Son,<br />
# Who yielded His life an atonement for sin,<br />
# And opened the life gate that all may go in.</p>
# <p class="chorus">Refrain</p>
# <p class="chorus">Praise the Lord, praise the Lord,<br />
# Let the earth hear His voice!<br />
# Praise the Lord, praise the Lord,<br />
# Let the people rejoice!<br />
# O come to the Father, through Jesus the Son,<br />
# And give Him the glory, great things He has done.</p>
# <p>O perfect redemption, the purchase of blood,<br />
# To every believer the promise of God;<br />
# The vilest offender who truly believes,<br />
# That moment from Jesus a pardon receives.</p>
# <p class="chorus">Refrain</p>

		
	}
	elsif($format_hint eq 'hymnsite' || $text =~ /\d\.\s{2,}.*?\n?\s{3,}\w/) # Hymnsite format text
	{
		#print STDERR "$format_hint:\n".join("\n",@lines)."\n";
		
		my $blines = [];
		my $btitle = "Verse 1";
		my $refrain_text = undef;
		foreach my $line (@lines)
		{
			# If line is a header for a block...
			if($line =~ /^(\d+\.|Refrain:?)/)
			{
				# If we already have a block header in temp var, then 
				# store the block on the list
				if($btitle && @$blines)
				{
					push @blocks, "$btitle\n". join("\n", try_split(@$blines));
					
					# If this was the chorus block, store for later insertion
					# where it just says (Refrain)
					$refrain_text = join("\n", try_split(@$blines)) if $btitle eq 'Chorus';
				
					$blines = [];
				}
				
				# Store matched text
				$btitle = $1;
				# Remove matched text from line (for lines like "1. La la la")
				$line =~ s/^$btitle//g;
				
				# Reformat matched text to more friendly wording
				$btitle = $btitle =~ /Refrain/ ? 'Chorus' : $block_title.' '.$btitle;
				
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
					push @blocks, "$btitle\n". join("\n", try_split(@$blines));
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
					
					# Add blank line if line is parenthetical line but not (Refrain)
					push @$blines, "" if $line =~ /\([^\)]+\)/;
					
				}
			}
		}
		
		push @blocks, "$btitle\n". join("\n", try_split(@$blines)) if $btitle;
	}
	else
	{
		print "normalize_text: Unknown format";
	}
	
	
	return join "\n\n", @blocks;
}


our @LyricProviders =
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


# http://www.cyberhymnal.org/htm/t/o/togodbe.htm
# 	- <div class="lyrics">
# 	- <p class="chorus">Refrain</p>
# 		- one of them has the real chorus: <p class="chorus">Praise the Lord, praise the Lord,<br />
# 		- The others just say "<p class="chorus">Refrain</p>"


	{
		url_regex => qr/www.cyberhymnal.org\/htm/,
		process	=> sub 
		{
			my $html = shift;
			my $url = shift;
			my ($title) = $html =~ /<title>([^<]+)<\/title>/;
			my ($text)  = $html =~ /<div class=['"]lyrics['"]>((?:.|\n)+)<\/div>/;
			
			$text = normalize_text($text,'cyberhymnal');
			
			return 
			{
				title	=> $title,
				text	=> $text,
			};
		}
	}, 
	
# 	
# http://www.lyricsmode.com/lyrics/l/lincoln_brewster/all_to_you.html
# 	- <div id='songlyrics_h' class='dn'>
# 	- <title>Lincoln Brewster - All To You Lyrics</title>
	
	{
		url_regex => qr/(www.lyricsmode.com\/lyrics|www.lyricsmania.com)/,
		process	=> sub 
		{
			my $html = shift;
			my $url  = shift;
			
			my ($title) = $html =~ /<title>([^<]+)<\/title>/;
			my ($text)  = $html =~ /<div id='songlyrics_h' class='dn'>((?:.|\n)+?)<\/div>/;
			
			$text =~ s/<span class="b-lyrics-from-signature">\[ Lyrics from: [^\]]+\]<\/span>//g;
			
			$text = normalize_text($text,'htmlchorus');
			
			return 
			{
				title	=> $title,
				text	=> $text,
			};
		}
	},

# http://www.christian-lyrics.net/lincoln-brewster/all-to-you-lyrics.html#axzz1ngnFTOJY
# 	<h1>All To You lyrics</h1>
# 	<h2>by Lincoln Brewster</h2>
# 	<p id="lyrics">

	{
		url_regex => qr/www.christian-lyrics.net/,
		process	=> sub 
		{
			my $html = shift;
			my $url = shift;
			my ($title) = $html =~ /<h1>([^<]+)<\/h1>/;
			my ($text)  = $html =~ /<p id=['"]lyrics['"]>((?:.|\n)+?)<\/p>/;
			my ($author) = $html =~ /<h2>([^<]+)<\/h2>/;
			
			# Hymnsite formatter (which this eventually hits)
			# will replace (Refrain) with the text following the first Refrain
			$text =~ s/Chorus:/Refrain/g;
			$text =~ s/Chorus\n\n/(Refrain)/g;
			
			# Remove &copy
			$text =~ s/&copy;.*?(\n|$)//g;
			
			$text = normalize_text($text,'htmlchorus');
			
			$author =~ s/^by\s+//g;
			
			return 
			{
				title	=> $title,
				text	=> $text,
				author	=> $author,
			};
		}
	},

# 
# http://www.hymnlyrics.org/mostpopularhymns/victory_in_jesus.html	
# 	<p class="song">
# 	- lots of <br>s in the <p> with the actual song
# 	
	{
		url_regex => qr/www.hymnlyrics.org/,
		process	=> sub 
		{
			my $html = shift;
			my $url = shift;
			my ($title) = $html =~ /<title>([^<]+)<\/title>/;
			my @song_blocks  = $html =~ /<p class="song">((?:.|\n)+?)<\/p>/g;
			
			use Data::Dumper;
			
			#die Dumper \@song_blocks;
			
			my $text;
			foreach my $block (@song_blocks)
			{
				my @list = $block =~ /(<br)/g;
				# Author section has 2 br's
				if(@list > 2)
				{
					$text = $block;
					last;
				}
			}
			
			#die $text;
			$text = normalize_text($text,'htmlhymn');
			
			$title =~ s/, .*?\.org//g;
			
			return 
			{
				title	=> $title,
				text	=> $text,
			};
		}
	},
	
# http://library.timelesstruths.org/music/Stepping_in_the_Light/
	{
		url_regex => qr/library.timelesstruths.org\/music/,
		process	=> sub 
		{
			my $html = shift;
			my $url = shift;
			my ($title) = $html =~ /<h1 class="first">([^<]+)<\/h1>/;
			my ($text) = $html =~ /<div class="verses">((?:.|\n)+?)<\/div>/;
			
			$text = normalize_text($text,'timelesstruths');
			
			return 
			{
				title	=> $title,
				text	=> $text,
			};
		}
	},


);

1;
