#!/usr/bin/perl

open(MTIME,"<mtime.dat");
my $mtime = <MTIME>;
close(MTIME);
$mtime =~ s/[\r\n]//g;
$mtime +=0;

my $file = "book-abbrv-list.txt";
my $cpp = "BookNameMap.cpp";

my $new_mtime = (stat($file))[9];
if($new_mtime <= $mtime)
{
	die "$file not modified, not regenerating $cpp.\n";
}

open(MTIME,">mtime.dat");
print MTIME $new_mtime;
close(MTIME);


my @data;
open(FILE,"<$file");
@data = <FILE>;
close(FILE);

open(CPP, ">$cpp") || die "Cannot open $cpp for writing: $!";

my @names;
my @proper_names;

print CPP "#include \"BibleModel.h\"\n";
print CPP "bool BibleVerseRef::bookNameMap_initalized = false;\n";
print CPP "QHash<QString,QString> BibleVerseRef::bookNameMap;\n";
print CPP "QStringList BibleVerseRef::bookNameList;\n";
print CPP "void BibleVerseRef::initNameMap()\n{\n";
print CPP "\tif(bookNameMap_initalized)\n\treturn;\n";
foreach my $line (@data)
{
	my ($book,$list) = $line =~ /^(.*?)\t(.*)$/;
	$book =~ s/(^\s+|\s+$)//g;
	my @list = split /,\s+/, $list;
	foreach my $abbrv (@list)
	{
		print CPP "\tbookNameMap[\"".lc($abbrv)."\"] = \"$book\";\n";
		print CPP "\tbookNameList << \"$abbrv\";\n";
		push @names, lc($abbrv);
	}
	print CPP "\tbookNameMap[\"".lc($book)."\"] = \"$book\";\n";
	print CPP "\tbookNameList << \"$book\";\n";
	push @names, lc($book);
}
print CPP "\tbookNameMap_initalized = true;\n";
print CPP "}\n";
print CPP "QRegExp BibleVerseRef::referenceExtractRegExp(\"((?:".join('|',@names).")\\\\s+(?:[0-9]+)(?:[:\\\\.][0-9]+)?(?:\\\\s*-\\\\s*[0-9]+)?)\",Qt::CaseInsensitive);\n"; 

close(CPP);
