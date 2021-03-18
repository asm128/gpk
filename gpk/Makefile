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
IncDir =../../zlib

OBJDIRS := $(patsubst %, $(IntDir), $(ProjectName))

CFLAGS=-std=c++17 -Wall -O$(OPT) -I$(IncDir)
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
