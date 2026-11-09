Platform:=Linux.x64
ProjectName:=gpk
Configuration:=Release
OPT=3
ifeq ($(DEBUG), 1)
	Configuration=Debug
	OPT=0

endif

OutDir =../../$(Platform).$(Configuration)
IntDir =../../obj/$(Platform).$(Configuration)/$(ProjectName)
IncDir =../../zlib ../../gpk/llc

OBJDIRS := $(patsubst %, $(IntDir), $(ProjectName))
INCDIR := $(patsubst %,-I%,$(IncDir))

CFLAGS=-std=c++20 -Wall -O$(OPT) $(INCDIR)
CC=g++

_OBJ = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
OBJ = $(patsubst %,$(IntDir)/%,$(_OBJ))

$(IntDir)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

build: $(OBJDIRS) $(OBJ)
	ar rcs $(OutDir)/$(ProjectName).a $(OBJ)

$(OBJDIRS):
	mkdir -p $@ 

.PHONY: clean

clean:
	rm -rf $(IntDir)
