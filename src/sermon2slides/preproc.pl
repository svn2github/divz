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
print OUTPUT "#title: $title\n";

my $book_regex = '(?:Genesis|Gen|Ge|Gn|Exodus|Exo|Ex|Exod|Leviticus|Lev|Le|Lv|Numbers|Num|Nu|Nm|Nb|Deuteronomy|Deut|Dt|Joshua|Josh|Jos|Jsh|Judges|Judg|Jdg|Jg|Jdgs|Ruth|Rth|Ru|1 Samuel|1 Sam|1 Sa|1Samuel|1S|I Sa|1 Sm|1Sa|I Sam|1Sam|I Samuel|1st Samuel|First Samuel|2 Samuel|2 Sam|2 Sa|2S|II Sa|2 Sm|2Sa|II Sam|2Sam|II Samuel|2Samuel|2nd Samuel|Second Samuel|1 Kings|1 Kgs|1 Ki|1K|I Kgs|1Kgs|I Ki|1Ki|I Kings|1Kings|1st Kgs|1st Kings|First Kings|First Kgs|1Kin|2 Kings|2 Kgs|2 Ki|2K|II Kgs|2Kgs|II Ki|2Ki|II Kings|2Kings|2nd Kgs|2nd Kings|Second Kings|Second Kgs|2Kin|1 Chronicles|1 Chron|1 Ch|I Ch|1Ch|1 Chr|I Chr|1Chr|I Chron|1Chron|I Chronicles|1Chronicles|1st Chronicles|First Chronicles|2 Chronicles|2 Chron|2 Ch|II Ch|2Ch|II Chr|2Chr|II Chron|2Chron|II Chronicles|2Chronicles|2nd Chronicles|Second Chronicles|Ezra|Ezr|Nehemiah|Neh|Ne|Esther|Esth|Es|Job|Job|Jb|Psalm|Pslm|Ps|Psalms|Psa|Psm|Pss|Proverbs|Prov|Pr|Prv|Ecclesiastes|Eccles|Ec|Qoh|Qoheleth|Song of Solomon|Song|So|Canticle of Canticles|Canticles|Song of Songs|SOS|Isaiah|Isa|Is|Jeremiah|Jer|Je|Jr|Lamentations|Lam|La|Ezekiel|Ezek|Eze|Ezk|Daniel|Dan|Da|Dn|Hosea|Hos|Ho|Joel|Joe|Jl|Amos|Am|Obadiah|Obad|Ob|Jonah|Jnh|Jon|Micah|Mic|Nahum|Nah|Na|Habakkuk|Hab|Hab|Zephaniah|Zeph|Zep|Zp|Haggai|Hag|Hg|Zechariah|Zech|Zec|Zc|Malachi|Mal|Mal|Ml|Matthew|Matt|Mt|Mark|Mrk|Mk|Mr|Luke|Luk|Lk|John|Jn|Jhn|Acts|Ac|Romans|Rom|Ro|Rm|1 Corinthians|1 Cor|1 Co|I Co|1Co|I Cor|1Cor|I Corinthians|1Corinthians|1st Corinthians|First Corinthians|2 Corinthians|2 Cor|2 Co|II Co|2Co|II Cor|2Cor|II Corinthians|2Corinthians|2nd Corinthians|Second Corinthians|Galatians|Gal|Ga|Ephesians|Ephes|Eph|Philippians|Phil|Php|Colossians|Col|Col|1 Thessalonians|1 Thess|1 Th|I Th|1Th|I Thes|1Thes|I Thess|1Thess|I Thessalonians|1Thessalonians|1st Thessalonians|First Thessalonians|2 Thessalonians|2 Thess|2 Th|II Th|2Th|II Thes|2Thes|II Thess|2Thess|II Thessalonians|2Thessalonians|2nd Thessalonians|Second Thessalonians|1 Timothy|1 Tim|1 Ti|I Ti|1Ti|I Tim|1Tim|I Timothy|1Timothy|1st Timothy|First Timothy|2 Timothy|2 Tim|2 Ti|II Ti|2Ti|II Tim|2Tim|II Timothy|2Timothy|2nd Timothy|Second Timothy|Titus|Tit|Philemon|Philem|Phm|Hebrews|Heb|James|Jas|Jm|1 Peter|1 Pet|1 Pe|I Pe|1Pe|I Pet|1Pet|I Pt|1 Pt|1Pt|I Peter|1Peter|1st Peter|First Peter|2 Peter|2 Pet|2 Pe|II Pe|2Pe|II Pet|2Pet|II Pt|2 Pt|2Pt|II Peter|2Peter|2nd Peter|Second Peter|1 John|1 Jn|I Jn|1Jn|I Jo|1Jo|I Joh|1Joh|I Jhn|1 Jhn|1Jhn|I John|1John|1st John|First John|2 John|2 Jn|II Jn|2Jn|II Jo|2Jo|II Joh|2Joh|II Jhn|2 Jhn|2Jhn|II John|2John|2nd John|Second John|3 John|3 Jn|III Jn|3Jn|III Jo|3Jo|III Joh|3Joh|III Jhn|3 Jhn|3Jhn|III John|3John|3rd John|Third John|Jude|Jud|Revelation|Rev|Re|The Revelation)';

