#############################################################
# Sample Makefile format
#############################################################
LIBRARY = libaegis.a
MAIN    = aegis
CC	= gcc -c
CFLAGS  = -O3 -fomit-frame-pointer -march=x86-64 -msse3 -fomit-frame-pointer -fopenmp
# -lstdc++
# -static-libgcc -static-intel
CFLAGS  += -I../../base/ -I../../features/ -I../../hmm/ -I../../ibis/ -I../../itf/ -I../../main/ -I../../models/ -I../../net/ -I../../search/ -I../../stc/  -D__USE_GNU -DLINUX
# -I/people/yct/Software/lda-0.9

SRCS	= *.cpp
IDIR    = /afs/cs.cmu.edu/user/fmetze/tools/ATLAS/include
IDIR2   = /afs/cs.cmu.edu/user/fmetze/tools/ATLAS/islr0s14/include
#IDIR    = /people/wrhsiao1/ATLAS/include
IDIR3   = /afs/cs.cmu.edu/user/fmetze/tools/CLAPACK-3.2.1/INCLUDE
IDIR4   = ../include-extra
LDIR    = .
LDIR    = /afs/cs.cmu.edu/user/fmetze/tools/ATLAS/lib/islr0s14
#LDIR2   = /people/wrhsiao1/ATLAS/lib/Linux_XEONSSE3_4
#LDIR3   = /people/wrhsiao1/CLAPACK/lib/Linux_XEONSSE3_4
#LDIR2   = /people/wrhsiao/ATLAS/lib/Linux_PIIISSE1_2
#LDIR3   = /people/wrhsiao/CLAPACK/lib/Linux_PIIISSE1_2
LIB	=
# nrc
OBJECT  = edge.o gaussian.o ahmm.o HTKparser.o main.o memmgr.o multimix.o pdf.o relation_info.o state.o readHTKformat.o computebelldelta.o computebell_fe_preprocess.o recognizer.o linear_algebra.o cluster_engine.o str_util.o math_engine.o token.o cache.o transcription.o mlf.o network.o reg_tree.o mllr.o adictionary.o kernel.o comp_kernel.o hmmset.o accumulator.o util_new.o janus_aegis.o aegis.o ev.o ei_kev.o rsw_ext.o svm.o mpe.o g-mmie.o gdlr.o gdfsa.o

.SUFFIXES : .o .cpp .c 

#############################################################


#############################################################
# Dependencies 
#############################################################

$(LIBRARY) : $(OBJECT)
	ar -rv $@ $(OBJECT)

#$(MAIN) : $(OBJECT) 
#	$(CC) -L$(LDIR) -L$(LDIR2) -L$(LDIR3) $(CFLAGS) -I$(IDIR) -I$(IDIR2) -o $@ $^ -l$(LIB) -llapack_Linux_P4SSE2 -lcblaswr -llapack -lcblas -latlas -lg2c 

#all : $(MAIN)  

#t : t.o
#	$(CC) -o $@ t.o $(OBJECT)

#useConfig : useConfig.o
#	$(CC) -o $@ useConfig.o $(OBJECT)
#############################################################

depend :
	makedepend -Y -- $(CFLAGS) -- $(SRCS)


clean :
	\rm -f *.o core $(MAIN)

backup :
	tar cvf - *.h *.cpp Makefile | gzip > /35Ga/SpeechTools/wav2mfcc_`date '+%d%m%y'`.tgz

install :
	cp -f $(LIBRARY) ../lib

.c.o :
	$(CC) $(CFLAGS) -I$(DIR) -c $<

.cpp.o :
	$(CC) $(CFLAGS) -I$(IDIR) -I$(IDIR2) -I$(IDIR3) -I$(IDIR4) -c $< 

# DO NOT DELETE

