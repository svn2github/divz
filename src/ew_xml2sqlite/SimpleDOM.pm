# Package: EAS::XML::SimpleDOM
# Simple XML class, not a true DOM, but useful in its own right. It's practical, rather than puritanical. Uses XML::DOM to load the documents.

package EAS::XML::SimpleDOM;



use strict;
use XML::DOM;

use Carp;

use Digest::MD5 qw/md5_hex/;
use Storable qw/store retrieve/;
use File::stat;

our $AUTOLOAD;  # it's a package global

my %cache;

=head1 Synopsis
  Example: 
	my $dom1 = EAS::XML::SimpleDOM->parse_xml('/my/config.xml')
	my $usage = EAS::XML::SimpleDOM->parse_xml('<a_bunch_of><xml><data usage="just for demo"/></xml></a_bunch_of>')->xml->data->usage;
=cut

sub parse_xml
{
	#my $node = load_xml($data)->{root};

	my $class = shift;
	my $file  = shift;
	
	return $class->new_node(load_xml($file)->{root});
}

sub new_node 
{
	my $that  = shift;
	my $class = ref($that) || $that;
	my $node = shift;
		
	#return $cache{$node} if defined $cache{$node};
	#die "Got past cache for $node\n";
	#print "Got past cache for node $node\n";
	$node = bless $node, $class;
	$cache{$node} = $node;
	if($node =~ /HASH/ )
	{
		foreach(keys %{$node->{attrs}})
		{
			$node->{$_} = $node->{attrs}->{$_} if !defined $node->{$_};
		}
	}
	
	return $node;
}	

use Data::Dumper;

use overload '""' => sub {my $x=shift;UNIVERSAL::isa($x,'HASH')&&$x->{value}?$x->{value}:$x};

#sub get{my $n=shift;$AUTOLOAD=shift;$n->AUTOLOAD}
sub to_xml
{
	my $node = shift;
	my $t = shift || "";
	#die $node;

	my $name = lc $node->node;
	#print STDERR "to_xml: $t $name\n";
	
	my @xml;
	
	push @xml, $t, "<".$name. (keys %{$node->attrs} ? " " : "");
	push @xml, join (" ", map { $_ . "=\"".encode_entities($node->attrs->{$_})."\"" } keys %{$node->attrs});


	if(@{ $node->children })
	{
		push @xml, ">\n";
		foreach my $child (@{$node->children})
		{
			push @xml, $child->to_xml($t."\t");
		}
		push @xml, $t, "</".$name.">\n";
	}
	elsif($node->value && !$node->{attrs}->{value})
	{
		push @xml, '>';
		push @xml, $node->value;
		push @xml, "</".$name.">\n";
	}
	else
	{
		push @xml, "/>\n";
	}
	
	return join '', @xml;
}


sub AUTOLOAD 
{
	my $node = shift;
	#my $type = ref($node)
	#	or croak "$node is not an object";
	
	my $name = shift || $AUTOLOAD;
	$name =~ s/.*:://;   # strip fully-qualified portion
	
	return if $name eq 'DESTROY';
	
	#print STDERR "DEBUG: AUTOLOAD() [$node] ACCESS $name\n"; # if $debug;
	return $node->get($name);
}

sub get
{
	my $node = shift;
	my $name = shift;

	#my $debug = 1 if $name eq 'status';
	#print STDERR "DEBUG: get() [$node] ACCESS $name\n"; # if $debug;
	
	my $is_hash = UNIVERSAL::isa($node,'HASH'); #$node =~ /HASH/;
	
	if($name eq 'children')
	{
		my @kids = $is_hash ? @{$node->{children}} : @$node;
		#@kids = grep { $_->{node} ne '#text' || $_->{value} =~ /[^\s\n\r]/ } @kids;
		$_ = $node->new_node($_) foreach @kids;
		@kids = grep { $_->{node} ne '#text' || $_->{value} =~ /[^\s\n\r]/ } @kids;
		$node->{children} = \@kids;
		
		return $node unless $is_hash;
	}
	
	if($is_hash)
	{
		return $node->{$name}          || $node->{lc $name}          if defined $node->{$name}          || defined $node->{lc $name};
		return $node->{attrs}->{$name} || $node->{attrs}->{lc $name} if defined $node->{attrs}->{$name} || defined $node->{attrs}->{lc $name};
	}
	
	#print STDERR "DEBUG: $node: $name...\n" if $debug;
	#print STDERR "DEBUG: $node: [".ref($node)."]\n" if $debug;
	#print "DEBUG: [$node] ACCESS $name - mark 2\n" if $debug;
	my @kids = $is_hash ? @{$node->{children}} : @$node;
	foreach(@kids)
	{
		#return $node->new_node($_) if $_->{node} eq $name || $_->{attrs}->{id} eq $name || $_->{attrs}->{name} eq $name;
		if ($_->{node} eq $name || $_->{attrs}->{id} eq $name || $_->{attrs}->{name} eq $name)
		{
			#EAS::Common::print_stack_trace;
			my $x = $node->new_node($_);
			#print Dumper $x;
			return $x;
			
		}
		
	}
	
	#print "DEBUG: [$node] ACCESS $name - mark 3\n" if $debug;
	
	if($name =~ /^.+s$/)
	{
		my $m = $name; 
		$m =~ s/s$//g;
		my @list;
		foreach(@kids)
		{
			push @list, $node->new_node($_) if $_->{node} eq $m;
		}
		return $node->new_node(\@list) if @list;
	}
	
	#croak "Unknown field '$name'"; # for node '$node->node'";
	return undef;
}  




