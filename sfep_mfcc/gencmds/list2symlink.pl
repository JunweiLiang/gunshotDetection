#!/usr/bin/perl

use strict;
use warnings;

if( scalar(@ARGV) != 3 || !(-f $ARGV[0]) || !(-f $ARGV[1]) || !(-f $ARGV[2]) ){
	print STDERR "usage: perl list2symlink.pl actual_path sym_path not_found_path\n";
	exit -1;
}

my %mm = ();
open(AA, "$ARGV[0]");
while(<AA>){
	chomp;
	my $a = $_;
	my $p = $_;
	$a =~ s/.*\///;
	$a =~ s/\..*//;
	$mm{ $a } = $p;
}
close(AA);

my @cmds = ();
my %dirs = ();
my $t = 0;
my $nf = 0;
open(BB, $ARGV[1]);
while(<BB>){
	chomp;
	my $p = $_;
	my $n = $_;
	my $d = $p;
	$n =~ s/.*\///;
	$d =~ s/$n$//;
	$n =~ s/\..*//;
	my $mapped = $mm{$n};
	if( !(defined $mapped) ){
		print STDERR "$n not found, using not found path $ARGV[2]\n";
		push @cmds, "ln -s $ARGV[2] $p";
		$nf++;
		$t++;
		next;
	}
	$dirs{$d} = 1;
	$t++;
	push @cmds, "ln -sf $mapped $p";
}
close(BB);

print STDERR "$nf/$t missing\n";
foreach my $d (keys %dirs){
	system("mkdir -p $d");
}

for(my $i = 0; $i < scalar(@cmds); $i++){
	system($cmds[$i]);
	if( $i % 1000 == 0 ){
		print STDERR "$i/".scalar(@cmds)."\n";
	}
}