accumulator.o: accumulator.h network.h ahmm.h cluster_engine.h memmgr.h pdf.h
accumulator.o: gaussian.h linear_algebra.h cppblas.h cpplapack.h
accumulator.o: clapack_orig.h nrutil_ex.h math_engine.h util_new.h global.h
accumulator.o: multimix.h state.h edge.h relation_info.h str_util.h hmmset.h
accumulator.o: ../../hmm/hmm.h ../../base/list.h ../../base/common.h
accumulator.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
accumulator.o: ../../base/array.h ../../ibis/sglobal.h ../../ibis/slimits.h
accumulator.o: ../../ibis/bmem.h ../../base/matrix.h
accumulator.o: ../../models/dictionary.h ../../models/word.h
accumulator.o: ../../models/phones.h ../../models/tags.h
accumulator.o: ../../models/amodel.h ../../models/tree.h
accumulator.o: ../../models/questions.h ../../models/ptree.h
accumulator.o: ../../models/modelSet.h ../../models/topo.h
accumulator.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
accumulator.o: ../../features/feature.h ../../models/stream.h
accumulator.o: ../../models/codebook.h ../../models/rewrite.h
accumulator.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
accumulator.o: ../../base/mach_ind_io.h ../../models/duration.h
accumulator.o: ../../models/context.h cache.h
adictionary.o: adictionary.h transcription.h str_util.h
aegis.o: aegis.h memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h
aegis.o: cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h util_new.h
aegis.o: global.h multimix.h state.h edge.h ahmm.h cluster_engine.h
aegis.o: relation_info.h str_util.h recognizer.h readHTKformat.h network.h
aegis.o: hmmset.h ../../hmm/hmm.h ../../base/list.h ../../base/common.h
aegis.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
aegis.o: ../../base/array.h ../../ibis/sglobal.h ../../ibis/slimits.h
aegis.o: ../../ibis/bmem.h ../../base/matrix.h ../../models/dictionary.h
aegis.o: ../../models/word.h ../../models/phones.h ../../models/tags.h
aegis.o: ../../models/amodel.h ../../models/tree.h ../../models/questions.h
aegis.o: ../../models/ptree.h ../../models/modelSet.h ../../models/topo.h
aegis.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
aegis.o: ../../features/feature.h ../../models/stream.h
aegis.o: ../../models/codebook.h ../../models/rewrite.h ../../models/covar.h
aegis.o: ../../models/cbmap.h ../../models/gs.h ../../base/mach_ind_io.h
aegis.o: ../../models/duration.h ../../models/context.h token.h
aegis.o: transcription.h cache.h accumulator.h HTKparser.h adictionary.h
aegis.o: reg_tree.h mlf.h ei_kev.h mllr.h kernel.h comp_kernel.h ev.h
aegis.o: rsw_ext.h ../../models/distrib.h ../../models/lh.h
aegis.o: ../../models/modalities.h mpe.h ../../ibis/glat.h ../../ibis/svmap.h
aegis.o: ../../ibis/ngramLM.h ../../ibis/svocab.h
aegis.o: ../../models/distribStream.h ../../models/score.h
aegis.o: ../../ibis/scoreA.h g-mmie.h gdlr.h ../../models/mladapt.h gdfsa.h
ahmm.o: ahmm.h cluster_engine.h memmgr.h pdf.h gaussian.h linear_algebra.h
ahmm.o: cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h
ahmm.o: util_new.h global.h multimix.h state.h edge.h relation_info.h
ahmm.o: str_util.h
cache.o: cache.h
cluster_engine.o: cluster_engine.h memmgr.h pdf.h gaussian.h linear_algebra.h
cluster_engine.o: cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h
cluster_engine.o: math_engine.h util_new.h global.h multimix.h state.h edge.h
cluster_engine.o: ahmm.h relation_info.h str_util.h
comp_kernel.o: comp_kernel.h memmgr.h pdf.h gaussian.h linear_algebra.h
comp_kernel.o: cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h
comp_kernel.o: util_new.h global.h multimix.h state.h edge.h ahmm.h
comp_kernel.o: cluster_engine.h relation_info.h str_util.h recognizer.h
comp_kernel.o: readHTKformat.h network.h hmmset.h ../../hmm/hmm.h
comp_kernel.o: ../../base/list.h ../../base/common.h ../../itf/error.h
comp_kernel.o: ../../itf/itf.h ../../ibis/ngramLM.h ../../base/array.h
comp_kernel.o: ../../ibis/sglobal.h ../../ibis/slimits.h ../../ibis/bmem.h
comp_kernel.o: ../../base/matrix.h ../../models/dictionary.h
comp_kernel.o: ../../models/word.h ../../models/phones.h ../../models/tags.h
comp_kernel.o: ../../models/amodel.h ../../models/tree.h
comp_kernel.o: ../../models/questions.h ../../models/ptree.h
comp_kernel.o: ../../models/modelSet.h ../../models/topo.h
comp_kernel.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
comp_kernel.o: ../../features/feature.h ../../models/stream.h
comp_kernel.o: ../../models/codebook.h ../../models/rewrite.h
comp_kernel.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
comp_kernel.o: ../../base/mach_ind_io.h ../../models/duration.h
comp_kernel.o: ../../models/context.h token.h transcription.h cache.h
comp_kernel.o: accumulator.h kernel.h
computebelldelta.o: util_new.h global.h computebelldelta.h
computebell_fe_preprocess.o: util_new.h global.h
edge.o: edge.h state.h pdf.h
ei_kev.o: ei_kev.h memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h
ei_kev.o: cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h util_new.h
ei_kev.o: global.h multimix.h state.h edge.h ahmm.h cluster_engine.h
ei_kev.o: relation_info.h str_util.h recognizer.h readHTKformat.h network.h
ei_kev.o: hmmset.h ../../hmm/hmm.h ../../base/list.h ../../base/common.h
ei_kev.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
ei_kev.o: ../../base/array.h ../../ibis/sglobal.h ../../ibis/slimits.h
ei_kev.o: ../../ibis/bmem.h ../../base/matrix.h ../../models/dictionary.h
ei_kev.o: ../../models/word.h ../../models/phones.h ../../models/tags.h
ei_kev.o: ../../models/amodel.h ../../models/tree.h ../../models/questions.h
ei_kev.o: ../../models/ptree.h ../../models/modelSet.h ../../models/topo.h
ei_kev.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
ei_kev.o: ../../features/feature.h ../../models/stream.h
ei_kev.o: ../../models/codebook.h ../../models/rewrite.h ../../models/covar.h
ei_kev.o: ../../models/cbmap.h ../../models/gs.h ../../base/mach_ind_io.h
ei_kev.o: ../../models/duration.h ../../models/context.h token.h
ei_kev.o: transcription.h cache.h accumulator.h mllr.h kernel.h comp_kernel.h
ev.o: ev.h memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h cpplapack.h
ev.o: clapack_orig.h nrutil_ex.h math_engine.h util_new.h global.h multimix.h
ev.o: state.h edge.h ahmm.h cluster_engine.h relation_info.h str_util.h
ev.o: recognizer.h readHTKformat.h network.h hmmset.h ../../hmm/hmm.h
ev.o: ../../base/list.h ../../base/common.h ../../itf/error.h ../../itf/itf.h
ev.o: ../../ibis/ngramLM.h ../../base/array.h ../../ibis/sglobal.h
ev.o: ../../ibis/slimits.h ../../ibis/bmem.h ../../base/matrix.h
ev.o: ../../models/dictionary.h ../../models/word.h ../../models/phones.h
ev.o: ../../models/tags.h ../../models/amodel.h ../../models/tree.h
ev.o: ../../models/questions.h ../../models/ptree.h ../../models/modelSet.h
ev.o: ../../models/topo.h ../../models/trans.h ../../models/senones.h
ev.o: ../../hmm/path.h ../../features/feature.h ../../models/stream.h
ev.o: ../../models/codebook.h ../../models/rewrite.h ../../models/covar.h
ev.o: ../../models/cbmap.h ../../models/gs.h ../../base/mach_ind_io.h
ev.o: ../../models/duration.h ../../models/context.h token.h transcription.h
ev.o: cache.h accumulator.h HTKparser.h adictionary.h reg_tree.h mllr.h
ev.o: kernel.h comp_kernel.h
gaussian.o: gaussian.h pdf.h linear_algebra.h cppblas.h cpplapack.h
gaussian.o: clapack_orig.h nrutil_ex.h math_engine.h util_new.h global.h
gdfsa.o: gdfsa.h ../../ibis/glat.h ../../ibis/slimits.h ../../ibis/sglobal.h
gdfsa.o: ../../ibis/svmap.h ../../base/list.h ../../base/common.h
gdfsa.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
gdfsa.o: ../../base/array.h ../../ibis/bmem.h ../../base/matrix.h
gdfsa.o: ../../models/dictionary.h ../../models/word.h ../../models/phones.h
gdfsa.o: ../../models/tags.h ../../ibis/ngramLM.h ../../ibis/svocab.h
gdfsa.o: ../../hmm/path.h ../../models/senones.h ../../features/feature.h
gdfsa.o: ../../models/stream.h ../../models/codebook.h ../../models/rewrite.h
gdfsa.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
gdfsa.o: ../../base/mach_ind_io.h ../../models/mladapt.h
gdfsa.o: ../../models/distrib.h ../../models/lh.h ../../models/modalities.h
gdfsa.o: ../../models/tree.h ../../models/questions.h ../../models/ptree.h
gdfsa.o: ../../models/modelSet.h ../../models/distribStream.h
gdfsa.o: ../../models/score.h ../../hmm/hmm.h ../../models/amodel.h
gdfsa.o: ../../models/topo.h ../../models/trans.h ../../models/duration.h
gdfsa.o: ../../models/context.h ../../ibis/scoreA.h relation_info.h
gdfsa.o: linear_algebra.h cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h
gdfsa.o: math_engine.h util_new.h global.h str_util.h cache.h gaussian.h
gdfsa.o: pdf.h
gdlr.o: gdlr.h ../../ibis/glat.h ../../ibis/slimits.h ../../ibis/sglobal.h
gdlr.o: ../../ibis/svmap.h ../../base/list.h ../../base/common.h
gdlr.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
gdlr.o: ../../base/array.h ../../ibis/bmem.h ../../base/matrix.h
gdlr.o: ../../models/dictionary.h ../../models/word.h ../../models/phones.h
gdlr.o: ../../models/tags.h ../../ibis/ngramLM.h ../../ibis/svocab.h
gdlr.o: ../../hmm/path.h ../../models/senones.h ../../features/feature.h
gdlr.o: ../../models/stream.h ../../models/codebook.h ../../models/rewrite.h
gdlr.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
gdlr.o: ../../base/mach_ind_io.h ../../models/mladapt.h
gdlr.o: ../../models/distrib.h ../../models/lh.h ../../models/modalities.h
gdlr.o: ../../models/tree.h ../../models/questions.h ../../models/ptree.h
gdlr.o: ../../models/modelSet.h ../../models/distribStream.h
gdlr.o: ../../models/score.h ../../hmm/hmm.h ../../models/amodel.h
gdlr.o: ../../models/topo.h ../../models/trans.h ../../models/duration.h
gdlr.o: ../../models/context.h ../../ibis/scoreA.h relation_info.h
gdlr.o: linear_algebra.h cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h
gdlr.o: math_engine.h util_new.h global.h str_util.h cache.h
g-mmie.o: g-mmie.h ../../ibis/glat.h ../../ibis/slimits.h
g-mmie.o: ../../ibis/sglobal.h ../../ibis/svmap.h ../../base/list.h
g-mmie.o: ../../base/common.h ../../itf/error.h ../../itf/itf.h
g-mmie.o: ../../ibis/ngramLM.h ../../base/array.h ../../ibis/bmem.h
g-mmie.o: ../../base/matrix.h ../../models/dictionary.h ../../models/word.h
g-mmie.o: ../../models/phones.h ../../models/tags.h ../../ibis/ngramLM.h
g-mmie.o: ../../ibis/svocab.h ../../hmm/path.h ../../models/senones.h
g-mmie.o: ../../features/feature.h ../../models/stream.h
g-mmie.o: ../../models/codebook.h ../../models/rewrite.h ../../models/covar.h
g-mmie.o: ../../models/cbmap.h ../../models/gs.h ../../base/mach_ind_io.h
g-mmie.o: ../../models/distrib.h ../../models/lh.h ../../models/modalities.h
g-mmie.o: ../../models/tree.h ../../models/questions.h ../../models/ptree.h
g-mmie.o: ../../models/modelSet.h ../../models/distribStream.h
g-mmie.o: ../../models/score.h ../../hmm/hmm.h ../../models/amodel.h
g-mmie.o: ../../models/topo.h ../../models/trans.h ../../models/duration.h
g-mmie.o: ../../models/context.h ../../ibis/scoreA.h relation_info.h
g-mmie.o: linear_algebra.h cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h
g-mmie.o: math_engine.h util_new.h global.h str_util.h cache.h
hmmset.o: hmmset.h network.h ahmm.h cluster_engine.h memmgr.h pdf.h
hmmset.o: gaussian.h linear_algebra.h cppblas.h cpplapack.h clapack_orig.h
hmmset.o: nrutil_ex.h math_engine.h util_new.h global.h multimix.h state.h
hmmset.o: edge.h relation_info.h str_util.h ../../hmm/hmm.h ../../base/list.h
hmmset.o: ../../base/common.h ../../itf/error.h ../../itf/itf.h
hmmset.o: ../../ibis/ngramLM.h ../../base/array.h ../../ibis/sglobal.h
hmmset.o: ../../ibis/slimits.h ../../ibis/bmem.h ../../base/matrix.h
hmmset.o: ../../models/dictionary.h ../../models/word.h ../../models/phones.h
hmmset.o: ../../models/tags.h ../../models/amodel.h ../../models/tree.h
hmmset.o: ../../models/questions.h ../../models/ptree.h
hmmset.o: ../../models/modelSet.h ../../models/topo.h ../../models/trans.h
hmmset.o: ../../models/senones.h ../../hmm/path.h ../../features/feature.h
hmmset.o: ../../models/stream.h ../../models/codebook.h
hmmset.o: ../../models/rewrite.h ../../models/covar.h ../../models/cbmap.h
hmmset.o: ../../models/gs.h ../../base/mach_ind_io.h ../../models/duration.h
hmmset.o: ../../models/context.h
HTKparser.o: HTKparser.h memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h
HTKparser.o: cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h util_new.h
HTKparser.o: global.h multimix.h state.h edge.h ahmm.h cluster_engine.h
HTKparser.o: relation_info.h str_util.h network.h adictionary.h
HTKparser.o: transcription.h hmmset.h ../../hmm/hmm.h ../../base/list.h
HTKparser.o: ../../base/common.h ../../itf/error.h ../../itf/itf.h
HTKparser.o: ../../ibis/ngramLM.h ../../base/array.h ../../ibis/sglobal.h
HTKparser.o: ../../ibis/slimits.h ../../ibis/bmem.h ../../base/matrix.h
HTKparser.o: ../../models/dictionary.h ../../models/word.h
HTKparser.o: ../../models/phones.h ../../models/tags.h ../../models/amodel.h
HTKparser.o: ../../models/tree.h ../../models/questions.h
HTKparser.o: ../../models/ptree.h ../../models/modelSet.h ../../models/topo.h
HTKparser.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
HTKparser.o: ../../features/feature.h ../../models/stream.h
HTKparser.o: ../../models/codebook.h ../../models/rewrite.h
HTKparser.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
HTKparser.o: ../../base/mach_ind_io.h ../../models/duration.h
HTKparser.o: ../../models/context.h reg_tree.h
janus_aegis.o: janus_aegis.h ../../ibis/slimits.h ../../ibis/lks.h
janus_aegis.o: ../../ibis/sglobal.h ../../ibis/bmem.h ../../base/matrix.h
janus_aegis.o: ../../base/common.h ../../itf/error.h ../../itf/itf.h
janus_aegis.o: ../../ibis/lmla.h ../../ibis/stree.h ../../ibis/phmm.h
janus_aegis.o: ../../base/list.h ../../ibis/ngramLM.h ../../base/array.h
janus_aegis.o: ../../models/senones.h ../../hmm/path.h
janus_aegis.o: ../../features/feature.h ../../models/stream.h
janus_aegis.o: ../../models/word.h ../../models/phones.h ../../models/tags.h
janus_aegis.o: ../../models/codebook.h ../../models/rewrite.h
janus_aegis.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
janus_aegis.o: ../../base/mach_ind_io.h ../../models/tree.h
janus_aegis.o: ../../models/questions.h ../../models/ptree.h
janus_aegis.o: ../../models/modelSet.h ../../models/trans.h
janus_aegis.o: ../../models/topo.h ../../models/duration.h ../../ibis/xhmm.h
janus_aegis.o: ../../ibis/svocab.h ../../models/dictionary.h
janus_aegis.o: ../../ibis/ngramLM.h ../../ibis/svmap.h ../../ibis/strace.h
janus_aegis.o: ../../ibis/smem.h ../../ibis/phraseLM.h ../../ibis/metaLM.h
janus_aegis.o: ../../ibis/cfg.h ../../ibis/cfgTypes.h ../../ibis/glat.h
janus_aegis.o: ../../models/mladapt.h ../../models/distrib.h
janus_aegis.o: ../../models/lh.h ../../models/modalities.h
janus_aegis.o: ../../models/distribStream.h ../../models/amodel.h
janus_aegis.o: ../../models/context.h ../../hmm/hmm.h aegis.h memmgr.h pdf.h
janus_aegis.o: gaussian.h linear_algebra.h cppblas.h cpplapack.h
janus_aegis.o: clapack_orig.h nrutil_ex.h math_engine.h util_new.h global.h
janus_aegis.o: multimix.h state.h edge.h ahmm.h cluster_engine.h
janus_aegis.o: relation_info.h str_util.h recognizer.h readHTKformat.h
janus_aegis.o: network.h hmmset.h token.h transcription.h cache.h
janus_aegis.o: accumulator.h HTKparser.h adictionary.h reg_tree.h mlf.h
janus_aegis.o: ei_kev.h mllr.h kernel.h comp_kernel.h ev.h rsw_ext.h mpe.h
janus_aegis.o: ../../models/score.h ../../ibis/scoreA.h g-mmie.h gdlr.h
janus_aegis.o: gdfsa.h
kernel.o: kernel.h memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h
kernel.o: cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h util_new.h
kernel.o: global.h multimix.h state.h edge.h ahmm.h cluster_engine.h
kernel.o: relation_info.h str_util.h recognizer.h readHTKformat.h network.h
kernel.o: hmmset.h ../../hmm/hmm.h ../../base/list.h ../../base/common.h
kernel.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
kernel.o: ../../base/array.h ../../ibis/sglobal.h ../../ibis/slimits.h
kernel.o: ../../ibis/bmem.h ../../base/matrix.h ../../models/dictionary.h
kernel.o: ../../models/word.h ../../models/phones.h ../../models/tags.h
kernel.o: ../../models/amodel.h ../../models/tree.h ../../models/questions.h
kernel.o: ../../models/ptree.h ../../models/modelSet.h ../../models/topo.h
kernel.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
kernel.o: ../../features/feature.h ../../models/stream.h
kernel.o: ../../models/codebook.h ../../models/rewrite.h ../../models/covar.h
kernel.o: ../../models/cbmap.h ../../models/gs.h ../../base/mach_ind_io.h
kernel.o: ../../models/duration.h ../../models/context.h token.h
kernel.o: transcription.h cache.h accumulator.h
linear_algebra.o: linear_algebra.h cppblas.h cpplapack.h clapack_orig.h
linear_algebra.o: nrutil_ex.h math_engine.h util_new.h global.h
main.o: memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h cpplapack.h
main.o: clapack_orig.h nrutil_ex.h math_engine.h util_new.h global.h
main.o: multimix.h state.h edge.h ahmm.h cluster_engine.h relation_info.h
main.o: str_util.h recognizer.h readHTKformat.h network.h hmmset.h
main.o: ../../hmm/hmm.h ../../base/list.h ../../base/common.h
main.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
main.o: ../../base/array.h ../../ibis/sglobal.h ../../ibis/slimits.h
main.o: ../../ibis/bmem.h ../../base/matrix.h ../../models/dictionary.h
main.o: ../../models/word.h ../../models/phones.h ../../models/tags.h
main.o: ../../models/amodel.h ../../models/tree.h ../../models/questions.h
main.o: ../../models/ptree.h ../../models/modelSet.h ../../models/topo.h
main.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
main.o: ../../features/feature.h ../../models/stream.h
main.o: ../../models/codebook.h ../../models/rewrite.h ../../models/covar.h
main.o: ../../models/cbmap.h ../../models/gs.h ../../base/mach_ind_io.h
main.o: ../../models/duration.h ../../models/context.h token.h
main.o: transcription.h cache.h accumulator.h HTKparser.h adictionary.h
main.o: reg_tree.h mlf.h
math_engine.o: math_engine.h linear_algebra.h cppblas.h cpplapack.h
math_engine.o: clapack_orig.h nrutil_ex.h util_new.h global.h
memmgr.o: memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h cpplapack.h
memmgr.o: clapack_orig.h nrutil_ex.h math_engine.h util_new.h global.h
memmgr.o: multimix.h state.h edge.h ahmm.h cluster_engine.h relation_info.h
memmgr.o: str_util.h
mlf.o: mlf.h transcription.h str_util.h
mllr.o: mllr.h memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h
mllr.o: cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h util_new.h
mllr.o: global.h multimix.h state.h edge.h ahmm.h cluster_engine.h
mllr.o: relation_info.h str_util.h recognizer.h readHTKformat.h network.h
mllr.o: hmmset.h ../../hmm/hmm.h ../../base/list.h ../../base/common.h
mllr.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
mllr.o: ../../base/array.h ../../ibis/sglobal.h ../../ibis/slimits.h
mllr.o: ../../ibis/bmem.h ../../base/matrix.h ../../models/dictionary.h
mllr.o: ../../models/word.h ../../models/phones.h ../../models/tags.h
mllr.o: ../../models/amodel.h ../../models/tree.h ../../models/questions.h
mllr.o: ../../models/ptree.h ../../models/modelSet.h ../../models/topo.h
mllr.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
mllr.o: ../../features/feature.h ../../models/stream.h
mllr.o: ../../models/codebook.h ../../models/rewrite.h ../../models/covar.h
mllr.o: ../../models/cbmap.h ../../models/gs.h ../../base/mach_ind_io.h
mllr.o: ../../models/duration.h ../../models/context.h token.h
mllr.o: transcription.h cache.h accumulator.h
mpe.o: mpe.h ../../ibis/slimits.h ../../ibis/glat.h ../../ibis/sglobal.h
mpe.o: ../../ibis/svmap.h ../../base/list.h ../../base/common.h
mpe.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
mpe.o: ../../base/array.h ../../ibis/bmem.h ../../base/matrix.h
mpe.o: ../../models/dictionary.h ../../models/word.h ../../models/phones.h
mpe.o: ../../models/tags.h ../../ibis/ngramLM.h ../../ibis/svocab.h
mpe.o: ../../hmm/path.h ../../models/senones.h ../../features/feature.h
mpe.o: ../../models/stream.h ../../models/codebook.h ../../models/rewrite.h
mpe.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
mpe.o: ../../base/mach_ind_io.h ../../models/distrib.h ../../models/lh.h
mpe.o: ../../models/modalities.h ../../models/tree.h ../../models/questions.h
mpe.o: ../../models/ptree.h ../../models/modelSet.h
mpe.o: ../../models/distribStream.h ../../models/score.h ../../hmm/hmm.h
mpe.o: ../../models/amodel.h ../../models/topo.h ../../models/trans.h
mpe.o: ../../models/duration.h ../../models/context.h ../../ibis/scoreA.h
mpe.o: util_new.h global.h str_util.h
multimix.o: multimix.h pdf.h gaussian.h linear_algebra.h cppblas.h
multimix.o: cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h util_new.h
multimix.o: global.h
network.o: network.h ahmm.h cluster_engine.h memmgr.h pdf.h gaussian.h
network.o: linear_algebra.h cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h
network.o: math_engine.h util_new.h global.h multimix.h state.h edge.h
network.o: relation_info.h str_util.h
pdf.o: pdf.h
readHTKformat.o: readHTKformat.h util_new.h global.h computebelldelta.h
readHTKformat.o: computebell_fe_preprocess.h
recognizer.o: recognizer.h memmgr.h pdf.h gaussian.h linear_algebra.h
recognizer.o: cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h
recognizer.o: util_new.h global.h multimix.h state.h edge.h ahmm.h
recognizer.o: cluster_engine.h relation_info.h str_util.h readHTKformat.h
recognizer.o: network.h hmmset.h ../../hmm/hmm.h ../../base/list.h
recognizer.o: ../../base/common.h ../../itf/error.h ../../itf/itf.h
recognizer.o: ../../ibis/ngramLM.h ../../base/array.h ../../ibis/sglobal.h
recognizer.o: ../../ibis/slimits.h ../../ibis/bmem.h ../../base/matrix.h
recognizer.o: ../../models/dictionary.h ../../models/word.h
recognizer.o: ../../models/phones.h ../../models/tags.h ../../models/amodel.h
recognizer.o: ../../models/tree.h ../../models/questions.h
recognizer.o: ../../models/ptree.h ../../models/modelSet.h
recognizer.o: ../../models/topo.h ../../models/trans.h ../../models/senones.h
recognizer.o: ../../hmm/path.h ../../features/feature.h ../../models/stream.h
recognizer.o: ../../models/codebook.h ../../models/rewrite.h
recognizer.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
recognizer.o: ../../base/mach_ind_io.h ../../models/duration.h
recognizer.o: ../../models/context.h token.h transcription.h cache.h
recognizer.o: accumulator.h
reg_tree.o: reg_tree.h
relation_info.o: relation_info.h
rsw_ext.o: rsw_ext.h memmgr.h pdf.h gaussian.h linear_algebra.h cppblas.h
rsw_ext.o: cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h util_new.h
rsw_ext.o: global.h multimix.h state.h edge.h ahmm.h cluster_engine.h
rsw_ext.o: relation_info.h str_util.h recognizer.h readHTKformat.h network.h
rsw_ext.o: hmmset.h ../../hmm/hmm.h ../../base/list.h ../../base/common.h
rsw_ext.o: ../../itf/error.h ../../itf/itf.h ../../ibis/ngramLM.h
rsw_ext.o: ../../base/array.h ../../ibis/sglobal.h ../../ibis/slimits.h
rsw_ext.o: ../../ibis/bmem.h ../../base/matrix.h ../../models/dictionary.h
rsw_ext.o: ../../models/word.h ../../models/phones.h ../../models/tags.h
rsw_ext.o: ../../models/amodel.h ../../models/tree.h ../../models/questions.h
rsw_ext.o: ../../models/ptree.h ../../models/modelSet.h ../../models/topo.h
rsw_ext.o: ../../models/trans.h ../../models/senones.h ../../hmm/path.h
rsw_ext.o: ../../features/feature.h ../../models/stream.h
rsw_ext.o: ../../models/codebook.h ../../models/rewrite.h
rsw_ext.o: ../../models/covar.h ../../models/cbmap.h ../../models/gs.h
rsw_ext.o: ../../base/mach_ind_io.h ../../models/duration.h
rsw_ext.o: ../../models/context.h token.h transcription.h cache.h
rsw_ext.o: accumulator.h HTKparser.h adictionary.h reg_tree.h mllr.h kernel.h
rsw_ext.o: comp_kernel.h
state.o: state.h pdf.h
str_util.o: str_util.h
svm.o: svm.h
token.o: token.h state.h pdf.h util_new.h global.h
transcription.o: transcription.h
util_new.o: util_new.h global.h
whitener.o: whitener.h readHTKformat.h util_new.h global.h linear_algebra.h
whitener.o: cppblas.h cpplapack.h clapack_orig.h nrutil_ex.h math_engine.h
