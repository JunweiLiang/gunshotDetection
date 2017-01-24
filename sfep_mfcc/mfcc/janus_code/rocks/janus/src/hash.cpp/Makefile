LIBRARY = libcpp.a

CC	= g++

# adapted from Janus Makefile
CFLAGS  = -I../base/ -I../features/ -I../hmm/ -I../ibis/ -I../itf/ -I../main/ -I../models/ -I../net/ -I../search/ -I../stc/ -D__USE_GNU -DLINUX -Wall -ffast-math -fomit-frame-pointer -DBINARY
#CFLAGS  = -I../base/ -I../features/ -I../hmm/ -I../ibis/ -I../itf/ -I../main/ -I../models/ -I../net/ -I../search/ -I../stc/  -D__USE_GNU -DLINUX -Wall -ffast-math -fomit-frame-pointer -I../LSA -DBINARY 
CFLAGS += -O3
#CFLAGS += -g
#CFLAGS += -DUPDATE_CBS

LFLAGS  = 

VERSION	= 1.1
PKGNAME	= cpp-$(VERSION)

# Pls add your .cpp files here
SRCS	=  janus_hash.cpp
OBJDIR ?= hash.cpp
OBJECT = $(foreach file,$(notdir $(SRCS:%.cpp=%.o)),../$(OBJDIR)/$(file))

.SUFFIXES : .o .cpp

#all : $(LIBRARY)
all: $(OBJECT)
forjanus: $(OBJECT)


$(LIBRARY) : $(OBJECT)
	ar -rv $@ $(OBJECT)

clean :
	#-rcsclean
	-rm -f $(LIBRARY) $(OBJECT) *.o tags core

pkg:
	-mkdir -p dist
	-tar cvf - *.h *.cpp Makefile | gzip > dist/$(PKGNAME).tar.gz

docify :
	for f in *.cpp *.h; do \
	   echo $$f; \
	   docify $$f $$f.new; \
	   mv $$f.new $$f; \
        done;

indent :
	for f in *.cpp *.h; do \
	   echo $$f; \
	   bcpp $$f; \
        done;

doc :
	mkdir -p Doc; \
	doc++ -A -a -b -p -d Doc *.h;

in :
	rcs-checkin *.h *.cpp Makefile
	rcsclean; \

depend:
	makedepend -Y -- $(CFLAGS) -- $(SRCS)

../$(OBJDIR)/%.o: %.cpp 
	$(CC) $(ALL_CFLAGS) -c $< -o $@

.cpp.o :
	$(CC) $(CFLAGS) -c $<

