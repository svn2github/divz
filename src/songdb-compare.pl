#!/usr/bin/perl
use strict;
use DBI;
my $dbh     = DBI->connect("dbi:SQLite:dbname=songs.db");
my $dbh_old = DBI->connect("dbi:SQLite:dbname=songs-old.db");
# CREATE TABLE songs (
#           songid integer primary key AUTOINCREMENT,
#           number int,
#           title varchar(255),
#           text text,
#           author varchar(255),
#           copyright varchar(255),
#           last_used datetime
#         , tags varchar(255));

my %old_songs_id;
my %old_songs_title;
my %new_songs_id;

my $sth;

$sth = $dbh_old->prepare('select * from songs');
$sth->execute;

while(my $ref = $sth->fetchrow_hashref)
{
	$old_songs_id{$ref->{songid}} = $ref;
	$old_songs_title{$ref->{title}} = $ref;
}

# Compare  songid, title, texct

$sth = $dbh->prepare('select * from songs');
$sth->execute;

while(my $ref = $sth->fetchrow_hashref)
{
	$new_songs_id{$ref->{songid}} = $ref;
	
	my $old_song = $old_songs_id{$ref->{songid}};
	
	if(!$old_song)
	{
		# Song doesn't exist by the new ID in the old database
		# Note: We will have to do another loop to catch songs in the old database that dont exist in the new by ID
		print "** Song # $ref->{songid} - \"$ref->{title}\" is NEW (no record by ID in old)\n";
		
		my $old_by_title = $old_songs_title{$ref->{title}};
		if($old_by_title)
		{
			print "\t Song DOES exist in OLD database by titlte as songid $old_by_title->{songid}\n";
		}
	}
	else
	{
		compare_songs($old_song,$ref);
	}
}

# Now check for songs by ID that are in OLD but not in NEW
foreach my $id (keys %old_songs_id)
{
	my $old_song = $old_songs_id{$id};
	my $new_song = $new_songs_id{$id};
	if(!$new_song)
	{
		print "xx Song # $old_song->{songid} - \"$old_song->{title}\" is OLD - exists in OLD DB, but not in NEW DB (no record by ID in new)\n";
	}
}

sub compare_songs
{
	my $old_song = shift;
	my $ref = shift;
	# Found matching songid in old database
	# Compare title and text
	if($old_song->{title} ne $ref->{title})
	{
		print "~~ Song # $ref->{songid} - \"$ref->{title}\" - different title than old song: \"$old_song->{title}\"\n";
	}
	if($old_song->{text} ne $ref->{text})
	{
		print "~~ Song # $ref->{songid} - \"$ref->{title}\" - different TEXT than old song, first 40 characters compared:\n";
		my $old_40 = substr($old_song->{text},0,40);
		my $new_40 = substr($ref->{text},0,40);
		
		$old_40 =~ s/[\r\n]/\//g;
		$new_40 =~ s/[\r\n]/\//g;
		
		print "\t Old: \"$old_40...\"\n";
		print "\t New: \"$new_40...\"\n";
	}

}