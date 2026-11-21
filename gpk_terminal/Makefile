IncDir			:= ../../gpk/llc ../../gpk/gpk
Libs			:= llc.a
GccLibs			:= xcb-image xkbcommon-x11 xkbcommon xcb pthread
Platform		:= Linux.x64
ProjectName		:= test_gpk_gui
Configuration	:= Release
ProjectDir		?= .
SolutionDir		?= $(ProjectDir)/..

C_DEFS			:= -DNOMINMAX -DGPK_XCB
OPT				:= 3

ifeq ($(DEBUG), 1)
	Configuration	= Debug
	OPT				= 0
	C_DEFS			+= -DDEBUG
endif

OutDir		:= $(SolutionDir)/../$(Platform).$(Configuration)
IntDir		:= $(SolutionDir)/../obj/$(Platform).$(Configuration)/$(ProjectName)

OBJDIRS	:= $(patsubst %, $(IntDir), $(ProjectName))
LIBS	:= $(patsubst %, $(OutDir)/%, $(Libs))
GCCLIBS := $(patsubst %, -l%, $(GccLibs))
INCDIR	:= $(patsubst %,-I%,$(IncDir))

CFLAGS		:= -std=c++23 -Wall -fpermissive -Wno-changes-meaning -O$(OPT) $(C_DEFS) $(INCDIR)
LLCFLAGS	:= -std=c++23 -Wall -Werror -O$(OPT) -I../../zlib $(C_DEFS) -include stdio.h -include stdlib.h
CC			:= g++

_OBJ	= $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJ		= $(patsubst %,$(IntDir)/%,$(_OBJ))

$(IntDir)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(OutDir)/llc.a:
	$(MAKE) -C ../../gpk/llc DEBUG=$(DEBUG) CFLAGS="$(LLCFLAGS)"

build: $(OBJDIRS) $(OBJ) $(LIBS)
	mkdir -p $(OutDir)
	$(CC) $(OBJ) $(CFLAGS) -o $(OutDir)/$(ProjectName).bin $(LIBS) $(GCCLIBS)

$(OBJDIRS):
	mkdir -p $@

.PHONY: clean

clean:
	rm -rf $(IntDir)
