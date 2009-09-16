#!/usr/bin/perl
use strict;

use DBI;
require 'SimpleDOM.pm';
use Data::Dumper;

my $songs_xml = shift || die "Missing Songs XML file - file not specified.\nUsage: $0 <songs XML file> <output SQLite DB file>";
my $songs_db  = shift || die "Missing Songs DB file - file not specified.\nUsage: $0 <songs XML file> <output SQLite DB file>";

my $force = lc(shift) eq 'force';
if(-f $songs_db && !$force)
{
	die "Error: $songs_db already exists, I won't overwrite unless the third arg on the command line says 'force'.\n";
}

unlink($songs_db) if -f $songs_db;

print "$0: Starting, reading '$songs_xml', writing '$songs_db' ...\n";
my $dbh = DBI->connect("dbi:SQLite:dbname=$songs_db","","");


$dbh->do(q{
	CREATE TABLE songs (
		songid integer primary key AUTOINCREMENT,
		number int,
		title varchar(255),
		text text,
		author varchar(255),
		copyright varchar(255),
		last_used datetime
	)
});

my $sth_ins_song = $dbh->prepare('INSERT INTO songs (title,text,number) VALUES  (?,?,?)');
		
	




my $xml_blob = `cat $songs_xml`;
$xml_blob =~ s/<alpha name="Copyright">([^\<]+?)<\/alpha>//g;


my $blob = EAS::XML::SimpleDOM::load_xml($xml_blob) || die "Error parsing $songs_xml: $!";
my $dom = EAS::XML::SimpleDOM::->new_node($blob->{list}->[0]);

#print Dumper $dom;

my @rows = @{ $dom->rows || [] };
if(!@rows)
{
	die "No <row> tags found in $songs_xml at the second level.\n";
}
else
{
	foreach my $row (@rows)
	{
		my %alpha = map { $_->name => $_->value } @{ $row->alphas || [] };
		my $text = $row->memoblob;
		my $title = $alpha{Title};
		#next if $title !~ /(-\s*Praise Song|\#?\s*(\d{2,3}))/i;
		print "Processing '$title' ...\n";
		
		my ($nbr) = $title =~ /\#?\s*(\d{2,3})/i;
		open(TMP,">/tmp/words.rtf");
		print TMP $text;
		close(TMP);
		system("rtf2text /tmp/words.rtf > /tmp/words.txt");
		my $words = `cat /tmp/words.txt`;
		#print "$title\n-----------------------------\n";
		#print "$words\n\n";
		
		
		
		$sth_ins_song->execute($title,$words,$nbr);
		
		#die "Test done";
	}
}