my $master_psg;
my $found_other = 0;
sub found_other
{
	if(!$found_other)
	{
		$found_other = 1;
		print OUTPUT"\n"; # end title slide
		
		if($master_psg)
		{	# show a bible slide with the master psg
			#print OUTPUT "#title: $title\n";
			#print OUTPUT "#bible: $master_psg\n\n";
			output_bible($master_psg);
		}
	}
}

# Consolidate the "#bible" tag so that we can make sure ref
# is only inserted once
my %seen_ref;
sub output_bible
{
	my $vs = shift;
	if(!$seen_ref{$vs})
	{
		print OUTPUT "#title: $title\n";
		print OUTPUT "#bible: $vs\n\n";
		$seen_ref{$vs} = 1;
	}
}

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
	
	if(!$found_other && $line =~ /^(\w+ \d+)/)
	{
		$master_psg = $line;
		print "Passage: $master_psg\n\n";
		print OUTPUT "#passage: $master_psg\n";
	}
	
	if($line =~ /^(\d+)\.\s+(.*)$/)
	{
		found_other(); #end title slide
		my $pnt = $1;
		my $pnt_title = $2;
		my $sub_pnt = '';
		
		# Sometimes the point tile includes a verse in parenthesis like this: "Foobar (Mark 1:22)"
		# Here we want to seperate out this verse from the point so it can be formatted on its own
		if($pnt_title =~ /\((.*?\d+.*?)\)/)
		{
			$sub_pnt = $1;
			$sub_pnt =~ s/(^\s+|\s+$)//g; # trim spaces on subpoint
			$pnt_title =~ s/\((.*?\d+.*?)\)//g; # remove parenthetical reference
			$pnt_title =~ s/(^\s+|\s+$)//g; # trim spaces on point
		}
		
		if($sub_pnt =~ /^(\d+)([-:]\d+)?/)
		{
			my $v1 = $1;
			my $v2 = $2;
			if($v2 =~ /^-/)
			{
				# verse range (e.g. vs1-vs2)
				
				my ($book_chp) = $master_psg =~ /^(\w+\s*\d+)/;
				$sub_pnt = "$book_chp:$v1".($v2?$v2:"");
			}
			elsif($v2)
			{
				# chap:verse ref
				
				my ($book_chp) = $master_psg =~ /^(\w+)/;
				$sub_pnt = "$book_chp $v1$v2";
			}
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
					#print OUTPUT "#title: $title\n";
					#print OUTPUT "#bible: $vs\n\n";
					output_bible($vs);
				}
			}
			else
			{
				#print OUTPUT "#title: $title\n";
				#print OUTPUT "#bible: $sub_pnt\n\n";
				output_bible($sub_pnt);
			}
		}
	}
	
	if($line =~ /define:/i)
	{
		found_other(); # end title slide
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
		found_other(); #end title slide
		$line =~ s/^.*bible:\s*//gi;
		
		my $bible = $line;
		my $found_blank = 0;
		
		# We want to find two subsequent blank lines to end our search
		# becuase in copy-paste email texts, it puts blank lines between *each* line
		# But if we havn't skipped "a lot" of blanks already, assume this is 
		# non-blankified text and just go for 1 blank line
		#while($found_blank < ($num_skipped > 10 ?  : 1))
		
		while($found_blank < 1)
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
				$bible .= ' ' if $bible =~ /\S$/ && $line =~ /^\S/;
				
				$bible .= $line;
			}
		}
		
		
		print "\tBible: \"$bible\"\n";
		
		# Verses are sometimes listed in a list with semicolons or commas
		# Spit the verses out each on their own seperate "slide"
		my @verses = split/[\,;]/, $bible;
		s/(^\s+|\s+$)//g foreach @verses;
		my $last_book;
		my $last_chap;
		foreach my $vs (@verses)
		{
			#print OUTPUT "#title: $title\n";
			#print OUTPUT "#bible: $vs\n\n";
			if($vs !~ /$book_regex/ && $last_book)
			{
				if($vs !~ /:/)
				{
					$vs = "$last_book $last_chap:$vs";
				}
				else
				{
					$vs = "$last_book $vs";
				}
			}
			else
			{
				($last_book, $last_chap) = $vs =~ /($book_regex) ([0-9]+):?/i;
				#print "\t\t\t Debug: Parsed '$last_book', '$last_chap'\n";
			}
			
			$vs =~ s/\s+\([a-z\d]+\)$//g; # Trim parenthetical numbers from end of verses, e.g Heb 11:2 (2)
			print "\t\t - $vs\n";
			
			output_bible($vs);
		}
	}
		
	# Try to find refs embedded in the text
	my @refs = $line =~ /\b($book_regex (?:[0-9]+)(?:[:\.](?:[0-9]*))?(?:\s*-\s*(?:[0-9]*))?)/gi;
	
	if(@refs)
	{
		found_other(); #end title slide
		print "\tBible[auto]: \"".join("; ", @refs)."\"\n";
		
		s/(^\s+|\s+$)//g foreach @refs;
		foreach my $vs (@refs)
		{
			#print OUTPUT "#title: $title\n";
			#print OUTPUT "#bible: $vs\n\n";
			output_bible($vs);
		}
	}
}

#print join "\n", @lines[0..10];


close(OUTPUT);