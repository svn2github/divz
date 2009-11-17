#!/usr/bin/perl

my @data;
open(FILE,"<book-abbrv-list.txt");
@data = <FILE>;
close(FILE);

my @names;

print "#include \"BibleModel.h\"\n";
print "bool BibleVerseRef::bookNameMap_initalized = false;\nQHash<QString,QString> BibleVerseRef::bookNameMap;\n";
print "void BibleVerseRef::initNameMap()\n{\n";
foreach my $line (@data)
{
	my ($book,$list) = $line =~ /^(.*?)\t(.*)$/;
	$book =~ s/(^\s+|\s+$)//g;
	my @list = split /,\s+/, $list;
	foreach my $abbrv (@list)
	{
		print "\tbookNameMap[\"".lc($abbrv)."\"] = \"$book\";\n";
		push @names, lc($abbrv);
	}
	print "\tbookNameMap[\"".lc($book)."\"] = \"$book\";\n";
	push @names, lc($book);
}
print "\tbookNameMap_initalized = true;\n";
print "}\n";
print "QRegExp BibleVerseRef::referenceExtractRegExp(\"((?:".join('|',@names).")\\\\s+(?:[0-9]+)(?:[:\\\\.][0-9]+)?(?:\\\\s*-\\\\s*[0-9]+)?)\",Qt::CaseInsensitive);\n"; 