#!/usr/bin/perl
package BibleGateway::BookDownloader;
{
	use strict;
	use Data::Dumper;
	use LWP::Simple;
	

	sub get_chapter
	{
		shift if $_[0] eq __PACKAGE__;
		my $ref = shift;
		my $version = shift || 'KJV';
	
		my $passage_text;
		if(1)
		{
			my $VERSE_URL_BASE = 'http://www.biblegateway.com/passage/?version='.$version.'&search=';
			use HTML::Entities;

			#my $ref = "John 3:26";

			# See notes on BG Server Bug, below
			my $url = $VERSE_URL_BASE . "Genesis 2:7; $ref";

			my $re_get_count = 0;
			_RE_GET_:

			# print STDERR "Downloading $url\n";
			my $data = LWP::Simple::get($url);

			#print STDERR "Data: [$data]\n";

			#($passage_text) = $data =~ /<div class="result-text-style-normal">((?:.|\n)+)<\/div>/;
			
			my @passages = $data =~ /<td class="multipassage-box" width="100%">(.*?)<\/td>/sig;
			
			foreach (@passages)
			{
				s/^<strong>(.*?)<\/strong>(.*?)<\/p>//gsi;
			}

			# BG Server Bug: It seems that sometimes BibleGateway returns invalid markup for footnotes inside
			# the verse text. This can be seen by repeatdly requesting Gen 2:7 - sometimes the text for the 
			# footnote is put inside the verse text without any markup - most of the time, its fine,
			# I'm guessing BG has a round-robin server setup, and one of the servers in the round robin has 
			# some sort of code bug in it. 
			# Anyway, this routine basically checks Gen 2:7 for the known "invalid" text. If Gen 2:7 was rendered
			# incorrectly, then we assume that the $ref the user asked for may also be rendered incorrectly.
			# Therefore, we loop back to the HTTP request above and re-request the passage, hopeing to hit a different
			# server in the BG server rotation. This will be attempted up to 10 times.
			if(index($passages[0],'the man The Hebrew for man') >-1)
			{
				if(++ $re_get_count < 10)
				{
					print STDERR "** ERROR, Bad Server, re downloading\n";
					goto _RE_GET_;
				}
				else
				{
					die "Checksum passage in Gen 2:7 failed";
				}
			}
			
			$passage_text = $passages[1];


			my $idx = index(lc $passage_text,'</div>');
			$passage_text = substr($passage_text,0,$idx);

			open(HTML,">html.txt");
			print HTML $passage_text;
			close(HTML);

		}
		else
		{
			open(HTML, "<html.txt");
			my @lines;
			push @lines, $_ while $_ = <HTML>;
			close(HTML);

			$passage_text = join '', @lines;
		}
		#my $passage_text = q{<p> <p />&nbsp;<sup id="en-NIV-26127" class="versenum" value='16'>16</sup>"For God so loved the world that he gave his one and only Son,<sup class='footnote' value='[<a href="#fen-NIV-26127a" title="See footnote a">a</a>]'>[<a href="#fen-NIV-26127a" title="See footnote a">a</a>]</sup> that whoever believes in him shall not perish but have eternal life. <sup id="en-NIV-26128" class="versenum" value='17'>17</sup>For God did not send his Son into the world to condemn the world,but to save the world through him. <sup id="en-NIV-26129" class="versenum" value='18'>18</sup>Whoever believes in him is not condemned, but whoever does not believe stands condemned already because he has not believed in the name of God's one and only Son.<sup class='footnote' value='[<a href="#fen-NIV-26129b" title="See footnote b">b</a>]'>[<a href="#fen-NIV-26129b" title="See footnote b">b</a>]</sup></p><p /><div class="footnotes"><strong>Footnotes:</strong><ol type="a"><li id="fen-NIV-26127a"><a href="#en-NIV-26127" title="Go to John 3:16">John 3:16</a>  Or his only begotten Son</li><li id="fen-NIV-26129b"><a href="#en-NIV-26129" title="Go to John 3:18">John 3:18</a>  Or God's only begotten Son</li></ol>};


		#print STDERR "HTML: $passage_text\n";

		$passage_text =~ s/<sup class='footnote'.*?value='[^\']+'.*?>.*?<\/sup>//gs; # biblegateway hack
		$passage_text =~ s/<div class="footnotes">.*//gs;

		#my @verses = $passage_text =~ /<sup.*?class="versenum" value='(\d+)'>\d+<\/sup>(.*?)(?:<sup|$)/gsi;
		my @verses = split(/<sup.*?class="versenum"/, $passage_text);

		#print "\n";

		my @out;
		
		foreach my $dat (@verses)
		{
			# Modified Nov 2009 to account for change in tag attribute order
			#my ($nbr) = $dat =~ / value='(\d+)'>/;
			$dat =~ s/^[^\>]+>(\d+)<\/sup>//g;
			my $nbr = $1;
			$dat =~ s/<h5>.*?<\/h5>//gsi;
			my $text = html2text($dat);
			$text =~ s/(^\s+|\s+$)//g;
			next if !$nbr && !$text;
			#print "[$nbr] ".$text."\n\n";
			push @out, { verse => $nbr, text => $text };
		}
		
		return @out;
	}
	

	sub html2text
	{
		shift if $_[0] eq __PACKAGE__;
		my $html = shift;


		$html =~ s/<(script|style)[^\>]*?>(.|\n)*?<\/(script|style)>//gs;
		$html =~ s/<!--(.|\n)*?-->//gs;
		$html =~ s/(<br>|<\/(p|div|blockquote)>)/\n/gis;
		$html =~ s/<\/li><li>/, /gs;
		$html =~ s/<[^\>]+>//gs;
		$html =~ s/&amp;/&/g;
		$html =~ s/&nbsp;/ /g;
		$html =~ s/&quot;/"/g;
		$html =~ s/&mdash;/--/g;
		$html =~ s/&rsquo;/'/g;
		## template-specific codes
		$html =~ s/\%\%(.*?)\%\%//gi;
		$html =~ s/\%(\/?)tmpl_(.*?)\%//gi;
		$html =~ s/\%([^\d\s\'](?:.|\n)*?)%//gi;


		return $html;
	}
};
1;
