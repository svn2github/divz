#!c:\Perl\bin\perl.exe
use strict;
`svn info -r HEAD`=~/Changed Rev:\s+(\d+)/;print $1