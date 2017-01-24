

# Specify the main targets

TYPE = opt

ifeq ($(OBJDIR),)
BINDIR = LSA_bin
STORE = .make-$(TYPE)
else
BINDIR = ../$(OBJDIR)
STORE = ../$(OBJDIR)/.make-$(TYPE)

endif

LIBRARY = $(BINDIR)/liblda.a
DEMOX = $(BINDIR)/demo
LDA = $(BINDIR)/lda
INIT = $(BINDIR)/initmodel


# Default build type

# Which directories contain source files
DIRS = main janus_itf

# Which libraries are linked
LIBS =

# Dynamic libraries
DLIBS =

# The next blocksc hange some variables depending on the build type
ifeq ($(TYPE),opt)
LDFLAGS =
CFLAGS  = -Wno-deprecated -I../base/ -I../features/ -I../hmm/ -I../ibis/ -I../itf/ -I../main/ -I../models/ -I../net/ -I../search/ -I../stc/  -D__USE_GNU -DLINUX -ffast-math -fomit-frame-pointer
 
CFLAGS += -O3 -DBINARY -DCHEAP
MACROS =
endif



# Add directories to the include and library paths
INCPATH = main janus_itf $(SRIINC)
#INCPATH = main janus_itf srilm-headers lsa-headers
LIBPATH = 

# Which files to add to backups, apart from the source code
EXTRA_FILES = Makefile demo.cpp initmodel.cpp main.cpp
# The compiler
CC = g++

# Where to store object and dependancy files.

# Makes a list of the source (.cpp) files.
SOURCE := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.cpp))
# List of header files.
HEADERS := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.h))
# Makes a list of the object files that will have to be created.
OBJECTS := $(addprefix $(STORE)/, $(SOURCE:.cpp=.o))
# Same for the .d (dependancy) files.
DFILES := $(addprefix $(STORE)/,$(SOURCE:.cpp=.d))

# Specify phony rules. These are rules that are not real files.
.PHONY: clean backup dirs

# what to build
all: lib init lda demox
forjanus: flatobjects
demox: $(DEMOX)
init: $(INIT)
lda: $(LDA)
lib: $(LIBRARY)

flatobjects: dirs $(OBJECTS)
	  @echo copying objects.
	  find $(STORE) -name "*.o" -exec cp {} ../$(OBJDIR) \;

# Main target. A @ in front of a command prevents make from displaying
# it to the standard output.
#$(TARGET): dirs $(OBJECTS)
#	  @echo Linking $(TARGET).#
#	  @$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(foreach LIBX, \
#          $(LIBS),-l$(LIBX)) $(foreach LIB,$(LIBPATH),-L$(LIB))

$(LDA): dirs $(LIBRARY) main.cpp
	$(CC) $(CFLAGS) $(foreach INC,$(INCPATH),-I$(INC))\
          -o $@ main.cpp $(LIBRARY) $(LDFLAGS) $(foreach LIBX, \
          $(LIBS),-l$(LIBX)) $(foreach LIB,$(LIBPATH),-L$(LIB))

$(DEMOX): dirs $(LIBRARY) demo.cpp
	$(CC) $(CFLAGS) $(foreach INC,$(INCPATH),-I$(INC))\
          -o $@ demo.cpp $(LIBRARY) $(LDFLAGS) $(foreach LIBX, \
          $(LIBS),-l$(LIBX)) $(foreach LIB,$(LIBPATH),-L$(LIB))

$(INIT): dirs $(LIBRARY) initmodel.cpp
	$(CC) $(CFLAGS) $(foreach INC,$(INCPATH),-I$(INC)) \
          -o $@ initmodel.cpp $(LIBRARY) $(LDFLAGS) $(foreach LIBX, \
          $(LIBS),-l$(LIBX)) $(foreach LIB,$(LIBPATH),-L$(LIB))

$(LIBRARY): dirs $(OBJECTS)
	echo building lib $(LIBRARY).
	ar -rv $@ $(OBJECTS)

# Rule for creating object file and .d file, the sed magic is to add
# the object path at the start of the file because the files gcc
# outputs assume it will be in the same dir as the source file.
$(STORE)/%.o: %.cpp
	  echo Creating object file for $*...
	  $(CC) -Wp,-MMD,$(STORE)/$*.dd $(CFLAGS) $(foreach INC,$(INCPATH),-I$(INC))\
                $(foreach MACRO,$(MACROS),-D$(MACRO)) -c $< -o $@
	  @sed -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(STORE)/$*.dd > $(STORE)/$*.d
	  @rm -f $(STORE)/$*.dd	


# Empty rule to prevent problems when a header is deleted.
%.hpp: ;
%.h: ;

# Cleans up the objects, .d files and executables.
clean:
	  @echo Making clean.
	  @-rm -f $(foreach DIR,$(DIRS),$(STORE)/$(DIR)/*.d $(STORE)/$(DIR)/*.o)
	  -rm -frd $(LIBRARY) $(DEMOX)  $(LDA)  $(INIT) $(STORE)


# Create necessary directories
dirs:
	    @-if [ ! -e $(STORE) ]; then mkdir $(STORE); fi;
	    @-if [ ! -e $(BINDIR) ]; then mkdir $(BINDIR); fi;
	    @-$(foreach DIR,$(DIRS), if [ ! -e $(STORE)/$(DIR) ]; \
         then mkdir $(STORE)/$(DIR); fi; )

# Includes the .d files so it knows the exact dependencies for every
# source.
-include $(DFILES)
