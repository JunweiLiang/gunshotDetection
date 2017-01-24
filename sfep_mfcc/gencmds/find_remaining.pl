#!/usr/bin/perl

use strict;
use warnings;

if( scalar(@ARGV) != 1 || !(-f $ARGV[0]) ){
	print STDERR "usage: ./find_remaining.pl cmds> remaining_cmds \n";
	exit;
}

my $num_checked = 0;
my $missing = 0;
open(BB, $ARGV[0]);
while(<BB>){
	chomp;
	my $cmd = $_;
	#find .record files in these scenearios: 'xxxxx.record' or touch xxxxx.record)
	$cmd =~ /([^ '"\)\(]*\.record)/;
	if( !defined $1 ){
		print STDERR "NO RECORD FILE FOUND IN COMMAND!!! ODD COMMAND $cmd!!!!\n";
	}
	if( !(-f $1) ){
		print $cmd."\n";
		$missing++;
	}
	$num_checked++;
}
close(BB);
if( $missing == 0 ){
	print STDERR "checked $num_checked files, check passed\n";
}else{
	print STDERR "missing $missing/$num_checked!!\n";
}
