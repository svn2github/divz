#!/usr/bin/perl

my @data;
open(FILE,"<book-abbrv-list.txt");
@data = <FILE>;
close(FILE);

my @names;
my @proper_names;

print "#include \"BibleModel.h\"\n";
print "bool BibleVerseRef::bookNameMap_initalized = false;\n";
print "QHash<QString,QString> BibleVerseRef::bookNameMap;\n";
print "QStringList BibleVerseRef::bookNameList;\n";
print "void BibleVerseRef::initNameMap()\n{\n";
print "\tif(bookNameMap_initalized)\n\treturn;\n";
foreach my $line (@data)
{
	my ($book,$list) = $line =~ /^(.*?)\t(.*)$/;
	$book =~ s/(^\s+|\s+$)//g;
	my @list = split /,\s+/, $list;
	foreach my $abbrv (@list)
	{
		print "\tbookNameMap[\"".lc($abbrv)."\"] = \"$book\";\n";
		print "\tbookNameList << \"$abbrv\";\n";
		push @names, lc($abbrv);
	}
	print "\tbookNameMap[\"".lc($book)."\"] = \"$book\";\n";
	print "\tbookNameList << \"$book\";\n";
	push @names, lc($book);
}
print "\tbookNameMap_initalized = true;\n";
print "}\n";
print "QRegExp BibleVerseRef::referenceExtractRegExp(\"((?:".join('|',@names).")\\\\s+(?:[0-9]+)(?:[:\\\\.][0-9]+)?(?:\\\\s*-\\\\s*[0-9]+)?)\",Qt::CaseInsensitive);\n"; 
