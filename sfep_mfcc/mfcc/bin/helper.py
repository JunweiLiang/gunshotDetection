import os
import sys

#returns all the files under a given path with a particular extension
def get_files(dir,type):
    filenames = list()
    pathList = os.listdir(dir)
    for path in pathList:
        path = dir + '/' + path
        if os.path.isdir(path):
            filenames.extend( get_files(path) )
        else:
            if(os.path.basename(path).endswith("."+type)):
                path = filenames.append( path )
        if(len(filenames)%100 == 0):
            print "size: "+str(len(filenames))
    return filenames


