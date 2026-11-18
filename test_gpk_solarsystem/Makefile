IncDir			:= ../../gpk/llc
Libs			:= llc.a zlibvc.a 
GccLibs			:= xcb xcb-image pthread
Platform		:= Linux.x64
ProjectName		:= test_gpk_solarsystem
Configuration	:= Release

C_DEFS			:= -DNOMINMAX -DGPK_XCB
OPT				:= 3

ifeq ($(DEBUG), 1)
	Configuration	= Debug
	OPT				= 0
	C_DEFS			+= -DDEBUG
endif

OutDir :=../../$(Platform).$(Configuration)
IntDir :=../../obj/$(Platform).$(Configuration)/$(ProjectName)

OBJDIRS	:= $(patsubst %, $(IntDir), $(ProjectName))
LIBS	:= $(patsubst %, $(OutDir)/%, $(Libs))
GCCLIBS := $(patsubst %, -l%, $(GccLibs))
INCDIR	:= $(patsubst %, -I%, $(IncDir))

CFLAGS	:= -o $(OutDir)/$(ProjectName).bin -std=c++17 -Wall -Werror -O$(OPT) $(C_DEFS) $(INCDIR) -L$(OutDir)
CC		:= g++

_OBJ	= $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
OBJ		= $(patsubst %,$(IntDir)/%,$(_OBJ))

$(IntDir)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

build: $(OBJDIRS) $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(C_DEFS) $(GCCLIBS) $(LIBS)

$(OBJDIRS):
	mkdir -p $@ 

.PHONY: clean

clean:
	rm -rf $(IntDir)
