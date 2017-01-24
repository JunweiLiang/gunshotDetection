#!/usr/bin/perl

use strict;
use warnings;
use File::Path;
use Cwd;

my $get_vid_info_chunk_size = 20000;
my $fisher_chunk_size = 100;
my $objectbank_chunk_size = 10;

my $noiseme_chunk_size = 100000;
#my $SBPCA_chunk_size = 200;
my $mfcc_chunk_size = 100000;
my $lmel_chunk_size = 100000;
my $asr_chunk_size = 100000;
my $PQ_dir = "PQ";
my $PQ_mapping_threads = 12;

#fisher info
my %fisher = (); #dim = pca_dim * gmms * 2 * 8
$fisher{"sift"} = [(80, 256, 2, 0.6, 0, ".gz", ".fv.spbof")]; #pca dim, gmms, pnorm, alpha, is resized?
$fisher{"csift"} = [(140, 256, 2, 0.4, 0, ".gz", ".fv.spbof")];
$fisher{"tch"} = [(35, 512, 2, 0.6, 0, ".gz", ".fv.spbof")];
$fisher{"MoSIFT_resized"} = [(80, 256, 2, 0.2, 1, ".gz", ".fv.spbof")];
$fisher{"stip"} = [(60, 256, 2, 0.6, 0, ".gz", ".fv.spbof")];
$fisher{"stip_new"} = [(60, 256, 2, 0.4, 1, ".gz", ".fv.spbof")];
$fisher{"trajectory"} = [(160, 256, 2, 0.6, 1, ".gz", ".fv.spbof")];
$fisher{"mfcc"} = [(20, 256, 2, 0.5, 2, "", ".fv")]; #last one is 2, no spatial information

#gmm info
my %gmm = (); #dim = pca_dim * gmms * 8
$gmm{"sift"} = [(60, 256, 0, "SIFT")]; #pca dim, gmms, is resized?
$gmm{"csift"} = [(32, 256, 0, "CSIFT")];
$gmm{"tch"} = [(32, 512, 0, "TCH")];
$gmm{"MoSIFT_resized"} = [(70, 256, 1, "MOSIFT")];
$gmm{"stip"} = [(80, 256, 0, "STIP")];
$gmm{"stip_new"} = [(60, 256, 1, "STIP")];
$gmm{"mfcc"} = [(32, 512, -1, "MFCC")];

if( scalar(@ARGV) != 1 ){
		print STDERR "usage: perl eageae_wrapper.pl config_file\n";
		exit(-1);
}

if( !(-f $ARGV[0]) ){
		print STDERR "could not find config file $ARGV[0]\n";
		exit(-1);
}

my @param_names = ("sfepdir", "outputdir", "videopaths", "machine", "python27");
my %params = ();
open(CONFIG, $ARGV[0]);
while(<CONFIG>){
		chomp;
		my @arr = split /=/, $_;
		$arr[0] = lc( $arr[0] );
		for( my $i = 2; $i < scalar(@arr); $i++){
				$arr[1] .= "=".$arr[$i];
		}
		$arr[1] =~ s/\"//g;
		$params{ $arr[0] } = $arr[1];
}
close(CONFIG);
for( my $i = 0; $i < scalar(@param_names); $i++){
		if( !defined $params{ $param_names[$i] } ){
				print STDERR "parameter $param_names[$i] not set!!\n";
				exit(-1);
		}
}

my $machine = $params{ "machine" };
if( $machine ne "rocks" && $machine ne "psc" ){
	print STDERR "unknown machine $machine, only rocks or psc is available.\n";
	exit -1;
}

my $sfepdir = $params{ "sfepdir" };
my $outputdir = $params{ "outputdir" };

if( !(-f "$sfepdir/gencmds/gencmds.pl") ){
	print STDERR "cannot find $sfepdir/gencmds/gencmds.pl, is sfepdir set correctly?\n";
	exit -1;
}
my $python27 = $params{ "python27" };
#$ENV{"PATH"} = "/opt/python27/bin/:".$ENV{"PATH"};
my $pythonpath = "$sfepdir/asr_noisemes/base/lib/python2.7/site-packages";
if( defined $ENV{"PYTHONPATH"} ){
	$pythonpath .= ":".$ENV{"PYTHONPATH"};
}

mkpath($outputdir);
#my $tmpdir = "$outputdir/tmp";
my $tmpdir = "/tmp";
my $featdir = "$outputdir/features";
my $scriptdir = "$outputdir/scripts";
mkpath($tmpdir);
mkpath($featdir);
mkpath($scriptdir);
my $linear_not_found_vector = "$featdir/linear_not_found_vector";
execute("touch $linear_not_found_vector");

my $ramdisk_prefix = "";
if( $machine eq "rocks" ){
	$ramdisk_prefix = "export SCRATCH_RAMDISK=$tmpdir &&";
}
if( $machine eq "psc" ){
	$tmpdir = "\$SCRATCH_RAMDISK";
}

if( !(-f $params{ "videopaths" } ) ){
	print STDERR "cannot find ".$params{ "videopaths" }."\n";
	exit -1;
}

my @vids = ();
my $prefix = "";
my $first = 1;
open(VIDS, $params{ "videopaths" });
while(<VIDS>){
	chomp;
	push @vids, $_;
	if( !(-f $_) ){
		die("video $_ not found!\n");
	}
	if( $first == 1 ){
		my $tp = $_;
		my $tp2 = $tp;
		$tp =~ s/.*\///;
		$tp2 =~ s/$tp$//;
		$prefix = $tp2;
	}else{
		my $now = $_;
		while( !( $now =~ /$prefix/ ) ){
			$prefix = substr($prefix, 0, length($prefix) - 1);
			my $lastdir = $prefix;
			$lastdir =~ s/.*\///;
			$prefix =~ s/$lastdir$//;
		}
	}
	$first = 0;
}
close(VIDS);
print STDERR "found ".scalar(@vids)." videos with prefix $prefix\n";
my $cmd_id = 0;

#get video width / height
my $video_info = "$featdir/video_info";
if( defined $params{ "video_info" } ){
	$video_info = $params{ "video_info" };
	if( !(-f $video_info) ){
		print STDERR "$video_info not found!\n";
		exit -1;
	}
	$cmd_id += 2;
}else{
	get_video_width_height( \@vids, $get_vid_info_chunk_size, "get_video_info", $video_info, $scriptdir);
}

#gen resize cmds
my $resized_vids_dir = "$featdir/resized_video/";
mkpath($resized_vids_dir);
my $resize_record_dir = "$featdir/resized_video/record";
mkpath($resize_record_dir);
my $outscript = "$scriptdir/".output_sh("gen_resize_cmd.sh");
my $resize_script = "$scriptdir/".output_sh("resize.sh");
execute("echo python $sfepdir/resize_code/python/gen_resize_compress_ffmpeg_cmd.py $sfepdir/resize_code/$machine/ffmpeg $prefix $video_info $resized_vids_dir $resize_script $sfepdir $resize_record_dir > $outscript");
output_records( "$scriptdir/" . output_sh("resize_check.sh"), $resize_script);

#get video width /height for resized
my @vids_resized = ();
for(my $i = 0; $i < scalar(@vids); $i++){
	my $a = $vids[$i];
	$a =~ s/^$prefix/$resized_vids_dir/;
	push @vids_resized, $a;
}

my $video_info_resized = "$featdir/video_info_resized";
if( defined $params{ "video_info_resized" } ){
	$video_info_resized = $params{ "video_info_resized" };
	if( !(-f $video_info_resized) ){
		print STDERR "$video_info_resized not found!\n";
		exit -1;
	}
	$cmd_id += 2;
}else{
	get_video_width_height( \@vids_resized, $get_vid_info_chunk_size, "get_video_info_resized", $video_info_resized, $scriptdir);
}



#gen keyframes
my @cmds = ();
my @records = ();
my $kf_dir = "$featdir/keyframes/output";
my $segx_dir = "$featdir/keyframes/segx";
my $kf_record_dir = "$featdir/keyframes/record";
my $colorprefix = "$sfepdir/ColorDescriptors";
my %dirs = (); 
my @kfs = ();
execute("mkdir -p $segx_dir");
for(my $i = 0; $i < scalar(@vids); $i++){
	my $kf_out = change_prefix($vids[$i], $prefix, $kf_dir, \%dirs, '.mp4', '.tar.gz');
	push @kfs, $kf_out;
	my $kf_record_out = change_prefix($vids[$i], $prefix, $kf_record_dir, \%dirs, '.mp4', '.record');
	my $cmd = "($ramdisk_prefix $colorprefix/$machine/keyframe_extract/kf_wrapper $colorprefix/$machine/keyframe_extract/VideoSegHisto $vids[$i] $kf_out \$SCRATCH_RAMDISK $colorprefix/$machine/keyframe_extract $segx_dir && touch $kf_record_out) >/dev/null 2>/dev/null";
	push @cmds, $cmd; 
}
my $out_sh = "$scriptdir/" . output_sh("keyframes.sh");
output_array( $out_sh, \@cmds);
output_records( "$scriptdir/" . output_sh("keyframes_check.sh"), $out_sh);
foreach my $dir (keys %dirs){
	mkpath($dir);
}

=head
#gen object bank
@cmds = ();
%dirs = ();
my $objbank_dir = "$featdir/objbank";
my $objbank_dir_kf = "$objbank_dir/kf";
my $objbank_dir_max = "$objbank_dir/max";
my $objbank_dir_avg = "$objbank_dir/avg";
my $objbank_dir_record = "$objbank_dir/record";
my $objbank_script_dir = "$scriptdir/objbank";
my $objbank_models_path = "$objbank_dir/models_path";
mkpath($objbank_dir);
open(AA, "$sfepdir/objbank/models1000_cq_neg2000.list");
open(BB, ">$objbank_models_path");
while(<AA>){
	chomp;
	my $p = "$sfepdir/objbank/$_";
	if( !(-f $p) ){
		print STDERR "object bank model $p not found!!\n";
		exit -1;
	}
	print BB "$p\n";
}
close(BB);
close(AA);
mkpath("$scriptdir/objbank/chunks");
for(my $i = 0; $i < scalar(@kfs); $i+=$objectbank_chunk_size){
	my $objbank_chunk = "$scriptdir/objbank/chunks/$i.sh";
	open(OUT, ">$objbank_chunk");
	for(my $j = $i; $j < $i + $objectbank_chunk_size && $j < scalar(@kfs); $j++){
		my $ob_kf_out = change_prefix($kfs[$j], $kf_dir, $objbank_dir_kf, \%dirs, '.tar.gz', '.tar.gz');
		my $ob_max_out = change_prefix($kfs[$j], $kf_dir, $objbank_dir_max, \%dirs, '.tar.gz', '.max.gz');
		my $ob_avg_out = change_prefix($kfs[$j], $kf_dir, $objbank_dir_avg, \%dirs, '.tar.gz', '.avg.gz');
		print OUT "$kfs[$j] $ob_kf_out $ob_max_out $ob_avg_out\n";
	}
	close(OUT);
	my $record_out = change_prefix($objbank_chunk, "$scriptdir\/objbank\/chunks", $objbank_dir_record, \%dirs, '.sh', '.record');

	my $cmd = "($sfepdir/objbank/$machine/OBmain $objbank_models_path $objbank_chunk $tmpdir && touch $record_out) >/dev/null 2>/dev/null";
	push @cmds, $cmd; 
}
$out_sh = "$scriptdir/" . output_sh("objbank.sh");
output_array( $out_sh, \@cmds);
output_records( "$scriptdir/" . output_sh("objbank_check.sh"), $out_sh);
foreach my $dir (keys %dirs){
	mkpath($dir);
}
=cut

