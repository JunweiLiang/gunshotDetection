#! /usr/bin/python 
"""This will convert the passed mfcc format files first into txyc and then bof format"""

import sys
import os
import helper

SPLIT=40;
if(os.environ.has_key("SPLIT")):
    SPLIT=int(os.environ["SPLIT"])
if(not os.environ.has_key("FORMAT")):
    os.environ["FORMAT"]="mfcc40";
if(not os.environ.has_key("QUEUE")):
    os.environ["QUEUE"]="testing";

print "Instances per thread : "+str(SPLIT)

#Define global paths
PathToBinaries = "/data/MM22/iyu/MFCC_Extraction_Pipeline/bin/"
PathTotxyc = "java -Xmx400m -cp "+ os.path.join(PathToBinaries,"bof.jar") + " txyc"
PathTobof = "java -Xmx400m -cp " + os.path.join(PathToBinaries,"bof.jar") + " bof"

def mfcc2txyc(mfccCsvPath,kmeansCenters,clusterCount,outpath,TopK):
    #if(os.path.exists(newPath)):
    #    return newPath
    commandTxyc = PathTotxyc + " " + kmeansCenters + " " + mfccCsvPath + " " + TopK + " " + outpath
    #print "Executing :" + commandTxyc
    #os.system(commandTxyc)
    return commandTxyc

def txyc2bof(txycPath,clusterCount,outpath,TopK):
    #if(os.path.exists(newPath)):
    #    return newPath

    commandBof = PathTobof + " " + txycPath + " " + clusterCount + " "+ TopK + " " + outpath
    #print "Executing :" + commandBof
    #os.system(commandBof)
    return commandBof

def usage():
    print "USAGE: mfcc2bof pathToMFCC kmeansCentres outpath"
    print " pathToMFCC   : root path under which the mfcc are located"
    print " kmeansCentres: KMeans Centers"
    print " outpath      : Path where the .txyc and .bof files would be located"
    print " topK         : Degree of Soft Clustering"
    sys.exit()

def qsub(script):
    print script
    script = os.path.abspath(script)
    rootdir = os.path.dirname(script)
    queue = os.environ["QUEUE"]
    #QSUB="qsub -j eo -S /bin/bash -o "+rootdir+" -l nodes=1:ppn=1,pmem=2000mb,walltime=\"23:59:00\" -d " + rootdir + " " +script
    QSUB="qsub -j eo -S /bin/bash -o "+rootdir+" -l nodes=1:ppn=1 -q "+ queue +" -d " + rootdir + " " +script
    print "Executing "+QSUB
    os.system(QSUB)
    return

def main():
    #Arg handling
    if(len(sys.argv)==4):
        #Give a default value
	    TopK = 10
    elif(not len(sys.argv)==5 ):
        usage()

    pathToMfcc = os.path.abspath(sys.argv[1]) #root path under which .mfcc are present
    kmeansCenters = os.path.abspath(sys.argv[2]) #kmeans centers
    outpath = os.path.abspath(sys.argv[3]) #dir for the output
    TopK = sys.argv[4] #soft clustering	

    #find out the number of kmeans centers by counting the number of rows 
    clusterCount = str(len(open(kmeansCenters).readlines()))

    #get all the mfcc files
    print "Searching for "+ os.environ["FORMAT"]+" files"
    mfccFiles = helper.get_files(pathToMfcc,os.environ["FORMAT"])

    count = 1
    name = os.path.join(outpath,"run_0.sh")
    script = open(name,"w");
    for mfcc in mfccFiles:
        if(count%SPLIT==0):
            script.close()
            qsub(name)
            name = os.path.join(outpath,"run_"+str(count/SPLIT)+".sh");
            script = open(name,"w")

        #create the new txyc file
        basename = os.path.basename(mfcc)
        txycFile = os.path.join(outpath,(basename.split("."))[0]+".txyc")
        bofFIle = os.path.join(outpath,(basename.split("."))[0]+".bof")

        script.write(mfcc2txyc(mfcc,kmeansCenters,clusterCount,outpath,TopK)+";\n");
        script.write(txyc2bof(txycFile,clusterCount,outpath,TopK)+";\n");
        count += 1
	
    #complete the last script
    script.close()
    qsub(name);
	
main()

