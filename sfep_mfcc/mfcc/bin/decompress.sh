#Performs decompression of bz2 files in a directory

srcpath=$1

for filename in $(find $srcpath -name "*.bz2") 
do
	bzip2 -d $filename
done