#MFCC
#Set up necessary paths for Janus
my $BINPATH = "$sfepdir/mfcc/bin";
my $JANUSROOT = "$sfepdir/mfcc/janus_code/$machine/";
my $precmd = "echo";
if( $machine eq "psc" ){
        $precmd = "source /usr/share/modules/init/bash && module load tcl && module load gcc && module load ATLAS && module load lapack";
}

my $janus_env = "export JANUSHOME=$JANUSROOT/janus && export JANUS_LIBRARY=\$JANUSHOME/library && export LD_LIBRARY_PATH=/opt/tcltk-8.5.10/lib/:$JANUSROOT/janus-libs/:\$LD_LIBRARY_PATH";
my $JANUS_SCRIPT=$JANUSROOT."/janus/janus";

my $janus_mfcc_script_dir = "$scriptdir/mfcc";
my $janus_mfcc_out_dir = "$featdir/mfcc/raw";
mkpath($janus_mfcc_out_dir);
my $janus_mfcc_record_dir = "$featdir/mfcc/record";
mkpath($janus_mfcc_record_dir);
@cmds = ();
mkpath($janus_mfcc_script_dir);
for(my $i = 0; $i < scalar(@vids); $i+=$mfcc_chunk_size){
	my $lst = "$janus_mfcc_script_dir/$i.lst";
	open(LST, ">".$lst);
	for(my $j = $i; $j < $i + $mfcc_chunk_size && $j < scalar(@vids); $j++){
		print LST $vids[$j]."\n";
	}
	close(LST);
	my $record_out = "$janus_mfcc_record_dir/mfcc.$i.record";
	push @cmds, "($janus_env && $precmd && $JANUS_SCRIPT $BINPATH/convert-v0.tcl $lst -featName FEAT -featDesc $BINPATH/featDesc_mfcc40 -featExt mfcc40 -outPath $janus_mfcc_out_dir && touch $record_out) > /dev/null 2>/dev/null";
}
$out_sh = "$scriptdir/" . output_sh("mfcc_extract.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("mfcc_extract_check.sh"), $out_sh);

my @mfcc_files = ();
my @cmd = ();
for(my $i = 0; $i < scalar(@vids); $i++){
	my $vid = $vids[$i];
	$vid =~ s/.*\///;
	$vid =~ s/\.mp4$/\.mfcc40.bz2/;
	my $record_out = "$janus_mfcc_record_dir/bzip.$i.record";
	push @cmd, "(bzip2 -d $janus_mfcc_out_dir/$vid && touch $record_out) >/dev/null 2>/dev/null";
	$vid =~ s/\.bz2$//;
	push @mfcc_files, "$janus_mfcc_out_dir/$vid";
}
$out_sh = "$scriptdir/" . output_sh("mfcc_decompress.sh");
output_array( $out_sh, \@cmd );
output_records( "$scriptdir/" . output_sh("mfcc_decompress_check.sh"), $out_sh);

my @mfccs = (
		[("mfcc4k", "bin", "clusterCenters/selected.mfcc.11746.v02.csv.4096.cluster_centres", 4096)],
		[("mfcc16kreduced", "bin", "clusterCenters/reduced.vocab", 4096)],
		[("mfcc8kstack5", "Stack5", "clusterCenters/selectedMFCC_5_stack5_2000_8K.csv.cluster_centres", "8192")]
	);

#for(my $i = 0; $i < scalar(@mfccs); $i++){
for(my $i = 0; $i < 1; $i++){
	my $name = $mfccs[$i][0];
	my $jar_dir = "$sfepdir/mfcc/".$mfccs[$i][1];
	my $centroids = "$sfepdir/mfcc/".$mfccs[$i][2];
	my $centroids_count = $mfccs[$i][3];
	my $out_dir_txyc = "$featdir/mfcc/$name/txyc";
	my $out_dir_bof = "$featdir/mfcc/$name/bof";
	my $out_dir_efm = "$featdir/mfcc/$name/efm";
	my $fv_dir = "$featdir/mfcc/$name/fv";
	my @linear_vectors = ();
	my @fv_list = ();
	mkpath($out_dir_txyc);
	mkpath($out_dir_bof);
	mkpath($out_dir_efm);
	mkpath($fv_dir);
	@cmds = ();
	for(my $j = 0; $j < scalar(@mfcc_files); $j++){
		my $v = $mfcc_files[$j];
		$v =~ s/.*\///;
		 #$v =~ s/\..*//; # if path has . or video name has more ., then will XD
         $v =~ s/\.[^\.]*$//;

		my $record_out = "$janus_mfcc_record_dir/$name.$v.record";
		my $cmd = "(java -Xmx2G -cp $jar_dir/bof.jar txyc $centroids $mfcc_files[$j] 10 $out_dir_txyc";
    		$cmd .= " && java -Xmx2G -cp $jar_dir/bof.jar bof $out_dir_txyc/$v.txyc $centroids_count 10 $out_dir_bof";
		$cmd .= " && (set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $out_dir_bof/$v.bof | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $out_dir_efm/$v.bof)";
		$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
		push @cmds, $cmd;
		push @linear_vectors, "$out_dir_efm/$v.bof";
	}
	$out_sh = "$scriptdir/" . output_sh("$name"."_bof_extract.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("$name"."_bof_extract_check.sh"), $out_sh);
	gen_PQ("$name"."_efm", \@linear_vectors, $janus_mfcc_record_dir);	
	if( $name eq "mfcc4k" ){
		gen_PQ("mfcc_fv", \@fv_list, $janus_mfcc_record_dir);	
	}
}

exit;

%dirs = ();
#gen_fisher("mfcc", \@mfcc_files, $janus_mfcc_out_dir, \%dirs, $janus_mfcc_record_dir);
#my $out_sh_gmm = gen_gmm("mfcc", \@mfcc_files, $janus_mfcc_out_dir, \%dirs, $janus_mfcc_record_dir);
#output_records( "$scriptdir/" . output_sh("mfcc_gmm_check.sh"), $out_sh_gmm );
foreach my $dir (keys %dirs){
	mkpath($dir);
}




my $janus_lmel_script_dir = "$scriptdir/lmel";
my $janus_lmel_raw_dir = "$featdir/lmel/raw";
mkpath($janus_lmel_raw_dir);
my $janus_lmel_bof_dir = "$featdir/lmel/bof";
mkpath($janus_lmel_bof_dir);
my $janus_lmel_efm_dir = "$featdir/lmel/bof_efm";
mkpath($janus_lmel_efm_dir);
my $janus_lmel_record_dir = "$featdir/lmel/record";
mkpath($janus_lmel_record_dir);
@cmds = ();
my @lmel_csv_output = ();
mkpath($janus_lmel_script_dir);
for(my $i = 0; $i < scalar(@vids); $i+=$lmel_chunk_size){
	my $lst = "$janus_lmel_script_dir/$i.lst";
	my $lmel_lst = "$janus_lmel_script_dir/$i.lmel";
	open(LST, ">".$lst);
	open(LMEL_LST, ">".$lmel_lst);
	for(my $j = $i; $j < $i + $lmel_chunk_size && $j < scalar(@vids); $j++){
		print LST $vids[$j]."\n";
		my $lmel = $vids[$j];
		$lmel =~ s/.*\///;
		$lmel =~ s/\..*/\.lmel60.bz2/;
		print LMEL_LST "$janus_lmel_raw_dir/$lmel\n";
		$lmel =~ s/\..*/\.csv/;
		push @lmel_csv_output, "$janus_lmel_raw_dir/$lmel";
	}
	close(LST);
	close(LMEL_LST);
	my $record_out = "$janus_lmel_record_dir/lmel.$i.record";
	push @cmds, "($janus_env && $precmd && $JANUS_SCRIPT $BINPATH/convert-v0.tcl $lst -featName FEAT -featDesc $BINPATH/featDesc_lmel60 -featExt lmel60 -outPath $janus_lmel_raw_dir && export PYTHONPATH=$pythonpath && $python27 $sfepdir/LMEL/bin/generateNormSparseDAFeature.py $sfepdir/LMEL/model/sparse_lmel_210_60_0.02_1.0.json_trained.json $lmel_lst $janus_lmel_raw_dir && touch $record_out) > /dev/null 2>/dev/null";
}
$out_sh = "$scriptdir/" . output_sh("lmel_extract.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("lmel_extract_check.sh"), $out_sh);

@cmds = ();
my $lmel_jar_dir = "$sfepdir/mfcc/bin";
my $lmel_centroids = "$sfepdir/LMEL/Codebooks/clusters.sparseAE.210_60_norm_mv.8000";
my $lmel_centroids_count = 8000;
my @linear_vectors = ();
for(my $j = 0; $j < scalar(@lmel_csv_output); $j++){
	my $vid = $lmel_csv_output[$j];
	my $cmd = "(java -Xmx2G -cp $lmel_jar_dir/bof.jar txyc $lmel_centroids $vid 10 $janus_lmel_raw_dir";
	$vid =~ s/.*\///;
	$vid =~ s/\..*/\.txyc/;
	my $bof = $vid;
	$bof =~ s/\.txyc/\.bof/;
	my $record_out = "$janus_lmel_record_dir/lmel_bof.$j.record";
	$cmd .= " && java -Xmx2G -cp $lmel_jar_dir/bof.jar bof $janus_lmel_raw_dir/$vid $lmel_centroids_count 10 $janus_lmel_bof_dir";
	$cmd .= " && (set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $janus_lmel_bof_dir/$bof | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $janus_lmel_efm_dir/$bof)";
	$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
	push @cmds, $cmd;
	push @linear_vectors, "$janus_lmel_efm_dir/$bof";
}
$out_sh = "$scriptdir/" . output_sh("lmel_bof_extract.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("lmel_bof_extract_check.sh"), $out_sh);
gen_PQ("LMEL_efm", \@linear_vectors, $janus_lmel_record_dir);	




#noiseme
my $noiseme_script_dir = "$scriptdir/noisemes";
my $noiseme_feature_dir = "$featdir/noisemes";
my $noiseme_record_dir = "$noiseme_feature_dir/record";
mkpath($noiseme_script_dir);
mkpath($noiseme_record_dir);
#gen wav files
my $wav_dir = "$noiseme_feature_dir/wav";	
mkpath($wav_dir);
@cmds = ();
my $record_out = "$noiseme_record_dir/wav.record";
my $cmd = "($janus_env && $sfepdir/mfcc/janus_code/$machine/janus/janus $sfepdir/mfcc/bin/mp42wav.tcl $prefix $wav_dir";
$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
push @cmds, $cmd;
$out_sh = "$scriptdir/" . output_sh("wav_gen.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("wav_gen_check.sh"), $out_sh);

#extract large scale
my $largescale_selected_dir = "$noiseme_feature_dir/largescale_selected";
mkpath($largescale_selected_dir);
@cmds = ();
for(my $i = 0; $i < scalar(@vids); $i += $noiseme_chunk_size){

	my $out_list = "$noiseme_script_dir/extractLS_$i.list";
	my $wav_list = "$noiseme_script_dir/extractLS_$i.wav.list";
	my $out_script = "$noiseme_script_dir/extractLS_$i.sh";
	open(NOSIOUT, ">$out_list");
	open(WAVOUT, ">$wav_list");
	for(my $j = $i; $j < $i + $noiseme_chunk_size && $j < scalar(@vids); $j++){
		my $v = $vids[$j];
		$v =~ s/.*\///;
		$v =~ s/\..*//;
		print NOSIOUT "$v\n";
		print WAVOUT "$wav_dir/$v.wav\n";
	}
	close(NOSIOUT);
	close(WAVOUT);	

	my $record_out = "$noiseme_record_dir/largescale.$i.record";
	#my $cmd = "(export PYTHONPATH=$pythonpath && $python27 $sfepdir/LargeScaleAudioPoolingFeatures/bin/extractLSPFeatures.py $wav_list $sfepdir/LargeScaleAudioPoolingFeatures/config/OpenSmile/MED_2s_100ms_csv.conf $sfepdir/LargeScaleAudioPoolingFeatures/config/Normalization/mv.dat $sfepdir/LargeScaleAudioPoolingFeatures/config/PCA/latent.csv $sfepdir/LargeScaleAudioPoolingFeatures/config/PCA/pcomps.csv $sfepdir/LargeScaleAudioPoolingFeatures/config/FeatureSelection/keeper5500 $noiseme_feature_dir $sfepdir/asr_noisemes/libraries/openSmile/bin/SMILExtract --comps 100 300 --save True";
	my $cmd = "(export PYTHONPATH=$pythonpath && $python27 $sfepdir/LargeScaleAudioPoolingFeatures/bin/extractLSPFeatures-zip.py $wav_list $sfepdir/LargeScaleAudioPoolingFeatures/config/OpenSmile/MED_2s_100ms_csv.conf $sfepdir/LargeScaleAudioPoolingFeatures/config/Normalization/mv.dat $sfepdir/LargeScaleAudioPoolingFeatures/config/PCA/latent.csv $sfepdir/LargeScaleAudioPoolingFeatures/config/PCA/pcomps.csv $sfepdir/LargeScaleAudioPoolingFeatures/config/FeatureSelection/keeper5500 $noiseme_feature_dir $sfepdir/asr_noisemes/libraries/openSmile/bin/SMILExtract --comps 100 300";
	#$cmd .= " && $python27 $sfepdir/asr_noisemes/noisemes/large_scale_feature_extraction/selectFtr.py $sfepdir/asr_noisemes/noisemes/large_scale_feature_extraction/mergebaseANDmfcc_manual.ftr $out_list $noiseme_feature_dir/FeatureExtraction $largescale_selected_dir";
	$cmd .= " && $python27 $sfepdir/asr_noisemes/noisemes/large_scale_feature_extraction/selectFtr-zip.py $sfepdir/asr_noisemes/noisemes/large_scale_feature_extraction/mergebaseANDmfcc_manual.ftr $out_list $noiseme_feature_dir/FeatureExtraction $largescale_selected_dir";
	$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
	push @cmds, $cmd;
}
$out_sh = "$scriptdir/" . output_sh("largescale_gen.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("largescale_gen_check.sh"), $out_sh);


my @lsf = (
		[("pca100", "codebooks/selected.LS5500.PCA100.8p.csv.8k.cluster_centres", "LSF_2sw_100ms_pca100")],
		[("pca300", "codebooks/selected.8p.LS.5500.PCA300.8000.cluster_centres", "LSF_2sw_100ms_pca300")],
	);
for(my $i = 0; $i < scalar(@lsf); $i++){
	my $name = $lsf[$i][0];
	my $jar_dir = "$sfepdir/LargeScaleAudioPoolingFeatures/bin/";
	my $centroids = "$sfepdir/LargeScaleAudioPoolingFeatures/$lsf[$i][1]";
	my $centroids_count = 8000;
	my $out_dir_txyc = "$featdir/lsf/$name/txyc";
	my $out_dir_bof = "$featdir/lsf/$name/bof";
	my $out_dir_efm = "$featdir/lsf/$name/efm";
	mkpath($out_dir_txyc);
	mkpath($out_dir_bof);
	mkpath($out_dir_efm);
	my $record_dir = "$featdir/lsf/$name.record";
	mkpath($record_dir);
	my @cmds = ();
	my @linear_vectors = ();
	for(my $j = 0; $j < scalar(@vids); $j++){
		my $v = $vids[$j];
		$v =~ s/.*\///;
		$v =~ s/\..*//;
		my $record_out = "$record_dir/$v.record";
		my $cmd = "(java -Xmx2G -cp $jar_dir/bof.jar txyc $centroids $noiseme_feature_dir/FeaturePCA/$v.$name 10 $out_dir_txyc";
    		$cmd .= " && java -Xmx2G -cp $jar_dir/bof.jar bof $out_dir_txyc/$v.txyc $centroids_count 10 $out_dir_bof";
		$cmd .= " && (set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $out_dir_bof/$v.bof | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $out_dir_efm/$v.bof)";
		$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
		push @cmds, $cmd;
		push @linear_vectors, "$out_dir_efm/$v.bof";
	}
	$out_sh = "$scriptdir/" . output_sh("$name"."_bof_extract.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("$name"."_bof_extract_check.sh"), $out_sh);
	gen_PQ($lsf[$i][2]."_efm", \@linear_vectors, $record_dir);	
}


my $largescale_rescaled_dir = "$noiseme_feature_dir/largescale_rescaled";
mkpath($largescale_rescaled_dir);
@cmds = ();
for(my $i = 0; $i < scalar(@vids); $i ++){
	my $vid_name = $vids[$i];
	$vid_name =~ s/.*\///;
	$vid_name =~ s/\..*//;
	my $record_out = "$noiseme_record_dir/scale_$vid_name.record";
	my $cmd = "($python27 $sfepdir/asr_noisemes/noisemes/large_scale_feature_extraction/scale-zip.py $largescale_selected_dir/$vid_name.mat $largescale_rescaled_dir/$vid_name.mat $sfepdir/asr_noisemes/noisemes/large_scale_feature_extraction/mergeANDmfcc.scale && touch $record_out) >/dev/null 2>/dev/null";
	push @cmds, $cmd;
}
$out_sh = "$scriptdir/" . output_sh("largescale_rescale.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("largescale_rescale_check.sh"), $out_sh);

refresh_path("$sfepdir/asr_noisemes/noisemes/noisemes.list", "$noiseme_script_dir/noisemes.list");
refresh_path("$sfepdir/asr_noisemes/noisemes/Sarnoff25.list", "$noiseme_script_dir/Sarnoff25.list");
refresh_path("$sfepdir/asr_noisemes/noisemes/SRI19.list", "$noiseme_script_dir/SRI19.list");
sub refresh_path{
	my ($in, $out) = @_;
	open(IN, $in);
	open(OUT, ">$out");
	while(<IN>){
		chomp;
		print OUT "$sfepdir/asr_noisemes/$_\n";
	}
	close(OUT);
	close(IN);
}

@cmds = ();
my $noisemes_predict_dir = "$noiseme_feature_dir/predict_noisemes";
my $sarnoff25_predict_dir = "$noiseme_feature_dir/predict_sarnoff25";
my $sri19_predict_dir = "$noiseme_feature_dir/predict_sri19";
mkpath($noisemes_predict_dir);
mkpath($sarnoff25_predict_dir);
mkpath($sri19_predict_dir);
for(my $i = 0; $i < scalar(@vids); $i += $noiseme_chunk_size){
	my $out_list = "$noiseme_script_dir/extractLS_$i.list"; #from before
	my $record_out = "$noiseme_record_dir/noiseme_predict.$i.record";
	my $cmd = "(export PYTHONPATH=$pythonpath && $python27 $sfepdir/asr_noisemes/noisemes/Predict/predict-zip.py $noiseme_script_dir/noisemes.list $out_list $largescale_rescaled_dir $noisemes_predict_dir";
	$cmd .= " && $python27 $sfepdir/asr_noisemes/noisemes/Predict/predict.py $noiseme_script_dir/Sarnoff25.list $out_list $largescale_rescaled_dir $sarnoff25_predict_dir";
	$cmd .= " && $python27 $sfepdir/asr_noisemes/noisemes/Predict/predict.py $noiseme_script_dir/SRI19.list $out_list $largescale_rescaled_dir $sri19_predict_dir";
	$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
	push @cmds, $cmd;
}
$out_sh = "$scriptdir/" . output_sh("noisemes_predict.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("noisemes_predict_check.sh"), $out_sh);

@cmds = ();
my $noisemes_bof_dir = "$noiseme_feature_dir/bof_noisemes";
my $sarnoff25_bof_dir = "$noiseme_feature_dir/bof_sarnoff25";
my $sri19_bof_dir = "$noiseme_feature_dir/bof_sri19";
mkpath($noisemes_bof_dir);
mkpath($sarnoff25_bof_dir);
mkpath($sri19_bof_dir);
for(my $i = 0; $i < scalar(@vids); $i ++){
	my $vid_name = $vids[$i];
	$vid_name =~ s/.*\///;
	$vid_name =~ s/\..*//;
	my $record_out = "$noiseme_record_dir/bof_$vid_name.record";
	my $cmd = "($python27 $sfepdir/asr_noisemes/noisemes/ExtractBof.py $noisemes_predict_dir/$vid_name.txt $noisemes_bof_dir ave 1";
	$cmd .= " && $python27 $sfepdir/asr_noisemes/noisemes/ExtractBof.py $sarnoff25_predict_dir/$vid_name.txt $sarnoff25_bof_dir ave 1";
	$cmd .= " && $python27 $sfepdir/asr_noisemes/noisemes/ExtractBof.py $sri19_predict_dir/$vid_name.txt $sri19_bof_dir ave 1";
	$cmd .= " && touch $record_out) >/dev/null 2>/dev/null";
	push @cmds, $cmd;
}
$out_sh = "$scriptdir/" . output_sh("noiseme_bof_gen.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("noiseme_bof_gen_check.sh"), $out_sh);





#emoICSI noisemes
my @emoICSI_noisemes = ("emotions", "ICSI");
for(my $i = 0; $i < scalar(@emoICSI_noisemes); $i++){
	my $lname = $emoICSI_noisemes[$i];
	my $largescale_rescaled_dir = "$noiseme_feature_dir/largescale_rescaled_$lname";
	mkpath($largescale_rescaled_dir);
	@cmds = ();
	for(my $i = 0; $i < scalar(@vids); $i ++){
		my $vid_name = $vids[$i];
		$vid_name =~ s/.*\///;
		$vid_name =~ s/\..*//;
		my $record_out = "$noiseme_record_dir/scale_$lname"."_$vid_name.record";
		my $cmd = "($python27 $sfepdir/asr_noisemes/noisemes/large_scale_feature_extraction/scale-zip.py $largescale_selected_dir/$vid_name.mat $largescale_rescaled_dir/$vid_name.mat $sfepdir/asr_noisemes/$lname/scaled.txt && touch $record_out) >/dev/null 2>/dev/null";
		push @cmds, $cmd;
	}
	$out_sh = "$scriptdir/" . output_sh("largescale_rescale_$lname.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("largescale_rescale_$lname"."_check.sh"), $out_sh);

	refresh_path("$sfepdir/asr_noisemes/$lname/models.list", "$noiseme_script_dir/$lname.list");

	@cmds = ();
	my $predict_dir = "$noiseme_feature_dir/predict_$lname";
	mkpath($predict_dir);
	for(my $i = 0; $i < scalar(@vids); $i += $noiseme_chunk_size){
		my $out_list = "$noiseme_script_dir/extractLS_$i.list"; #from before
		my $record_out = "$noiseme_record_dir/noiseme_predict_$lname.$i.record";
		my $cmd = "(export PYTHONPATH=$sfepdir/asr_noisemes/base_emoICSI/lib/python2.7/site-packages:$pythonpath && $python27 $sfepdir/asr_noisemes/noisemes/Predict/predict-zip.py $noiseme_script_dir/$lname.list $out_list $largescale_rescaled_dir $predict_dir";
		$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
		push @cmds, $cmd;
	}
	$out_sh = "$scriptdir/" . output_sh("$lname"."_predict.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("$lname"."_predict_check.sh"), $out_sh);

	@cmds = ();
	my $bof_dir = "$noiseme_feature_dir/bof_".$lname;
	mkpath($bof_dir);
	for(my $i = 0; $i < scalar(@vids); $i ++){
		my $vid_name = $vids[$i];
		$vid_name =~ s/.*\///;
		$vid_name =~ s/\..*//;
		my $record_out = "$noiseme_record_dir/bof_$vid_name"."_$lname.record";
		my $cmd = "($python27 $sfepdir/asr_noisemes/noisemes/ExtractBof.py $predict_dir/$vid_name.txt $bof_dir ave 1";
		$cmd .= " && touch $record_out) >/dev/null 2>/dev/null";
		push @cmds, $cmd;
	}
	$out_sh = "$scriptdir/" . output_sh("$lname"."_bof_gen.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("$lname"."_bof_gen_check.sh"), $out_sh);
}





my $asr_script_dir = "$scriptdir/asr";
mkpath($asr_script_dir);
my $asr_feature_dir = "$featdir/asr";
my $asr_record_dir = "$asr_feature_dir/record";
mkpath($asr_feature_dir);
mkpath($asr_record_dir);
my %to_replace = ();
$to_replace{ "/home/iyu/sfep" } = $sfepdir;
update_file("$sfepdir/asr_noisemes/srt_gen/hmmSeg.tcl", "$asr_script_dir/hmmSeg.tcl", \%to_replace);
update_file("$sfepdir/asr_noisemes/srt_gen/desc-v15.tcl", "$asr_script_dir/desc-v15.tcl", \%to_replace);

open(SRTLIST, ">$asr_script_dir/srt.list");
@cmds = ();
%to_replace = ();
$to_replace{ "/home/iyu/sfep/asr_noisemes/srt_gen" } = $asr_script_dir;
for(my $i = 0; $i < scalar(@vids); $i += $asr_chunk_size){
	my $out_list = "$asr_script_dir/$i.list";
	open(NOSIOUT, ">$out_list");
	for(my $j = $i; $j < $i + $asr_chunk_size && $j < scalar(@vids); $j++){
		my $v = $vids[$j];
		print NOSIOUT "$v\n";
		$v =~ s/.*\///;
		$v =~ s/\..*/\.srt/;
		print SRTLIST "$asr_feature_dir/hypos-$i/$v\n";
	}
	close(NOSIOUT);

	update_file("$sfepdir/asr_noisemes/srt_gen/test-hvc-v15.tcl", "$asr_script_dir/test-hvc-v15-$i.tcl", \%to_replace);
	mkpath("$asr_feature_dir/pem-v15-$i");
	my $record_out = "$asr_record_dir/gen_asr_srt.$i.record";
	my $cmd = "( cd $asr_feature_dir";
	$cmd .= " && $janus_env && $precmd && $JANUS_SCRIPT $asr_script_dir/hmmSeg.tcl -uemFile $out_list -lp 20 -pad 0.15 -outputFile pem-v15-$i/hvc -id hvc";
	$cmd .= " && $JANUS_SCRIPT $asr_script_dir/test-hvc-v15-$i.tcl";
	$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
	push @cmds, $cmd;
}
$out_sh = "$scriptdir/" . output_sh("asr_srt_gen.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("asr_srt_gen_check.sh"), $out_sh);
close(SRTLIST);

sub update_file{
	my ($in, $out, $map) = @_;
	open(IN, $in);
	open(OUT, ">$out");	
	while(<IN>){
		foreach my $a (keys %{$map}){
			my $val = $map->{$a};
			s/$a/$val/;
		}
		print OUT $_;
	}
	close(OUT);
	close(IN);	
}

#gen asr bof
my $asr_bof_dir = "$asr_feature_dir/asr_bof";	
mkpath($asr_bof_dir);
@cmds = ();
$record_out = "$asr_record_dir/gen_asr_bof.record";
$cmd = "($python27 $sfepdir/asr_noisemes/asr/code/ASRBof.py $asr_script_dir/srt.list $sfepdir/asr_noisemes/asr/ASR-med12.dict $asr_bof_dir";
$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
push @cmds, $cmd;
$out_sh = "$scriptdir/" . output_sh("asr_srt_bof_gen.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("asr_srt_bof_gen_check.sh"), $out_sh);





#gen asr bof
my $kaldi_srt_dir = "$asr_feature_dir/kaldi/srt";	
mkpath($kaldi_srt_dir);
my $kaldi_bof_dir = "$asr_feature_dir/kaldi/bof";	
mkpath($kaldi_bof_dir);
@cmds = ();
$record_out = "$asr_record_dir/gen_kaldi_bof.record";
my $srt_not_found_file = "$asr_feature_dir/kaldi/nfw";
execute("touch $srt_not_found_file");
my @srts = ();
my $kaldi_efm_dir = "$asr_feature_dir/kaldi/efm";
mkpath($kaldi_efm_dir);
my @cmds_kaldi_efm = ();
for(my $i = 0; $i < scalar(@vids); $i++){
	my $v = $vids[$i];
	$v =~ s/.*\///;
	$v =~ s/\..*//;
	push @srts, "$kaldi_srt_dir/$v.srt";
	my $cmd = "(set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $kaldi_bof_dir/$v.bof | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $kaldi_efm_dir/$v.bof && touch $asr_record_dir/$v.kaldi.efm.record) >/dev/null 2>/dev/null";
	push @linear_vectors, "$kaldi_efm_dir/$v.bof";
	push @cmds_kaldi_efm, $cmd;
}
my $kaldi_srt_list = "$asr_feature_dir/srt.list";
output_array($kaldi_srt_list, \@srts);
gen_sym_links("kaldi", \@srts, $kaldi_srt_dir, $srt_not_found_file);
@cmds = ();
$cmd = "($python27 $sfepdir/asr_noisemes/asr/code/ASRBof.py $kaldi_srt_list $sfepdir/asr_noisemes/asr_kaldi/dict $kaldi_bof_dir";
$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
push @cmds, $cmd;
$out_sh = "$scriptdir/" . output_sh("kaldi_srt_bof_gen.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("kaldi_srt_bof_gen_check.sh"), $out_sh);
$out_sh = "$scriptdir/" . output_sh("kaldi_efm.sh");
output_array( $out_sh, \@cmds_kaldi_efm );
output_records( "$scriptdir/" . output_sh("kaldi_efm_check.sh"), $out_sh);
gen_PQ("kaldi_decode_med30k_speech_tgpr_speech_w_marker_efm", \@linear_vectors, $asr_record_dir);	



my @asr_names = ("asr", "kaldi");
my @asr_dirs = ($asr_bof_dir, $kaldi_bof_dir);
my @asr_dicts = ("$sfepdir/asr_noisemes/asr/ASR-med12.dict", "$sfepdir/asr_noisemes/asr_kaldi/dict");
for(my $i = 0; $i < scalar(@asr_dirs); $i++){
	my $name = $asr_names[$i];
	my $asr_dir = $asr_dirs[$i];
	open(PATHS, ">$noiseme_script_dir/Path_$name-noiseme-Sarnoff-SRI-emoICSI.list");
	print PATHS "$noiseme_feature_dir/bof_emotions\n";
	print PATHS "$noiseme_feature_dir/bof_ICSI\n";
	print PATHS "$asr_dir\n";
	print PATHS "$noisemes_bof_dir\n";
	print PATHS "$sarnoff25_bof_dir\n";
	print PATHS "$sri19_bof_dir\n";
	close(PATHS);

	open(DICTS, ">$noiseme_script_dir/Dict_$name-noiseme-Sarnoff-SRI-emoICSI.list");
	print DICTS "$sfepdir/asr_noisemes/emotions/concepts\n";
	print DICTS "$sfepdir/asr_noisemes/ICSI/concepts\n";
	print DICTS "$asr_dicts[$i]\n";
	print DICTS "$sfepdir/asr_noisemes/merged/2smainPredict.list\n";
	print DICTS "$sfepdir/asr_noisemes/merged/Sarnoff25-predict.list\n";
	print DICTS "$sfepdir/asr_noisemes/merged/SRI19-predict.list\n";
	close(DICTS);

	my $asr_noiseme_merged_dir = "$asr_feature_dir/$name"."_noisemes";
	mkpath($asr_noiseme_merged_dir);
	@cmds = ();
	for(my $i = 0; $i < scalar(@vids); $i += $noiseme_chunk_size){
		my $out_list = "$noiseme_script_dir/extractLS_$i.list"; #from before
			my $record_out = "$noiseme_record_dir/$name"."_noiseme_merge.$i.record";
		my $cmd = "($python27 $sfepdir/asr_noisemes/merged/CombineSemantic.py $noiseme_script_dir/Path_$name-noiseme-Sarnoff-SRI-emoICSI.list $noiseme_script_dir/Dict_$name-noiseme-Sarnoff-SRI-emoICSI.list $out_list $asr_noiseme_merged_dir $asr_feature_dir/$name"."_merged_dict";
		$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
		push @cmds, $cmd;
	}
	$out_sh = "$scriptdir/" . output_sh("merge_$name"."_noisemes.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("merge_$name"."_noisemes_check.sh"), $out_sh);

	my $asr_noiseme_merged_efm_dir = "$asr_feature_dir/$name"."_noisemes_efm";
	mkpath($asr_noiseme_merged_efm_dir);
	@cmds = ();
	for(my $i = 0; $i < scalar(@mfcc_files); $i++){
		my $v = $mfcc_files[$i];
		$v =~ s/.*\///;
		$v =~ s/\..*//;
		my $cmd = "(set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $asr_noiseme_merged_dir/$v.bof | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $asr_noiseme_merged_efm_dir/$v.bof && touch $noiseme_record_dir/$v.efm.record) >/dev/null 2>/dev/null";
		push @cmds, $cmd;
	}
	$out_sh = "$scriptdir/" . output_sh("$name"."_noisemes_efm.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("$name"."_noisemes_efm_check.sh"), $out_sh);
	gen_PQ("$name"."_noiseme-Sarnoff-SRI-emoICSI_efm", \@linear_vectors, $noiseme_record_dir);	
}





#AUDs opening
my $auds_dir = "$featdir/auds";
execute("mkdir -p $auds_dir/bof"); #auds1024 features go in bof
my $auds_efm_dir = "$auds_dir/efm";
execute("mkdir -p $auds_efm_dir");
my $auds_record_dir = "$auds_dir/record";
execute("mkdir -p $auds_record_dir");
my @bofs = ();
@cmds = ();
@linear_vectors = ();
for(my $i = 0; $i < scalar(@vids); $i++){
	my $v = $vids[$i];
	$v =~ s/.*\///;
	$v =~ s/\..*//;
	my $cmd = "(set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $auds_dir/bof/$v.spbof | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $auds_efm_dir/$v.bof && touch $auds_record_dir/$v.efm.record) >/dev/null 2>/dev/null";
	push @linear_vectors, "$auds_efm_dir/$v.bof";
	push @cmds, $cmd;
	push @bofs, "$auds_dir/bof/$v.spbof";
}
my $auds_not_found_file = "$auds_dir/not_found";
open(NF, ">$auds_not_found_file");
my $val = sprintf("%g", 1/1024);
for(my $i = 1; $i <= 1024; $i++){
	print NF "$i:$val ";
}
print NF "\n";
close(NF);
gen_sym_links("auds", \@bofs, $auds_dir, $auds_not_found_file);
#gen_sym_links("auds", \@bofs, $auds_dir, $linear_not_found_vector);
$out_sh = "$scriptdir/" . output_sh("auds_efm.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("auds_efm_check.sh"), $out_sh);
gen_PQ("auds1024_efm", \@linear_vectors, $auds_record_dir);	

sub gen_sym_links{
	my ($name, $files, $tp_dir, $not_found) = @_;
	my @bofs = @{ $files };
	open(OUT, ">$tp_dir/$name.sym_list");
	for(my $i = 0; $i < scalar(@bofs); $i++){
		print OUT "$bofs[$i]\n";
	}
	$out_sh = "$scriptdir/" . output_sh("$name"."_gensym.sh");
	my @cmds = ();
	push @cmds, "if [ \$# -eq 0 ]";
	push @cmds, "  then";
	push @cmds, "  echo \"please provide file list\"";
	push @cmds, "  exit -1";
	push @cmds, "fi";
	push @cmds, "perl $sfepdir/gencmds/list2symlink.pl \$1 $tp_dir/$name.sym_list $not_found";
	output_array( $out_sh, \@cmds );
}


=head
my $SBPCA_script_dir = "$scriptdir/SBPCA";
my $SBPCA_feature_dir = "$featdir/SBPCA";
my $SBPCA_record_dir = "$SBPCA_feature_dir/record";
mkpath($SBPCA_script_dir);
mkpath($SBPCA_feature_dir);
mkpath($SBPCA_record_dir);
my $SBPCA_feature_txt_dir = "$featdir/SBPCA/txt";
mkpath($SBPCA_feature_txt_dir);
my $SBPCA_feature_bof_dir = "$featdir/SBPCA/bof";
mkpath($SBPCA_feature_bof_dir);
@cmds = ();
for(my $i = 0; $i < scalar(@vids); $i += $SBPCA_chunk_size){
	open(LST, ">$SBPCA_script_dir/wav.$i.lst");
	open(LSTSB, ">$SBPCA_script_dir/SBPCA.$i.lst");
	for(my $j = $i; $j < $i + $SBPCA_chunk_size && $j < scalar(@vids); $j++){
		my $name = $vids[$j];
		$name =~ s/.*\///;
		$name =~ s/\..*//;
		print LST "$wav_dir/$name.wav\n";
		print LSTSB "$SBPCA_feature_txt_dir/$name.txt\n";
	}
	close(LST);
	close(LSTSB);

	my $record_out = "$SBPCA_record_dir/$i.record";
	my $cmd = "($python27 $sfepdir/SBPCA/bin/createVideoMap.py $SBPCA_script_dir/wav.$i.lst $SBPCA_feature_txt_dir > $SBPCA_script_dir/video.$i.map";
	$cmd .= " && /opt/matlab/7.12/bin/matlab -nojvm -nodesktop -nosplash -r \"cd $sfepdir/SBPCA/calcSBPCA-v0.1; calcSBPCA $SBPCA_script_dir/video.$i.map $sfepdir/SBPCA/config/config.2s.2s 'output_format txt'; exit\"";
	$cmd .= " && export PYTHONPATH=$pythonpath && $python27 $sfepdir/SBPCA/bin/genBOF.py --dimension 4000 $SBPCA_script_dir/SBPCA.$i.lst $SBPCA_feature_bof_dir";
	$cmd .= " && touch $record_out) > /dev/null 2>/dev/null";
	push @cmds, $cmd;
}
$out_sh = "$scriptdir/" . output_sh("SBPCA.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("SBPCA_check.sh"), $out_sh);
=cut





if( ! defined $params{"video_info"} ){
	print STDERR "please provide video_info in config_file so that I can generate commands for feature extraction\n";
	exit -1;
}
$cmd_id = 100;
open(VIDINFO, $params{"video_info"});
my %video_width = ();
my %video_height = ();
while( <VIDINFO> ){
	chomp;
	my @arr = split / /, $_;
	$arr[0] =~ s/.*\///;
	$arr[0] =~ s/\..*$//;
	$video_width{$arr[0]} = $arr[1];
	$video_height{$arr[0]} = $arr[2];
}
close(VIDINFO);



#gen sift, csift, tch
my @siftfeats = ("sift", "csift", "tch");
my @dim = (128, 384, 45);
%dirs = ();
for(my $i = 0; $i < scalar(@siftfeats); $i++){
	@cmds = ();
	my @raw = ();
	my @linear_vectors = ();
	my @fv_list = ();
	my $feat_dir = "$tmpdir/$siftfeats[$i]/raw";
	my $txyc_dir = "$tmpdir/$siftfeats[$i]/txyc";
	my $spbof_dir = "$featdir/$siftfeats[$i]/spbof";
	my $spbofmerged_dir = "$featdir/$siftfeats[$i]/spbofmerged";
	my $spbofmerged_efm_dir = "$featdir/$siftfeats[$i]/spbofmerged_efm";
	my $fv_dir = "$featdir/$siftfeats[$i]/fv";
	my $record_dir = "$featdir/$siftfeats[$i]/record";
	my $desc = $siftfeats[$i];
	if( $desc eq "tch" ){
		$desc = "transformedcolorhistogram";
	}
	for(my $j = 0; $j < scalar(@kfs); $j++){
		my $vidname = $kfs[$j];
		$vidname =~ s/.*\///;
		$vidname =~ s/\..*//;
		my $feat_out = change_prefix($kfs[$j], $kf_dir, $feat_dir, \%dirs, '.tar.gz', '.tar.gz');
		my $txyc_out = change_prefix($kfs[$j], $kf_dir, $txyc_dir, \%dirs, '.tar.gz', '.tar.gz');
		my $spbof_out = change_prefix($kfs[$j], $kf_dir, $spbof_dir, \%dirs, '.tar.gz', '.tar.gz');
		my $spbofmerged_out = change_prefix($kfs[$j], $kf_dir, $spbofmerged_dir, \%dirs, '.tar.gz', '.spbof');
		my $spbofmerged_efm_out = change_prefix($kfs[$j], $kf_dir, $spbofmerged_efm_dir, \%dirs, '.tar.gz', '.spbof');
		my $fv_out = change_prefix($kfs[$j], $kf_dir, $fv_dir, \%dirs, '.tar.gz', '.fv');
		my $record_out = change_prefix($kfs[$j], $kf_dir, $record_dir, \%dirs, '.tar.gz', '.record');
		my $w = $video_width{$vidname};
		my $h = $video_height{$vidname};
		if( ! defined $w ){
			$w = "";
			$h = "";
		}
		my $cmd = "(mkdir -p $feat_dir && mkdir -p $txyc_dir && $ramdisk_prefix $colorprefix/$machine/ColorSIFTWrapper $colorprefix/shared/colorDescriptor $kfs[$j] $feat_out harrislaplace $desc $siftfeats[$i]";
		$cmd .= " && $colorprefix/$machine/TxycWrapper $colorprefix/$machine/txyc $colorprefix/kmeans/$siftfeats[$i].center $feat_out $txyc_out $siftfeats[$i] txyc 4096 10 $dim[$i]";
		$cmd .= " && $colorprefix/$machine/SpbofWrapper $colorprefix/$machine/spbof $txyc_out $spbof_out txyc spbof 4096 10 1 $w $h";
		$cmd .= " && $colorprefix/$machine/BofMerger $spbof_out $spbofmerged_out spbof";
		$cmd .= " && (set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $spbofmerged_out | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $spbofmerged_efm_out)";
		my $codebook_path = "ColorDescriptors/codebook/$siftfeats[$i]";
		my @ar = @{ $fisher{$siftfeats[$i]} };
		$cmd .= " && $sfepdir/fisher/cpp_version/bin/fisher $feat_out $fv_out $w $h $siftfeats[$i] $ar[2] $ar[3] $sfepdir/fisher/$codebook_path/PCA$ar[0]_MaxFeat100000_GmmCtr$ar[1]_GmmInitkmeans/codebook.txt $sfepdir/fisher/$codebook_path/PCA$ar[0]_MaxFeat100000_GmmCtr$ar[1]_GmmInitkmeans/pca_proj.txt";
		$cmd .= " && rm -f $feat_out $txyc_out && touch $record_out) >/dev/null 2>/dev/null";
		#$cmd .= " && touch $record_out) >/dev/null 2>/dev/null";
		push @cmds, $cmd;
		#push @raw, $feat_out;
		push @linear_vectors, $spbofmerged_efm_out;
		push @fv_list, $fv_out.".gz";
	}

	$out_sh =  "$scriptdir/" . output_sh("$siftfeats[$i]_extract.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("$siftfeats[$i]_check.sh"), $out_sh);

	gen_PQ($siftfeats[$i]."_spbof_efm", \@linear_vectors, $record_dir);	
	gen_PQ($siftfeats[$i]."_fv", \@fv_list, $record_dir);

	#gen_fisher($siftfeats[$i], \@raw, $feat_dir, \%dirs, $record_dir);
#my $out_sh_gmm = gen_gmm($siftfeats[$i], \@raw, $feat_dir, \%dirs, $record_dir);
#output_records( "$scriptdir/" . output_sh("$siftfeats[$i]_gmm_check.sh"), $out_sh_gmm );
	foreach my $dir (keys %dirs){
		if( !($dir =~ /\$SCRATCH_RAMDISK/) ){
			mkpath($dir);
		}
	}
}






if( ! defined $params{"video_info_resized"} ){
	print STDERR "please provide video_info_resized in config_file so that I can generate commands for feature extraction\n";
	exit -1;
}
$cmd_id = 200;
open(VIDINFO, $params{"video_info_resized"});
my %video_resized_width = ();
my %video_resized_height = ();
while( <VIDINFO> ){
	chomp;
	my @arr = split / /, $_;
	$arr[0] =~ s/.*\///;
	$arr[0] =~ s/\..*$//;
	$video_resized_width{$arr[0]} = $arr[1];
	$video_resized_height{$arr[0]} = $arr[2];
}
close(VIDINFO);

#gen mosift, stip, trajectory
my @motionfeats = ("MoSIFT_resized", "stip", "stip_new", "trajectory");
my @centroids = ("kmeans/MED11_dev_mosift_resize_compress_kmeans_center", "kmeans/stip_MED11_centers", "kmeans/MED11_dev_STIP_danny_resize_compress_kmeans_center", "kmeans/MED11_dev_traj_not_light_resize_compress_kmeans_center");
my @exe = ("$machine/siftmotionffmpeg -r -t 1 -k 2 -z", "$machine/wrapper $sfepdir/stip/$machine/stip/stipdet", "$machine/wrapper $sfepdir/stip/$machine/stip/stipdet", "bin_not_light/DenseTrack_not_light");
%dirs = ();
for(my $i = 0; $i < scalar(@motionfeats); $i++){
	@cmds = ();
	my @raw = ();
	my @linear_vectors = ();
	my @fv_list = ();
	my $feat_dir = "$tmpdir/$motionfeats[$i]/raw";
	my $txyc_dir = "$tmpdir/$motionfeats[$i]/txyc";
	my $spbof_dir = "$featdir/$motionfeats[$i]/spbof";
	my $spbof_efm_dir = "$featdir/$motionfeats[$i]/spbof_efm";
	my $fv_dir = "$featdir/$motionfeats[$i]/fv";
	my $record_dir = "$featdir/$motionfeats[$i]/record";
	for(my $j = 0; $j < scalar(@vids_resized); $j++){
		my $vidname = $vids_resized[$j];
		$vidname =~ s/.*\///;
		$vidname =~ s/\..*//;
		my $vidpath = $vids_resized[$j];
		my $vids_dir = $resized_vids_dir;
		my $width = $video_resized_width{$vidname};
		my $height = $video_resized_height{$vidname};
		if( $motionfeats[$i] eq "stip" ){
			$vidpath = $vids[$j];
			$vids_dir = $prefix;
			$width = $video_width{$vidname};
			$height = $video_height{$vidname};
		}

		if( ! defined $width ){ #avoid var undef warnings.... just output a wrongly formatted command, will fail so ok
			$width = "";
			$height = "";
		}

		my $feat_out = change_prefix($vidpath, $vids_dir, $feat_dir, \%dirs, '.mp4', '.gz');
		my $txyc_out = change_prefix($vidpath, $vids_dir, $txyc_dir, \%dirs, '.mp4', '.txyc');
		my $spbof_out = change_prefix($vidpath, $vids_dir, $spbof_dir, \%dirs, '.mp4', '.spbof');
		my $spbof_efm_out = change_prefix($vidpath, $vids_dir, $spbof_efm_dir, \%dirs, '.mp4', '.spbof');
		my $fv_out = change_prefix($vidpath, $vids_dir, $fv_dir, \%dirs, '.mp4', '.fv');
		my $record_out = change_prefix($vidpath, $vids_dir, $record_dir, \%dirs, '.mp4', '.record');
		my $middle = "";
		if( $motionfeats[$i] eq "trajectory" ){
			$middle = " | gzip > "
		}
		my $cmd = "(mkdir -p " . getdir($feat_out) . " && mkdir -p " . getdir($txyc_out) . " && $ramdisk_prefix $sfepdir/$motionfeats[$i]/$exe[$i] $vidpath $middle $feat_out";
		#my $cmd = "(mkdir -p $feat_dir && mkdir -p $txyc_dir && $ramdisk_prefix $sfepdir/$motionfeats[$i]/$exe[$i] $vidpath $middle $feat_out";
		$cmd .= " && $sfepdir/$motionfeats[$i]/$machine/txyc $sfepdir/$motionfeats[$i]/$centroids[$i] 4096 $feat_out $txyc_out";
		$cmd .= " && $sfepdir/$motionfeats[$i]/$machine/spbof $txyc_out $width $height 4096 10 $spbof_out 1"; 
		$cmd .= " && (set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $spbof_out | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $spbof_efm_out)";
		my $codebook_path = $motionfeats[$i];
		my @ar = @{ $fisher{$motionfeats[$i]} };
		$cmd .= " && $sfepdir/fisher/cpp_version/bin/fisher $feat_out $fv_out $width $height $motionfeats[$i] $ar[2] $ar[3] $sfepdir/fisher/$codebook_path/PCA$ar[0]_MaxFeat100000_GmmCtr$ar[1]_GmmInitkmeans/codebook.txt $sfepdir/fisher/$codebook_path/PCA$ar[0]_MaxFeat100000_GmmCtr$ar[1]_GmmInitkmeans/pca_proj.txt";
		#$cmd .= " && rm -f $feat_out $txyc_out && touch $record_out) >/dev/null 2>/dev/null";
		$cmd .= " && touch $record_out) >/dev/null 2>/dev/null";
		push @cmds, $cmd;
		push @raw, $feat_out;
		push @linear_vectors, $spbof_efm_out;
		push @fv_list, $fv_out."\.gz";
	}
	$out_sh = "$scriptdir/" . output_sh("$motionfeats[$i]_extract.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("$motionfeats[$i]_check.sh"), $out_sh);
	gen_PQ($motionfeats[$i]."_spbof_efm", \@linear_vectors, $record_dir);	
	gen_PQ($motionfeats[$i]."_fv", \@fv_list, $record_dir);

	#gen_fisher($motionfeats[$i], \@raw, $feat_dir, \%dirs, $record_dir);
#output_records( "$scriptdir/" . output_sh("$motionfeats[$i]_fv_check.sh"), $out_sh_fish);
#if( $motionfeats[$i] ne "trajectory" ){
#my $out_sh_gmm = gen_gmm($motionfeats[$i], \@raw, $feat_dir, \%dirs, $record_dir);
#output_records( "$scriptdir/" . output_sh("$motionfeats[$i]_gmm_check.sh"), $out_sh_gmm);
#}

#HOG3D
#if( $i == 0 || $i == 2 ){ #i = 0 is mosift, i = 2 is stip_new
#	my $out_sh_hog3d = gen_hog3d($motionfeats[$i], \@vids_resized, \@raw, $feat_dir, \%dirs, $record_dir);
#	output_records( "$scriptdir/" . output_sh("$motionfeats[$i]_hog3d_check.sh"), $out_sh_hog3d);
#}

	foreach my $dir (keys %dirs){
		if( !($dir =~ /\$SCRATCH_RAMDISK/) ){
			mkpath($dir);
		}
	}

}

my @imtraj = (
	"imtraj_nospfv","gz",
	"imtraj_nospfv_skip1-3-6","gz",
	"imtraj_nospfv_skip1-3-6_256_xyt","gz"
);

my $imtraj_dir = "$featdir/imtraj";
my $imtraj_record_dir = "$imtraj_dir/record";
execute("mkdir -p $imtraj_record_dir");
for(my $i = 0; $i < scalar(@imtraj); $i+=2){
	my @linear_vectors = ();	
	for(my $j = 0; $j < scalar(@vids); $j++){
		my $p = $vids[$j];
		my $n = $p;
		$n =~ s/.*\///;
		$n =~ s/\..*//;
		push @linear_vectors, "$imtraj_dir/$imtraj[$i]/$n.$imtraj[$i+1]";
	}
	gen_sym_links($imtraj[$i], \@linear_vectors, $imtraj_dir, $linear_not_found_vector);
	gen_PQ($imtraj[$i], \@linear_vectors, $imtraj_record_dir);	

	if( $i == 0 ){ #original one
		my @names = ("diy", "sin346", "sports", "ucf101");
		my @fullnames = (
			"diy_scaled_decval_pooling",
			"sin346_scaled_decval_pooling",
			"sports_scaled_decval_pooling",
			"ucf101_scaled_decval"
		);
		my @models = ("$sfepdir/concepts/diy/diy_dtraj.liblinear.packedmodels","/home/iyu/sfep/concepts/sin346/sin346.liblinear.packedmodel","$sfepdir/concepts/sports/sports_dtraj.liblinear.packedmodels","$sfepdir/concepts/ucf101/ucf101.linear.packedmodels");
		my @bow_paths = ("scaled_decval_pooling", "scaled_decval_pooling", "scaled_decval_pooling", "scaled_decval");
		my $concepts_dir = "$scriptdir/concepts";
		mkpath($concepts_dir);
		my $feat_list = "$concepts_dir/feat_list";
		open(OO, ">$feat_list");
		for(my $j = 0; $j < scalar(@linear_vectors); $j++){
			print OO "$linear_vectors[$j]\n";
		}
		close(OO);
		for(my $j = 0; $j < scalar(@names); $j++){
			my $raw_dir = "$featdir/$names[$j]/raw";
			my $bof_dir = "$featdir/$names[$j]/bof";
			my $record_dir = "$featdir/$names[$j]/record";
			mkpath($raw_dir);
			mkpath($bof_dir);
			mkpath($record_dir);
			@cmds = ();
			my $cmd = "(java -Xmx30G -jar $sfepdir/concepts/multithread_predict.jar $models[$j] $feat_list $raw_dir 128 12";
			$cmd .= " && java -jar $sfepdir/concepts/bowgen.jar $raw_dir $sfepdir/concepts/$names[$j]/$names[$j].mapping $sfepdir/concepts/$names[$j]/decval.scale $bof_dir HVC && touch $record_dir/predict.record)  >/dev/null 2>/dev/null";
			push @cmds, $cmd;
			$out_sh = "$scriptdir/" . output_sh("$names[$j]_gen.sh");
			output_array( $out_sh, \@cmds );
			output_records( "$scriptdir/" . output_sh("$names[$j]_gen_check.sh"), $out_sh);

			my @concept_linear_vectors = ();
			for(my $k = 0; $k < scalar(@vids); $k++){
				my $v = $vids[$k];
				$v =~ s/.*\///;
				$v =~ s/\..*//;
				push @concept_linear_vectors, "$bof_dir/$bow_paths[$j]/$v.bow";
			}
			gen_PQ($fullnames[$j], \@concept_linear_vectors, $record_dir);	
		}	
	}
}



$cmd_id = 300;
my @paratroopers = (
	"bauds", 8192,
	"UC", 16384,
	"MED14_cnn_dogwood_3layer", 32744,
	"MED14_cnn_kangaroo_3layer", 23528,
	"MED14_cnn_koala_3layer", 32744,
	"MED14_cnn_model3_3layer", 36072,
	"MED14_cnn_model4_3layer", 36072,
	"ImageNet_model_w5_fc1_vavg", 4096,
	"ImageNet_model_w5_probs_vavg", 21842,
	"ImageNet_model_w5_probs_vmax", 21842,
	"ImageNet_model_w6_fc1_vavg", 4096,
	"ImageNet_model_w6_probs_vavg", 21842,
	"ImageNet_model_w6_probs_vmax", 21842,
	"ImageNet_model_w7_fc1_vavg", 4096,
	"ImageNet_model_w7_probs_vavg", 21842,
	"ImageNet_model_w7_probs_vmax", 21842,
	"MED14_cnn_koala_10views", 32744,
	"emo-ICSI", 19,
	"noiseme-Sarnoff-SRI", 84
);

for(my $k = 0; $k < scalar(@paratroopers); $k+=2){
	my $f = $paratroopers[$k];
	my $f_dir = "$featdir/$f";
	execute("mkdir -p $f_dir/bof"); 
	my $f_efm_dir = "$f_dir/efm";
	execute("mkdir -p $f_efm_dir");
	my $f_record_dir = "$f_dir/record";
	execute("mkdir -p $f_record_dir");
	my @bofs = ();
	@cmds = ();
	@linear_vectors = ();
	for(my $i = 0; $i < scalar(@vids); $i++){
		my $v = $vids[$i];
		$v =~ s/.*\///;
		$v =~ s/\..*//;
		my $cmd = "(set -o pipefail && export LD_LIBRARY_PATH=$sfepdir/explicit_feature_map/homo_map:\$LD_LIBRARY_PATH && cat $f_dir/bof/$v.bof | $sfepdir/explicit_feature_map/homo_map/homo_map_pipe 1 0.5 > $f_efm_dir/$v.bof && touch $f_record_dir/$v.efm.record) >/dev/null 2>/dev/null";
		push @linear_vectors, "$f_efm_dir/$v.bof";
		push @cmds, $cmd;
		push @bofs, "$f_dir/bof/$v.bof";
	}
	my $f_not_found_file = "$f_dir/not_found";
	open(NF, ">$f_not_found_file");
	my $val = sprintf("%g", 1/$paratroopers[$k+1]);
	for(my $i = 1; $i <= $paratroopers[$k+1]; $i++){
		print NF "$i:$val ";
	}
	print NF "\n";
	close(NF);
	gen_sym_links($f, \@bofs, $f_dir, $f_not_found_file);
	$out_sh = "$scriptdir/" . output_sh("$f"."_efm.sh");
	output_array( $out_sh, \@cmds );
	output_records( "$scriptdir/" . output_sh("$f"."_efm_check.sh"), $out_sh);
	gen_PQ($f."_efm", \@linear_vectors, $f_record_dir);	
}

#add paratrooper for concepts, no efm!
my @paratroopers_noefm = (
	"sin346_scaled_decval_pooling", "346",
        "sports_scaled_decval_pooling","478",
        "diy_scaled_decval_pooling", "1601",
        "ucf101_scaled_decval_pooling", "101",
	"yfcc609_scaled_decval_pooling", "609",
	"yfcc644_scaled_decval_pooling", "644"
);

for(my $k = 0; $k < scalar(@paratroopers_noefm); $k+=2){
	my $f = $paratroopers_noefm[$k];
	my $f_dir = "$featdir/$f";
	execute("mkdir -p $f_dir/bof"); 
	my $f_record_dir = "$f_dir/record";
	execute("mkdir -p $f_record_dir");
	@cmds = ();
	@linear_vectors = ();
	for(my $i = 0; $i < scalar(@vids); $i++){
		my $v = $vids[$i];
		$v =~ s/.*\///;
		$v =~ s/\..*//;
		push @linear_vectors, "$f_dir/bof/$v.bof";
	}
	close(NF);
	gen_sym_links($f, \@linear_vectors, $f_dir, $linear_not_found_vector);
	gen_PQ($f, \@linear_vectors, $f_record_dir);	
}





$cmd_id = 900;
@cmds = ();
open(PQFL, "$sfepdir/checker/PQ_features_list");
my $checker_dir = "$featdir/checker";
execute("mkdir -p $checker_dir");
my @check_vids = ("HVC389723", "HVC917458", "HVC874251", "HVC293016", "HVC244765");
my @lines = ();
for(my $i = 0; $i < scalar(@vids); $i++){
	my $v = $vids[$i];
	$v =~ s/.*\///;
	$v =~ s/\..*//;
	for(my $j = 0; $j < scalar(@check_vids); $j++){
		if( $check_vids[$j] eq $v ){
			$lines[$j] = $i;
		}
	}
}
for(my $i = 0; $i < scalar(@check_vids); $i++){
	if( ! defined $lines[$i] ){
		print STDERR "check video $check_vids[$i] not found\n";
		exit -1;
	}
}

while(<PQFL>){
	chomp;
	my @aaa = split / /, $_;
	my $f = $aaa[0];
	my $name = $f;
	my $lb = 4;
	my $ub = 8;
	if( $name =~ /ucf101/ || $name =~ /sin346/ || $name =~ /sports/ || $name =~ /diy/ || $name =~ /yfcc/ ){
		$lb = 1;
		$ub = 1;
	}
	for(my $i = $lb; $i <= $ub; $i += 4 ){
		my $cmd = "$sfepdir/PQ/code/PQ_predict $featdir/PQ/$f"."_$i.mapped $sfepdir/checker/PQ_model/$f"."_$i.EK100.E037.model $checker_dir/$f.E037.$i.predict 1 2>/dev/null 1>/dev/null";
		push @cmds, $cmd;
		push @cmds, "perl $sfepdir/checker/svm_predict_checker.pl $checker_dir/$f.E037.$i.predict $sfepdir/checker/PQ_predict/$f.E037.$i.predict ".join(",", @lines);
		push @cmds, "perl /home/iyu/MyCodes/lab/MED14_pipeline/sanity_checker/sc.pl $checker_dir/$f.E037.$i.predict E037";
		$cmd = "$sfepdir/PQ/code/PQ_predict $featdir/PQ/$f"."_$i.mapped $sfepdir/checker/PQ_model/$f"."_$i.EK100.E039.model $checker_dir/$f.E039.$i.predict 1 2>/dev/null 1>/dev/null";
		push @cmds, $cmd;
		push @cmds, "perl $sfepdir/checker/svm_predict_checker.pl $checker_dir/$f.E039.$i.predict $sfepdir/checker/PQ_predict/$f.E039.$i.predict ".join(",", @lines);
		push @cmds, "perl /home/iyu/MyCodes/lab/MED14_pipeline/sanity_checker/sc.pl $checker_dir/$f.E039.$i.predict E039";
		$cmd = "$sfepdir/PQ/code/PQ_predict $featdir/PQ/$f"."_$i.mapped $sfepdir/checker/PQ_model/$f"."_$i.EK100.E023.model $checker_dir/$f.E023.$i.predict 1 2>/dev/null 1>/dev/null";
		push @cmds, $cmd;
		push @cmds, "perl $sfepdir/checker/svm_predict_checker.pl $checker_dir/$f.E023.$i.predict $sfepdir/checker/PQ_predict/$f.E023.$i.predict ".join(",", @lines);
		push @cmds, "perl /home/iyu/MyCodes/lab/MED14_pipeline/sanity_checker/sc.pl $checker_dir/$f.E023.$i.predict E023";
	}
}
close(PQFL);
$out_sh = "$scriptdir/" . output_sh("PQ_checker.sh");
output_array( $out_sh, \@cmds );




@cmds = ();
my $eqgesdir = "$featdir/eqges";
my $filelistdir = "$eqgesdir/file_lists";
mkpath($filelistdir);
open(PQFL, "$sfepdir/checker/PQ_features_list");
while(<PQFL>){
	chomp;
	my @arr = split / /, $_;
	push @cmds, "find $featdir/$arr[1] | egrep \"\\.$arr[2]\$\" > $filelistdir/$arr[0].list";
}
close(PQFL);
$out_sh = "$scriptdir/" . output_sh("gen_filelists.sh");
output_array( $out_sh, \@cmds );

@cmds = ();
my $traindistdir = "$eqgesdir/training_distance";
my $concatfldir = "$eqgesdir/training_distance/fl";
my $recdir = "$traindistdir/record";
mkpath($concatfldir);
mkpath($recdir);
open(PQFL, "$sfepdir/checker/PQ_features_list");
while(<PQFL>){
	chomp;
	my @arr = split / /, $_;
	if( !(-f "/data/MM2/MED/labelsets/filelists/$arr[0].list") ){
		print STDERR "/data/MM2/MED/labelsets/filelists/$arr[0].list not found!!\n";
		next;
	}
	my $cmd = "cat $filelistdir/$arr[0].list /data/MM2/MED/labelsets/filelists/$arr[0].list >> $concatfldir/$arr[0].list";
	$cmd .= " && perl $sfepdir/gen_distance/splitTrainTestFileList.pl /data/MM21/iyu/MED14submissions/EAGEAE_krsvm/edu.cmu.lti.aladdin.eventagents/data/labels/MED14-MEDTEST-EK100/training.idx /data/MM21/iyu/MED14submissions/EAGEAE_krsvm/edu.cmu.lti.aladdin.eventagents/data/labels/MED14-MEDTEST-EK100/evl.idx $concatfldir/$arr[0].list $concatfldir/$arr[0].dev.list $concatfldir/$arr[0].evl.list $arr[3] $concatfldir/$arr[0].nfp LINEAR 2>$concatfldir/$arr[0].err"; #dimensions is fake! because linear kernel doesnot matter
	$cmd .= " && $sfepdir/gen_distance/compute_distance/chi2 $concatfldir/$arr[0].dev.list $traindistdir/$arr[0].dev.dist 12 $arr[3] LINEAR && touch $recdir/$arr[0].record";
	push @cmds, $cmd;
}
close(PQFL);
$out_sh = "$scriptdir/" . output_sh("gen_train_dists.sh");
output_array( $out_sh, \@cmds );
output_records( "$scriptdir/" . output_sh("gen_train_dists_check.sh"), $out_sh);


print STDERR "gencmds done\n";


sub getdir{
	my $path = shift;
	my $dir = $path;
	$path =~ s/.*\///;
	$dir =~ s/$path//;
	return $dir;
}

sub output_records{
	my ($fname, $cmds) = @_;
	open(FNAME, ">$fname");
	print FNAME "perl $sfepdir/gencmds/find_remaining.pl $cmds > $fname.remaining\n";
	close(FNAME);
}

sub gen_PQ{
	my ($name, $list, $recorddir) = @_;
	$name =~ s/MoSIFT_resized/mosift/;
	my @linear_vectors = @{$list};
	my $out_dir = "$featdir/$PQ_dir";
	execute("mkdir -p $out_dir");
	my $pq_conf_dir = "$scriptdir/$PQ_dir/";
	execute("mkdir -p $pq_conf_dir");
	my %map = ();
	for(my $i = 0; $i < scalar(@linear_vectors); $i++){
		my $name = $linear_vectors[$i];
		$name =~ s/.*\///;
		$name =~ s/\..*//;
		$map{ $name } = $linear_vectors[$i];
	}
	my @files = ();
#make sure order is right && no missing videos
	for(my $i = 0; $i < scalar(@vids); $i++){
		my $name = $vids[$i];
		$name =~ s/.*\///;
		$name =~ s/\..*//;
		my $path = $map{$name};
		if( ! defined $path ){
			$path = $linear_not_found_vector;
		}
		push @files, $path;
	}
	my $fl = "$pq_conf_dir/$name.list";
	output_array($fl, \@files);
	if( $name =~ /ucf101/ || $name =~ /sin346/ || $name =~ /sports/ || $name =~ /diy/ || $name =~ /yfcc/ ){
		my $cmd = "($sfepdir/PQ/code/PQ_mapper $fl $sfepdir/PQ/centroids/$name"."_256_1.bin $out_dir/$name"."_1.mapped $PQ_mapping_threads && touch $recorddir/PQ_$name"."_1.record) >/dev/null 2>/dev/null";
		my $out_sh = "$scriptdir/" . output_sh("PQ_$name"."_mapping.sh");
		my @arr = ();
		push @arr, $cmd;
		output_array($out_sh, \@arr);
		output_records( "$scriptdir/" . output_sh("PQ_$name"."_mapping_check.sh"), $out_sh );
	}else{
		my $cmd = "($sfepdir/PQ/code/PQ_mapper $fl $sfepdir/PQ/centroids/$name"."_256_4.bin $out_dir/$name"."_4.mapped $PQ_mapping_threads && touch $recorddir/PQ_$name"."_4.record) >/dev/null 2>/dev/null";
		my $out_sh = "$scriptdir/" . output_sh("PQ_$name"."_mapping.sh");
		my @arr = ();
		push @arr, $cmd;
		$cmd = "($sfepdir/PQ/code/PQ_mapper $fl $sfepdir/PQ/centroids/$name"."_256_8.bin $out_dir/$name"."_8.mapped $PQ_mapping_threads && touch $recorddir/PQ_$name"."_8.record) >/dev/null 2>/dev/null";
		push @arr, $cmd;
		output_array($out_sh, \@arr);
		output_records( "$scriptdir/" . output_sh("PQ_$name"."_mapping_check.sh"), $out_sh );
	}
}

sub gen_hog3d{
	my ($name, $resvidsptr, $rawptr, $feat_dir, $dirs, $record_dir) = @_;
	my @vids_resized = @{ $resvidsptr };
	my @raw = @{ $rawptr };
	my $hog_dir = "$featdir/$name/hog3d";
	my $raw_dir = "$hog_dir/raw";
	my $pos_dir = "$hog_dir/pos";
	my $txyc_dir = "$hog_dir/txyc";
	my $spbof_dir = "$hog_dir/spbof";
	my @hog3d_cmds = ();
	my $stipormosift = 0; #mosift
		if( $name eq "stip_new" ){
			$stipormosift = 1;
		}
	for(my $i = 0; $i < scalar(@raw); $i ++){
		my $vidname = $raw[$i];
		$vidname =~ s/.*\///;
		$vidname =~ s/\..*//;
		my $width = 0; #$ar[2] == -1 -> no spatial information
			my $height = 0; 
		$width = $video_resized_width{$vidname};
		$height = $video_resized_height{$vidname};

		my $raw_out = change_prefix($raw[$i], $feat_dir, $raw_dir, $dirs, '\..*', '.gz');
		my $pos_out = change_prefix($raw[$i], $feat_dir, $pos_dir, $dirs, '\..*', '.pos');
		my $txyc_out = change_prefix($raw[$i], $feat_dir, $txyc_dir, $dirs, '\..*', '.txyc');
		my $spbof_out = change_prefix($raw[$i], $feat_dir, $spbof_dir, $dirs, '\..*', '.spbof');
		my $record_out = change_prefix($raw[$i], $feat_dir, $record_dir, $dirs, '\..*', '.hog3d.record');

		my $cmd = "( python $sfepdir/HOG3D/src/pos.py $raw[$i] $pos_dir $stipormosift";
		$cmd .= " && $sfepdir/HOG3D/bin/extractFeatures_x86_64_v1.3.0 -p $pos_out $vids_resized[$i] | gzip > $raw_out ";
		$cmd .= " && $sfepdir/HOG3D/bin/HOG_des $sfepdir/HOG3D/kmeans/hog3d_des 4096 $raw_out $txyc_out";
		$cmd .= " && $sfepdir/stip/rocks/spbof $txyc_out $width $height 4096 10 $spbof_out 1";
		$cmd .= " && touch $record_out) >/dev/null 2>/dev/null";
		push @hog3d_cmds, $cmd;
	}
	my $out_sh = "$scriptdir/" . output_sh("hog3d_$name"."_extract.sh");
	output_array( $out_sh, \@hog3d_cmds );
	return $out_sh;
}

sub gen_gmm{
	my ($name, $rawptr, $feat_dir, $dirs, $record_dir) = @_;
	my @raw = @{ $rawptr };
	my @ar = @{ $gmm{$name} };
	my $gmm_dir = "$featdir/$name/gmm";
	my @gmm_cmds = ();
	for(my $i = 0; $i < scalar(@raw); $i ++){
		my $vidname = $raw[$i];
		$vidname =~ s/.*\///;
		$vidname =~ s/\..*//;
		my $width = 0; #$ar[2] == -1 -> no spatial information
			my $height = 0; 
		if( $ar[2] == 1 ){ #resized
			$width = $video_resized_width{$vidname};
			$height = $video_resized_height{$vidname};
		}elsif( $ar[2] == 0 ){ #noresized
			$width = $video_width{$vidname};
			$height = $video_height{$vidname};
		}

		my $gmm_out = change_prefix($raw[$i], $feat_dir, $gmm_dir, $dirs, '\..*', '.gmm.gz');

		my $codebook_path = $name;
		if( $name eq "sift" || $name eq "csift" || $name eq "tch" ){
			$codebook_path = "ColorDescriptors/$name";
		}
		my $record_out = change_prefix($raw[$i], $feat_dir, $record_dir, $dirs, '', '.gmm.record');
		my $cmd = "($sfepdir/gmm/code/bin/overwijk_gmm_mapping_direct $sfepdir/gmm/$codebook_path/UBM.$ar[0]d.summary $sfepdir/gmm/$codebook_path/sample_1M.pca $ar[0] $raw[$i] $ar[3] $width $height 20 $gmm_out && touch $record_out) >/dev/null 2>/dev/null"; #tau is 20

			push @gmm_cmds, $cmd;
	}
	my $out_sh = "$scriptdir/" . output_sh("gmm_$name"."_extract.sh");
	output_array( $out_sh, \@gmm_cmds );
	return $out_sh;
}

sub gen_fisher{
	my ($name, $rawptr, $feat_dir, $dirs, $record_dir) = @_;
	my @raw = @{ $rawptr };
	my @ar = @{ $fisher{$name} };
	my $fv_dir = "$featdir/$name/fv";
	my $fv_conf_dir = "$scriptdir/fv_$name"."_conf/";
	my @fv_cmds = ();
	mkpath($fv_conf_dir);
	my @confs = ();
	my @fv_list = ();
	for(my $i = 0; $i < scalar(@raw); $i += $fisher_chunk_size){
		my $conf = "$fv_conf_dir/fvconf.$i";
		push @confs, $conf;
		open(CONF, ">".$conf);
		for(my $j = $i; $j < $i + $fisher_chunk_size && $j < scalar(@raw); $j++){
			my $vidname = $raw[$j];
			$vidname =~ s/.*\///;
			$vidname =~ s/\..*//;
			my $width = 10000;
			my $height = 10000;
			if( $ar[4] == 1 ){
				$width = $video_resized_width{$vidname};
				$height = $video_resized_height{$vidname};
			}elsif( $ar[4] == 0 ){
				$width = $video_width{$vidname};
				$height = $video_height{$vidname};
			}

			my $fv_out = change_prefix($raw[$j], $feat_dir, $fv_dir, $dirs, $ar[5], $ar[6]);
			print CONF "$raw[$j] $fv_out $width $height\n";
			push @fv_list, $fv_out.".gz";
		}
		close(CONF);
#my $cmd = "/opt/matlab/7.12/bin/matlab -nodisplay -r \"addpath(genpath('$sfepdir/fisher/matlab')); FisherVectorEncode('$conf','$name','$sfepdir/fisher/$name/PCA60_MaxFeat100000_GmmCtr256_GmmInitkmeans/codebook.mat','PCA60_MaxFeat100000_GmmCtr256_GmmInitkmeans/pca_proj.mat',$pnorm,$alpha,'sum'); exit;\""
		my $record_out = change_prefix($conf, $fv_conf_dir, $record_dir, $dirs, '', '.fv.record');

		my $codebook_path = $name;
		if( $name eq "sift" || $name eq "csift" || $name eq "tch" ){
			$codebook_path = "ColorDescriptors/codebook/$name";
		}
#my $cmd = "/opt/matlab/7.12/bin/matlab -nodisplay -r \"addpath(genpath('$sfepdir/fisher/matlab')); FisherVectorEncode('$conf','$name','$sfepdir/fisher/$codebook_path/PCA$ar[0]_MaxFeat100000_GmmCtr$ar[1]_GmmInitkmeans/codebook.mat','$sfepdir/fisher/$codebook_path/PCA$ar[0]_MaxFeat100000_GmmCtr$ar[1]_GmmInitkmeans/pca_proj.mat',$ar[2],$ar[3],'sum', '$record_out'); exit;\" > /dev/null 2>/dev/null";
		my $cmd = "($sfepdir/fisher/cpp_version/bin/fisher $conf $name $ar[2] $ar[3] $sfepdir/fisher/$codebook_path/PCA$ar[0]_MaxFeat100000_GmmCtr$ar[1]_GmmInitkmeans/codebook.txt $sfepdir/fisher/$codebook_path/PCA$ar[0]_MaxFeat100000_GmmCtr$ar[1]_GmmInitkmeans/pca_proj.txt && touch $record_out) >/dev/null 2>/dev/null";
		push @fv_cmds, $cmd;
	}
	my $out_sh = "$scriptdir/" . output_sh("fv_$name"."_extract.sh");
	output_array( $out_sh, \@fv_cmds );
	output_records( "$scriptdir/" . output_sh("$name"."_fv_check.sh"), $out_sh );
	gen_PQ($name."_fv", \@fv_list, $record_dir);
}

sub change_prefix{
	my ($old_path, $prefix, $newdir, $all_dirs, $old_suffix, $new_suffix) = @_;
	$old_path =~ s/^$prefix/$newdir\//;
	$old_path =~ s/$old_suffix$/$new_suffix/;
	my $dir_out = $old_path;
	$dir_out =~ s/[^\/]*$//;
	$all_dirs -> { $dir_out } = 1;	
	return $old_path;
}

sub output_sh{
	my ($sh) = @_;
	my $sh2 = sprintf('%03d_', $cmd_id).$sh;
	$cmd_id++;
	return $sh2;
}

sub get_video_width_height{
	my ($vidss, $chunk_size, $name, $output, $scriptdir) = @_;
	my @vids = @{$vidss};
	my @chunked_vids = split_array(\@vids, $chunk_size); #200 vids per chunk for finding video size
		my $info_scripts_dir = "$scriptdir/$name";
	mkpath($info_scripts_dir);
	my @cmds = ();
	my @info_files = ();
	for(my $i = 0; $i < scalar(@chunked_vids); $i++){
		output_array( "$info_scripts_dir/vids.chunk.$i", $chunked_vids[$i] );
		push @cmds, "python $sfepdir/resize_code/python/get_video_dim_info_by_list_2.0.py $info_scripts_dir/vids.chunk.$i $info_scripts_dir/video.info.$i $sfepdir/resize_code/$machine/ffmpeg";
		push @info_files, "$info_scripts_dir/video.info.$i";
	}
	output_array( "$scriptdir/".output_sh("$name.sh") , \@cmds);
#execute("rm -f $output");
	my $cmd = "cat ".join(" ", @info_files)." > $output";
	open(OUT, ">$scriptdir/".output_sh("$name"."_merge.sh"));
	print OUT "$cmd\n";
	close(OUT);
}

#resize videos

sub output_array{
	my ($fname, $ar) = @_;
	my @arr = @{$ar};
	#if( -f $fname ){
	#	return;
	#}
	open(FNAME, ">$fname");
	for(my $i = 0; $i < scalar(@arr); $i++){
		print FNAME $arr[$i]."\n";
	}
	close(FNAME);
}
sub output_array2{
	#return;
	my ($fname, $ar) = @_;
	my @arr = @{$ar};
	open(FNAME, ">$fname");
	for(my $i = 0; $i < scalar(@arr); $i++){
		print FNAME $arr[$i]."\n";
	}
	close(FNAME);
}

sub split_array{
	my ($ar, $batch_size) = @_;
	my @arr = @{$ar};
	my @output_array = ();
	my $acc = 0;
	my $i = 0;
	my $max_size = scalar(@arr);
	while( 1 ){
		my $stop = 0;
		my @tp_array = ();
		for(my $j = 0; $j < $batch_size; $j++){
			push @tp_array, $arr[$acc];
			$acc++;
			if( $acc == $max_size ){
				$stop = 1;
				last;
			}
		}
		push @output_array, [@tp_array];
		if( $stop == 1 ){
			last;
		}
	}
	return @output_array;
}

sub execute{
	my ($cmd) = @_;
#print STDERR "executing $cmd\n";
	my $ret = system($cmd);
	if( $ret < 0 ){
		print STDERR "$cmd failed with return value $ret!!\n";
	}
}

sub get_all_files{
	my ($dir, $l, $suffix) = @_;

	if( -d $dir ){
	}else{
		print STDERR "$dir does not exist!!\n";
		exit -1;
	}
	opendir(D, $dir);
	my @f = readdir(D);
	closedir(D);

	foreach my $file (@f) {
		my $fname = $dir . "/" . $file;
		if( $file eq '.' || $file eq '..' ){
		}elsif( -d $fname ){
			get_all_files($fname, $l, $suffix);
		}else{
#$file =~ s/\..*//;
			my $name = $file;
			$name =~ s/\..*//;
			if( $file =~ /\.$suffix$/ ){
				$l -> {$name} = $fname;
			}
		}
	}
}