# =================
# Static Utility Functions - handles most of the loading

sub load_xml
{
	my $file = shift;	
	my $name = shift;
	
	#print STDERR "file=$file, name=$name\n";
	
	if(!-f $file)
	{
		warn "Invalid file ".substr($file,0,255).", trying to load as XML" unless index($file,'>') > 0;
		#EAS::Common::print_stack_trace();
		#EAS::Web::Common::error("data",[$file,$name,-f $file]);
		my $parser = new XML::DOM::Parser;
		
		$file =~ s/^.*?<\?xml[^\>]+>//g;
		
		
		my $doc = $parser->parse ($file);
		
		#die Dumper($doc);
		
		my %id_hash;
		my $list = node_to_list($doc,\%id_hash);
		my $root = shift @{$list};
		
		#print Dumper $list,\%id_hash;
		
		my $rep = {root=>$root,tags=>\%id_hash, _mtime => undef, list => $list};
		
		$doc->dispose;
		
		return $rep;
	}

	my $cache = '/tmp/eas-xml-simpledom-'.md5_hex($file).'.cache.storable';
	
	#print "Parsing XML $file...\n";
	
	die "Invalid file '$file'" if !-f $file;
	
	my $rep = -f $cache ? retrieve($cache) : undef;
	
	my $mtime = stat($file)->mtime + 199;
	
	#print STDERR "my mtime: $mtime, rep->{_mtime} is $rep->{_mtime}\n";
	
	if(!$rep || $rep->{_mtime} != $mtime)
	{
		#print STDERR "[-CACHE MISS] for $file, \$cache=$cache\n";
		my $parser = new XML::DOM::Parser;
		my $doc = $parser->parsefile ($file);
		
		#die Dumper($doc);
		
		my %id_hash;
		my $root = shift @{node_to_list($doc,\%id_hash)};
		
		#print Dumper $list,\%id_hash;
		
		$rep = {root=>$root,tags=>\%id_hash, _mtime => $mtime};
		
		$doc->dispose;
		
		store $rep, $cache;
		system("chmod 777 $cache");
	}
	else
	{
		#print STDERR "[+CACHE HIT] for $file, \$cache=$cache\n";
	}
	
	
	return $rep;	
}


sub doid
{
	my $byid = shift;
	my $ref = shift;
	
	if(ref $ref eq 'HASH')
	{
		#print STDERR "HASH:".Dumper $ref;
		foreach my $key (keys %$ref)
		{
			$byid->{$key} = $ref;
			doid($byid,$ref->{$key}) if ref $ref->{$key};
		}
		$byid->{$ref->{id}} = $ref if $ref->{id};
	}
	elsif(ref $ref eq 'ARRAY')
	{
		#die Dumper $ref;
		#print STDERR "ARRAY:".Dumper $ref;
		doid($byid,$_) foreach @$ref;
	}
}

sub node_to_list
{
	my $master = shift;
	my $hash = shift || {};
	
	my @nodes = $master->getChildNodes;
	
	my @list;
	
	foreach my $node (@nodes)
	{
		my $ref = {
			node	=>	$node->getNodeName,
			value	=>	$node->getNodeValue,
		};
		
		my @attribs;
		
		#print "Name: ",$node->getNodeName,", Value: ",$node->getNodeValue,"\n";
		
		my $map = $node->getAttributes();
		if($map)
		{
			my @list = $map->getValues;        # returns a perl list
			foreach my $attr (@list)
			{
				#print "\tName: ",$attr->getNodeName,", Value: ",$attr->getNodeValue,"\n";
				push @attribs, { attrib => $attr->getNodeName, value => $attr->getNodeValue };
			}
		}
		
		my %map = map {$_->{attrib}=>$_->{value}} @attribs;
		
		$ref->{attrs}    = \%map;
		#$ref->{attrlist} = \@attribs;
		
		$ref->{children} = node_to_list($node,$hash);
		
		if(!$ref->{value})
		{
			
			my @text;
			my @real;
			foreach my $child (@{$ref->{children}})
			{
				if($child->{node} eq '#text')
				{
					push @text, $child->{value} unless $child->{value} =~ /^(\s|\n)*$/;
				}
				else
				{
					push @real, $child;
				}
			}
			
			if(!@real && @text)
			{
				$ref->{children} = \@real;
				$ref->{value} = join('',@text);
			}
			else
			{
				#my @kids = grep { $_->{node} ne '#text' || $_->{value} =~ /[^\s\n\r]/ } @{$ref->{children}};
				#$ref->{children} = \@kids;
			}
		}
		
		$hash->{$map{id}} = $ref if defined $map{id};
		
		push @list, $ref;
	}
	
	#@list = grep { $_->{node} ne '#text' || $_->{value} && $_->{value} =~ /[^\s\n\r]/ } @list;
	return \@list;
}

sub find_tags
{
	my $node = shift;
	
	$node = $node->{root} if exists $node->{root};
	
	my $tag = shift;
	
	my $one = shift || 0;
	
	#print "Search[$tag], At[$node->{node}]\n";
	my @tags;
	push @tags, $node if $node->{node} eq $tag;
	return @tags if @tags && $one;
	
	foreach my $child (@{$node->{children}})
	{
		push @tags, find_tags($child,$tag,$one);
		return @tags if @tags && $one;
	}
	
	return @tags;
}




1;