#!/usr/bin/perl

use strict;
use warnings;
use Cwd;
my @videos=("HVC767576", "HVC806846", "HVC685238");

if( scalar(@ARGV) != 1 ){
	print STDERR "perl run.pl cluster(rocks|psc)\n";
	exit -1;
}
my $cluster = $ARGV[0];
my $cwd = getcwd;
#Set up necessary paths for Janus
my $BINPATH = "$cwd/bin";
my $JANUSROOT = $cwd."/janus_code/$cluster/";
$ENV{"JANUSHOME"} = "$JANUSROOT/janus";
$ENV{"JANUS_LIBRARY"} = $ENV{"JANUSHOME"}."/library";
if( defined $ENV{"LD_LIBRARY_PATH"} ){
	$ENV{"LD_LIBRARY_PATH"} = "/opt/tcltk-8.5.10/lib/:$JANUSROOT/janus-libs/:".$ENV{"LD_LIBRARY_PATH"};
}else{
	$ENV{"LD_LIBRARY_PATH"} = "/opt/tcltk-8.5.10/lib/:$JANUSROOT/janus-libs/";
}
my $JANUS_SCRIPT=$JANUSROOT."/janus/janus";

my $precmd = "echo";
if( $cluster eq "psc" ){
        $precmd = "source /usr/share/modules/init/bash && module load tcl && module load gcc && module load ATLAS && module load lapack";
}

open(LST, ">tmp/video.lst");
for(my $i = 0; $i < scalar(@videos); $i++){
	print LST "$cwd/../videos/$videos[$i].mp4\n";
}
close(LST);

#extract the features
execute("$precmd && $JANUS_SCRIPT $BINPATH/convert-v0.tcl $cwd/tmp/video.lst -featName FEAT -featDesc $BINPATH/featDesc_mfcc40 -featExt mfcc40 -outPath $cwd/tmp");

for(my $i = 0; $i < scalar(@videos); $i++){
	my $vid = $videos[$i];
	execute("bzip2 -d tmp/$vid.mfcc40.bz2");
}

my @mfccs = (
		[("mfcc4k", "bin", "clusterCenters/selected.mfcc.11746.v02.csv.4096.cluster_centres", 4096)],
		[("mfcc16kreduced", "bin", "clusterCenters/reduced.vocab", 4096)],
		[("mfcc8kstack5", "Stack5", "clusterCenters/selectedMFCC_5_stack5_2000_8K.csv.cluster_centres", "8192")]
	);

my $failed = 0;
for(my $i = 0; $i < scalar(@mfccs); $i++){
	my $name = $mfccs[$i][0];
	my $jar_dir = $mfccs[$i][1];
	my $centroids = $mfccs[$i][2];
	my $centroids_count = $mfccs[$i][3];
	for(my $j = 0; $j < scalar(@videos); $j++){
		my $vid = $videos[$j];
		my $mfcc = $vid.".mfcc40";
		my $cmd = "java -cp $jar_dir/bof.jar txyc $centroids tmp/$vid.mfcc40 10 tmp/$name";
		execute($cmd);
    		$cmd = "java -cp $jar_dir/bof.jar bof tmp/$name/$vid.txyc $centroids_count 10 tmp/$name";
		execute($cmd);
		my $diff = `perl correlation.pl tmp/$name/$vid.bof answer_output/$name/$vid.bof $centroids_count`;
		if( $diff < 0.99 ){
			print STDERR "$vid $name correlation only $diff!\n";
			$failed = 1;
		}
	}
}
if( $failed == 0 ){
	print STDERR "check passed!!\n";
}else{
	print STDERR "check failed!!\n";
}

sub execute{
	my $cmd = shift;
	print STDERR "$cmd\n";
	system($cmd);
}
