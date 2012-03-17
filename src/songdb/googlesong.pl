#!/usr/bin/perl

use strict;
use LWP;
use HTML::TreeBuilder;
use JSON qw/encode_json/;

sub url_encode
{
	shift if $_[0] eq __PACKAGE__;
	local $_;
	$_=shift;
	s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
	s/ /+/g;
	$_;
}

sub url_decode
{
	shift if $_[0] eq __PACKAGE__;
	local $_;	
	$_=shift;s/\%([A-Fa-f0-9]{2})/pack('C', hex($1))/seg;$_;
}


my $query = shift || 'to god be the glory'; 
my $dont_preproc_url = shift || 0;

my($user_agent, $url, $browser, $request, $max_result, $response, $start,
$content, $search_result, @search_results);

my $per_page = 10;
$max_result = 10;
$start = 0;

$url = 'http://www.google.com/search?hl=en&q='.url_encode($query).'+lryics&aq=f&oq=&aqi=&num='.$per_page;
$user_agent = LWP::UserAgent->new();

#print "Searching...this may take a minute\n";
while ( $start < $max_result ) 
{
	$request = HTTP::Request->new(GET => $url."&start=$start");
	$user_agent->timeout(30);
	$user_agent->agent('Mozilla/5.0');
	$response = $user_agent->request($request);
	if($response->is_success)
	{
		$content .= $response->content;
		$start += $per_page;
	}
}

#parse $content with treebuilder
my $page = HTML::TreeBuilder->new();
$page->parse($content);
$page->eof();

#the following code finds every <li> item with class 'g' -- right now,
#this is how search results are styled on google. so, the HTML result
#for each item returned by google is stored in @search_results
#
#you could uncomment this:
#foreach $search_result (@search_results){
# print $search_result->as_HTML,"\n\n";
#}
#and see how each item looks in the array.

@search_results= $page->look_down(
sub{ $_[0]-> tag() eq 'li' and ($_[0]->attr('class') =~ /g/)}
);

my @result_list;
foreach $search_result (@search_results){
	my($url, $title, $summary);

	#now that we have each HTML chunk of search results stored in an array
	#we can take it apart further:

	$page = HTML::TreeBuilder->new_from_content($search_result->as_HTML);

	#the title is styled as <h3 class=r>
	$title = $page->look_down(
		sub{ $_[0]-> tag() eq 'h3' and ($_[0]->attr('class') =~ /r/)}
	);
	#the summary is styled as <div class=s>
	$summary = $page->look_down(
		sub{ $_[0]-> tag() eq 'div' and($_[0]->attr('class') =~ /s/)}
	);

	#now we have to get the href attribute from the title to get the link
	#so we load the title, as HTML, into the treebuilder object
	#print STDERR "title: $title, summary: $summary\n";
	next if !$title;
	$page = HTML::TreeBuilder->new_from_content($title->as_HTML);

	#the link is styled as <a class=l href="..."
	#the following assigns $url the href attribute
	$page->look_down(
		sub{ $_[0]-> tag() eq 'a' and
		($_[0]->attr('class') =~ /l/), $url = $_[0]->attr('href')}
	);

	#print everything out...
	#if($title) { print 'title: '.$title->as_text."\n";}
	#if($summary){ print 'summary: '.$summary->as_text."\n";}
	#if($url){ print 'url: '.$url."\n\n";}
	
	unless($dont_preproc_url)
	{
		$url =~ s/^\/url\?//g;
		my @parts = split /&/, $url;
		my $q = shift @parts;
		$q =~ s/^q=//g;
		$url = url_decode($q);
	}
	
	push @result_list, 
	{
		title => $title ? $title->as_text : undef,
		summary => $summary ? $summary->as_text : undef,
		url => $url,
	};
}

print encode_json(\@result_list);

#delete the treebuilder object.
$page->delete;
