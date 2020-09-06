## Makefile for C++ project using Boost
#
# @author Cedric "levif" Le Dillau
#
# Some notes:
# - Using ':=' instead of '=' assign the value at Makefile parsing time,
#   others are evaluated at usage time. This discards
# - Use ':set list' in Vi/Vim to show tabs (Ctrl-v-i force tab insertion)
#

# List to '.PHONY' all fake targets, those that are neither files nor folders.
# "all" and "clean" are good candidates.
.PHONY: all, clean

# Define the final program name
PROGNAME := LinuxClient 

# Pre-processor flags to be used for includes (-I) and defines (-D) 
CPPFLAGS := -DUSE_BOOST -DTELLDUS_ACTIVE=0

# CFLAGS is used for C compilation options.
CFLAGS := -Wall -O0

# CXXFLAGS is used for C++ compilation options.
CXXFLAGS += -Wall -O0 -std=c++17

# LDFLAGS is used for linker (-g enables debug symbols)
#LDFLAGS  += -lpthread -lcrypto -lssl -lcpprest -lstdc++ -lpthread -ltelldus-core -lcurl -lbluetooth -g
LDFLAGS  += -lpthread -lcrypto -lssl -lcpprest -lstdc++ -lpthread -lcurl -lbluetooth -g

# Which Boost modules to use (all)
BOOST_MODULES = \
  date_time     \
  system        

# Boost libraries' type (a suffix)
BOOST_MODULES_TYPE := -mt

# Define library names with their type
BOOST_MODULES_LIBS := $(addsuffix $(BOOT_MODULES_TYPE),$(BOOST_MODULES))

# Define the linker argument to use the Boost libraries.
BOOST_LDFLAGS := $(addprefix -lboost_,$(BOOST_MODULES_LIBS))

# Feed compiler/linker flags with Boost's
CPPFLAGS += $(BOOST_CPPFLAGS)
LDFLAGS += $(BOOST_LDFLAGS)

# List the project' sources to compile or let the Makefile recognize
# them for you using 'wildcard' function.
#
#SOURCES = simple_ls.cpp rawr.cpp converter.cpp
SOURCES = $(wildcard *.cpp)

# List the project' headers or let the Makefile recognize
# them for you using 'wildcard' function.
#
#HEADERS = simple_ls.h 2dquicksort.h rawr.h
HEADERS = $(wildcard %.h)

# Construct the list of object files based on source files using
# simple extension substitution.
OBJECTS = $(SOURCES:%.cpp=%.o)

#
# Now declare the dependencies rules and targets
#
# Starting with 'all' make it  becomes the default target when none 
# is specified on 'make' command line.
all : $(PROGNAME)

# Declare that the final program depends on all objects and the Makfile
$(PROGNAME) : $(OBJECTS) Makefile
	gcc -o $@ $(OBJECTS) $(LDFLAGS)

# Now the choice of using implicit rules or not (my choice)...
#
# Choice 1: use implicit rules and then we only need to add some dependencies
#           to each object.
#
## Tells make that each object file depends on all headers and this Makefile.
#$(OBJECTS) : $(HEADERS) Makefile
#
# Choice 2: don't use implicit rules and specify our will
%.o: %.cpp $(HEADERS) Makefile
	gcc $(CXXFLAGS) $(CPPFLAGS) -c $(OUTPUT_OPTION) $<

# Simple clean-up target
# notes:
# - the '@' before 'echo' informs make to hide command invocation.
# - the '-' before 'rm' command to informs make to ignore errors.
clean :
	@echo "Clean."
	-rm -f *.o $(PROGNAME) *.bak
