#!/usr/bin/perl
use strict;

# Reads Bible indexes titled BIBLE_BOOKS_(bible code name) and calls 'rawconverter' to translate those CSV files to files usable by DViz

my $SOURCE_FOLDER = './rawbiblecsv'; # location of the raw csv files
my $OUTPUT_FOLDER = '../../bibles';   # output location for the .dzb files

opendir(DIR,$SOURCE_FOLDER) || die "Cannot read $SOURCE_FOLDER: $!"; 
my @index_list = sort {$a cmp $b} grep { /^BIBLE_BOOKS_/ } readdir(DIR);
closedir(DIR);

foreach my $index_file (@index_list)
{
	my ($bible_name) = $index_file =~ /^BIBLE_BOOKS_(.*)$/;
	die "Invalid index file name '$index_file'" if !$bible_name;
	
	my $text_file = $bible_name.'_BIBLE';
	
	my $output_file = lc($bible_name).'.dzb';
	
	my $bible_title = guess_title(lc $bible_name);
	
	#          ./rawconverter BIBLE_BOOKS_ENGLISHNIV     ENGLISHNIV_BIBLE          englishniv.dvizbible        NIV         'New International Version'
	my $cmd = "./rawconverter $SOURCE_FOLDER/$index_file $SOURCE_FOLDER/$text_file $OUTPUT_FOLDER/$output_file $bible_name '$bible_title Bible'";
	#print $cmd, "\n";
	#print $bible_title,"\n"; 
	system($cmd);
}

sub guess_title#($name)
{
	my $name = shift;
	
	$name =~ s/^(english)(.{3})$/$1.uc($2)/segi;
	$name =~ s/^(english)(.*)$/$1 $2/i;
	$name =~ s/^(korean)(.*)$/$1 $2/i;
	$name =~ s/^(czech)(.*)$/$1 $2/i;
	$name =~ s/^(chinese)(.*)$/$1 $2/i;
	$name =~ s/^(ESPERANTO)(.*)$/$1 $2/i;
	$name =~ s/^(french)(.*)$/$1 $2/i;
	$name =~ s/^(german)(.*)$/$1 $2/i;
	$name =~ s/^(haitian)(.*)$/$1 $2/i;
	$name =~ s/^(italian)(.*)$/$1 $2/i;
	$name =~ s/^(latin)(.*)$/$1 $2/i;
	$name =~ s/^(serbian)(.*)$/$1 $2/i;
	$name =~ s/^(spanish)(.*)$/$1 $2/i;
	$name =~ s/^(PORTUGUESE)(.*)$/$1 $2/i;
	$name =~ s/^(romanian)(.*)$/$1 $2/i;
	$name =~ s/^(russian)(.*)$/$1 $2/i;
	$name =~ s/\b(lanuova)(.*)$/$1 $2/i;
	$name =~ s/\b(hangulkjv)$/Hangul KJV/i;
	
	$name =~ s/([a-z])(\d)/$1 $2/gi;
	$name =~ s/([a-z])([A-Z])/$1 $2/g;
	$name =~ s/([a-z])_([a-z])/$1.' '.uc($2)/segi;
	$name =~ s/^([a-z])/uc($1)/seg;
	$name =~ s/\/([a-z])/'\/'.uc($1)/seg;
	$name =~ s/\s([a-z])/' '.uc($1)/seg;
	$name =~ s/\s(of|the|and|a)\s/' '.lc($1).' '/segi;
	
	$name =~ s/^(wbtc)(\w)(.*)$/uc($2).$3." WBTC"/segi;
	
	$name =~ s/union$/ Union/i;
	
	$name =~ s/\s{2,}/ /g;
	
	return $name;
}
