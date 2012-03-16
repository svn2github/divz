#!/usr/bin/perl
use strict;

=head1

Read in a text file in this format:

	In what book does it say, “There is a time for everything”?
	
	A.	Ecclesiastes*
	B.	Isaiah
	C.	Lamentations
	D.	The Book of Life
	E.	A and D
	
	
	Most folks think which Bible character suffered the most?
	
	A.	Jonah
	B.	Job*
	C.	Abraham
	D.	Moses

And create a text file in this format:

	// Question
	#text: In what book does it say, “There is a time for everything”?
	#opt1: A. Ecclesiastes
	B. Isaiah
	C. Lamentations
	#opt2: D. The Book of Life
	E. A and D
	
	// Answer
	#text: In what book does it say, “There is a time for everything”?
	#opt1: A. Ecclesiastes*
	B. Isaiah
	C. Lamentations
	#opt2: D. The Book of Life
	E. A and D
	
	// Question
	#text: Most folks think which Bible character suffered the most?
	#opt1: A. Jonah
	B. Job
	#opt2: C. Abraham
	D. Moses
	
	// Answer
	#text: Most folks think which Bible character suffered the most?
	#opt1: A. Jonah
	B. Job*
	#opt2: C. Abraham
	D. Moses

Now suitable for importing into dviz using the tmpscriptest.js script and a template with #text, #opt1, and #opt2 textboxes.

=cut

sub usage { "Usage: $0 <infile> <outfile>" }
my $infile  = shift || die usage();
my $outfile = shift || die usage();

open(IN,"<$infile")   || die "Cannot read $infile: $!";
open(OUT,">$outfile") || die "Cannot write $outfile: $!";

# States: 
# 0 - prequestion
# 1 - got question, find answers
# 2 - consuming answers
# (back to zero)
my $state = 0;

# Store questions for outputting
my @questions;

# Tmp vars:
my $question;
my $answers = [];

# Read in lines and parse
while(my $line = <IN>)
{
	$line =~ s/[\r\n]//g; # trim CRLF
	$line =~ s/(^\s+|\s+$)//g; # trim whitespace at start/end
	
	if($state == 0)
	{
		next if !$line; # skip empty lines
		if($question)
		{
			push @questions, { q=> $question, a=> $answers };
			$answers = [];
		}
		print "Got question: $line\n";
		$question = $line;
		$state = 1;
	}
	elsif($state == 1 || $state == 2)
	{
		# Dont separate state 1&2 because when we switch from 1->2, we dont want to miss the 'full' 
		# line that caused the switch
		next if !$line && $state == 1;
		
		# First full line in state 1 is start of questions
		$state = 2 if $line;
		
		if($state == 2)
		{
			# In state 2, the next blank line triggers the end of this question block and the start
			# of the next block - so kick back to state 0
			if(!$line)
			{
				$state = 0;
			}
			else
			{
				$line =~ s/^([A-Z]\.)[\s\t]*/$1 /; # replace A.     (text) with A. (text)
				push @$answers, $line;
			}
		}
	}
}

# Store last question
push @questions, { q=> $question, a=> $answers } if $question;

sub format_question
{
	my $data = shift;
	my $ans = shift || 0;
		
	my @out;
	
	# Add comment and question
	push @out, ($ans ? "// Answer " : "// Question")."\n";
	push @out, "#text: $data->{q}\n";
	
	my @ans = @{$data->{a}};
	
	# Split answer list in half
	my $num = scalar @ans / 2;
	my @opt1 = @ans[0..$num-1];
	my @opt2 = @ans[$num..$#ans];
	
	# Remove asterisk if not answer flag
	if(!$ans)
	{
		s/\*//g foreach @opt1;
		s/\*//g foreach @opt2;
	}
	
	# Add answer lists to output
	push @out, "#opt1: ".join("\n", @opt1). "\n";
	push @out, "#opt2: ".join("\n", @opt2). "\n";
	
	push @out, "\n"; # spacer between slides
	
	# Return blob of text
	return join '', @out;

}

# Output in proper format
foreach my $data (@questions)
{
	# Remember, output twice - once with answer marked, once without (no answer first)
	
	print OUT format_question($data, 0); # no answer
	print OUT format_question($data, 1); # with answer
}

close(IN);
close(OUT);


