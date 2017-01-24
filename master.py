# coding=utf-8
# master script for video synchronization using audio

from ChunWai import *
import sys,os

def usage():
	print """
		-video: the path to the video for gunshot detection
		-workPath : the path for all the temporary files. You can delete it later
		-debug : under this mode we will print out as much as possible
	"""
	sys.exit()

def getTime(timestr):
	hour,minutes,sec = timestr.strip().split(":")
	return float(hour)*60.0*60.0+float(minutes)*60.0+float(sec)

if __name__ == "__main__":
	videoPath,workPath,debug = resolveParam(['-video','-workPath'],['-debug'])
	if(cp([videoPath,workPath])):
		usage()
	workPath = parsePath(workPath)
	codePath = parsePath(os.path.dirname(os.path.realpath(__file__))) # where the master.py at , assuming the rest of the scripts are all here

	# just some hard coded parameters, paraparaJob is the parallel cores to use
	paraJob,m,n,rerankIter,rerankPos,rerankNeg = 4,3,1,2,0.0112,0.667,

	# the model we will use for gunshot detection, we could replace with better model in the future
	modelPath = codePath+"models/Update_20161011.model"

	mkdir(workPath)
	# some temporay path
	segmentPath = workPath+"segments/"
	sfepPath = os.path.abspath(workPath+"sfep_mfcc/")+"/"	
	bofPath = workPath+"bof_efm/"
	# the path to save prediction graph
	graphPath = workPath+"predictionGraph/"

	# the scripts
	sfepToolPath = codePath+"sfep_mfcc/"
	segmentScript = codePath+'videoSeg.py'
	detectionScript = codePath+'liblinear_predict.py'
	rerankingScript = codePath+'reranking.py'
	graphScript = codePath+'eval_nolabel.py'
	try:
		duration = getTime(getVideoDuration(videoPath))
	except Exception as e:
		print "video is corrupted, please send this message to me:%s"%e
		sys.exit()

	# mkdir
	mkdir(segmentPath)
	mkdir(bofPath)
	mkdir(graphPath)
	
	print "Step 1, segmenting video..."
	cmd = "python %s -video %s -path %s -m %s -n %s"%(segmentScript,videoPath,segmentPath,m,n)
	runParallel(cmd,paraJob,debug)

	# get segment video lst
	output = commands.getoutput("ls %s/* > %svideo_segments.lst"%(os.path.abspath(segmentPath),workPath))


	print "Step 2, get mfcc..."
	
	# remove sfep path
	output = commands.getoutput("rm -rf %s"%sfepPath)
	# get a sfep config file
	config = "sfepdir=%s\noutputdir=%s\nvideopaths=%s/video_segments.lst\nmachine=rocks\npython27=python"%(sfepToolPath,os.path.abspath(sfepPath),os.path.abspath(workPath))
	configfile = open("%ssfep_mfcc_config.txt"%(workPath),"w")
	configfile.writelines("%s"%config)
	configfile.close()
	
	output = commands.getoutput("perl %sgencmds/gencmds.pl %ssfep_mfcc_config.txt"%(sfepToolPath,workPath))
	if(debug):
		print output

	output = commands.getoutput("bash %sscripts/009_mfcc_extract.sh"%sfepPath)
	if(debug):
		print output

	output = commands.getoutput("parallel -j %s < %sscripts/011_mfcc_decompress.sh"%(paraJob,sfepPath))
	if(debug):
		print output

	output = commands.getoutput("parallel -j %s < %sscripts/013_mfcc4k_bof_extract.sh"%(paraJob,sfepPath))
	if(debug):
		print output

	# move feature into bofPath
	output = commands.getoutput("mv %sfeatures/mfcc/mfcc4k/efm/* %s"%(sfepPath,bofPath))
	# remove sfep path
	output = commands.getoutput("rm -rf %s"%sfepPath)

	#get feature count
	featureCount = len(getFiles(bofPath,"bof"))
	topn = int(featureCount*rerankPos)
	botm = int(featureCount*rerankNeg)
	if(topn == 0):
		topn = 5

	print "Step 3, detecting gunshot..."
	output = commands.getoutput("python %s -featp %s -model %s -smoothing > %stesting_output.txt"%(detectionScript,bofPath,modelPath,workPath))
	if(debug):
		print output

	print "Step 4, reranking..."
	for i in xrange(rerankIter):
		if(i == 0):
			testfile = "original_output.txt"
		else:
			testfile = "reranking%s.txt"%(i-1)
		output = commands.getoutput("python %s -testfile %s%s -topn %s -botm %s -featp %s -smoothing > %sreranking%s.txt"%(rerankingScript,workPath,testfile,topn,botm,bofPath,workPath,i))
		if(debug):
			print output

	print "Step 5, printing gunshot probability graph..."	
	cmd1 = "python %s -imgfile %sreranking.png -testfile %sreranking%s.txt"%(graphScript,graphPath,workPath,rerankIter-1)
	cmd2 = "python %s -imgfile %soriginal.png -testfile %stesting_output.txt"%(graphScript,graphPath,workPath)
	
	output = commands.getoutput(cmd1)
	if(debug):
		print output
	output = commands.getoutput(cmd2)
	if(debug):
		print output
	