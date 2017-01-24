# gunshotDetection

	1. Dependencies:
		(1) ffmpeg >= 2.2.4
		(2) parallel
		(3) python 2.7
			a. numpy
			b. matplotlib
			c. liblinear (see https://github.com/cjlin1/liblinear/tree/master/python for installation, after installation, please change the path to the tool at the beginning of the script reranking.py)

	2. Example command:
		$ python master.py -video testVideo/h-3VlDyHml8.mp4 -workPath test/ -debug

	3. Output:
		This script's input is one video (h-3VlDyHml8.mp4), and outputs gunshot detection results.
		Under test/ directory as in the example command, you will find:

		test/
			bof_efm/ # you can delete this
			segments/ # this as well
			sfep_mfcc_config.txt # this as well
			video_segments.lst # this as well

			predictionGraph/ # where there will be two gunshot probability graphs
			original_output.txt # output from the original gunshot detection model
			reranking0.txt # output after 1st reranking
			reranking1.txt # output after 2nd reranking


		The two gunshot probability graphs (original/reranking) indicate when in the video has gunshots. The original graph is direct output from our gunshot model, and the reranking graph is smoother (less peaks). 

		In original_output.txt, the content is like this (the same as rerankingN.txt):

		h-3VlDyHml8_00:00:00.000_00:00:03.000.bof 0.125023953733 1 # ignore the 1
		h-3VlDyHml8_00:00:01.000_00:00:04.000.bof 0.226340212292 1
		h-3VlDyHml8_00:00:02.000_00:00:05.000.bof 0.295783539461 1
		h-3VlDyHml8_00:00:03.000_00:00:06.000.bof 0.356629042813 1
		h-3VlDyHml8_00:00:04.000_00:00:07.000.bof 0.318163187588 1
		h-3VlDyHml8_00:00:05.000_00:00:08.000.bof 0.218750178549 1
		...

		It means that for video h-3VlDyHml8.mp4, the confidence of it containing gunshot during 00:00:00 to 00:00:03 is 0.125023953733, and during 00:00:03 to 00:00:06 it is 0.356629042813. (so from 3 seconds to 6 seconds the video is more likely to contain gunshots)
