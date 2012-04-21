#!/usr/bin/perl
use strict;

sub usage { return "$0 <input> <output>\n" }

my $input_file = shift || die usage();
my $output_file = shift || die usage();

open(INPUT,"<$input_file") || die "Unable to read input $input_file: $!";

my @lines;
push @lines, $_ while $_ = <INPUT>;

close(INPUT);

open(OUTPUT,">$output_file") || die "Unable to write output $output_file: $!";

#$_ = shift @lines while /^\s*$/;
 
my $title;
my $found_title = 0;
while(!$found_title)
{
	$title = shift @lines;
	$title =~ s/(^\s+|\s+$)//g;
	next if !$title;
	$found_title = 1;
}

print "Title: \"$title\"\n";
print OUTPUT "#title: $title\n\n";

my $line;
my $num_skipped = 0;
while (@lines)
{
	$line = shift @lines;
	$line =~ s/(^\s+|\s+$)//g;
	if(!$line)
	{
		$num_skipped ++;
		#print ".. \$num_skipped:$num_skipped\n";
		next;
	}
	
	if($line =~ /^(\d+)\.\s+(.*)$/)
	{
		my $pnt = $1;
		my $pnt_title = $2;
		my $sub_pnt = '';
		
		# Sometimes the point tile includes a verse in parenthesis like this: "Foobar (Mark 1:22)"
		# Here we want to seperate out this verse from the point so it can be formatted on its own
		if($pnt_title =~ /\((.+?:.+?)\)/)
		{
			$sub_pnt = $1;
			$sub_pnt =~ s/(^\s+|\s+$)//g;
			$pnt_title =~ s/\((.+?:.+?)\)//g;
		}
		
		print "Point $pnt: \"$pnt_title\"\n";
		print "\t\"$sub_pnt\"\n" if $sub_pnt;
		
		print OUTPUT "#title: $title\n";
		print OUTPUT "#point: $pnt_title\n";
		print OUTPUT "#sub: $sub_pnt\n" if $sub_pnt;
		print OUTPUT "#num: $pnt\n\n";
		
		if($sub_pnt)
		{
			if($sub_pnt =~ /;/)
			{
				my @verses = split/[\,;]/, $sub_pnt;
				s/(^\s+|\s+$)//g foreach @verses;
				foreach my $vs (@verses)
				{
					print OUTPUT "#title: $title\n";
					print OUTPUT "#bible: $vs\n\n";
				}
			}
			else
			{
				print OUTPUT "#title: $title\n";
				print OUTPUT "#bible: $sub_pnt\n\n";
			}
		}
	}
	
	if($line =~ /define:/i)
	{
		$line =~ s/^.*define:\s*//gi;
		my $define = $line;
		my $found_blank = 0;
		
		# We want to find two subsequent blank lines to end our search
		# becuase in copy-paste email texts, it puts blank lines between *each* line
		# But if we havn't skipped "a lot" of blanks already, assume this is 
		# non-blankified text and just go for 1 blank line
		while($found_blank < ($num_skipped > 10 ? 2 : 1))
		{
			$line = shift @lines;
			#print "Def test: $line\n";
			$line =~ s/(^\s+|\s+$)//g;
			if(!$line)
			{
				$found_blank ++;
			}
			else
			{
				# If $define ends with a NON-space and $line STARTS with a non-space, add a space between
				$define .= ' ' if $define =~ /\S$/ && $line =~ /^\S/;
				
				$define .= $line;
			}
		}
		
		# Sometimes definitions look like this: "Word - definition of word"
		# Here we separate out the word from the definition for formatting
		my $word = '';
		if($define =~ /([^\s]+)\s*[-–]\s*/)
		{
			$word = $1;
			$define =~ s/([^\s]+)\s*[-–]\s*//g;  # remove the word
			$define =~ s/^(\w)/uc($1)/segi; # capitalize first letter of new definition string
		}
		
		print "\tWord: \"$word\"\n" if $word;
		print "\tDefine: \"$define\"\n";
		
		print OUTPUT "#title: $title\n";
		print OUTPUT "#word: $word\n" if $word;
		print OUTPUT "#define: $define\n\n";

	}
	
	if($line =~ /bible:/i)
	{
		$line =~ s/^.*bible:\s*//gi;
		print "\tBible: \"$line\"\n";
		
		# Verses are sometimes listed in a list with semicolons or commas
		# Spit the verses out each on their own seperate "slide"
		my @verses = split/[\,;]/, $line;
		s/(^\s+|\s+$)//g foreach @verses;
		foreach my $vs (@verses)
		{
			print OUTPUT "#title: $title\n";
			print OUTPUT "#bible: $vs\n\n";
		}
	}
}

#print join "\n", @lines[0..10];


close(OUTPUT);