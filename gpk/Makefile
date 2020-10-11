Platform=Linux.x64
ProjectName:=gpk
Configuration=Release
OPT=3
ifeq ($(DEBUG), 1)
	Configuration=Debug
	OPT=0
endif

OutDir =../../$(Platform).$(Configuration)
IntDir =../../obj/$(Platform).$(Configuration)/$(ProjectName)

OBJDIRS := $(patsubst %, $(IntDir), $(ProjectName))

CFLAGS=-std=c++17 -Wall -O$(OPT)
CC=g++

_OBJ = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
OBJ = $(patsubst %,$(IntDir)/%,$(_OBJ))

$(IntDir)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

build: $(OBJDIRS) $(OBJ)
	ar rcs $(OutDir)/lib$(LIB_NAME).a $(OBJ)

$(OBJDIRS):
	mkdir -p $@ 

.PHONY: clean

clean:
	rm -rf $(IntDir)