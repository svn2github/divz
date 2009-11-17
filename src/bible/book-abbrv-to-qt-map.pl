#!/usr/bin/perl

my @data;
open(FILE,"<book-abbrv-list.txt");
@data = <FILE>;
close(FILE);

print "#include \"BookModel.h\"\n";
print "bool BibleVerseRef::bookNameMap_initalized = false;\nQHash<QString,QString> BibleVerseRef::bookNameMap;\n";
print "void BibleVerseRef::initNameMap()\n{\n";
foreach my $line (@data)
{
	my ($book,$list) = $line =~ /^(.*?)\t(.*)$/;
	$book =~ s/(^\s+|\s+$)//g;
	my @list = split /,\s+/, $list;
	foreach my $abbrv	(@list)
	{
		print "\tbookNameMap[\"".lc($abbrv)."\"] = \"$book\"\n";
	}
}
print "\tbookNameMap_initalized = true;\n";
print "}\n";