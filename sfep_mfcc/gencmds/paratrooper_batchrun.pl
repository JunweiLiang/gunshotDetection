#!/usr/bin/perl

use strict;
use warnings;

while(<STDIN>){
	chomp;
	if( /gensym/ ){
		/\d\d\d_(.*)_gensym\.sh/;
		my $fname = $1;
		if( !(-f "/data/MM2/MED/labelsets/filelists/$fname.list") ){
			print STDERR "/data/MM2/MED/labelsets/filelists/$fname.list not found!\n";
		}
		print "sh $_ /data/MM2/MED/labelsets/filelists/$fname.list\n";
	}else{
		print "sh $_\n";
	}
}
