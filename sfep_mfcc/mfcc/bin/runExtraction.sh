
######################################
#
#	MFCC Extraction Recipe. 
#	Takes in path to the mp4 and generates mfcc40.bz2 files
#	USAGE: VideoPath MFCCPath 
#
######################################

VideoPath=$1
MFCCPath=$2
featureType=$3 # lmel60 for LogMel features and mfcc40 for MFCC features
FORMAT=$4

#WAVPath=$3

#Set up necessary paths for Janus
BINPATH=/data/MM22/iyu/MFCC_Extraction_Pipeline/bin
JANUSROOT=/data/MM22/iyu/MFCC_Extraction_Pipeline/janus_code/
export JANUSHOME=$JANUSROOT/janus
export JANUS_LIBRARY=$JANUSHOME/library
export LD_LIBRARY_PATH=/opt/tcltk-8.5.10/lib/:${JANUSROOT}/janus-libs/:$LD_LIBRARY_PATH

echo $JANUSHOME
echo $JANUS_LIBRARY

JANUS_SCRIPT=${JANUSROOT}/janus/janus

#convert MP4 files to wav files
#${JANUS_SCRIPT} mp42wav.tcl

#get all the mp4 files in the directory
abs_vid_path=$(readlink -f $VideoPath);
find $abs_vid_path -name "*.$FORMAT" > video.lst

#extract the features
${JANUS_SCRIPT} ${BINPATH}/convert-v0.tcl video.lst -featName FEAT -featDesc ${BINPATH}/featDesc_${featureType} -featExt ${featureType} -outPath ${MFCCPath}

errorCode=$?
if [[ $errorCode -ne 0 ]]
	then
	exit $errorCode
fi

