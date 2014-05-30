CC=g++

ifeq ($(CFG),debug)
	CXXFLAGS=-Wall -g -DDEBUG
	LDFLAGS=-Wall -g -DDEBUG
else
	CXXFLAGS=-Wall
	LDFLAGS=-Wall
endif

INCPATH=inc
SRCPATH=src
OBJPATH=obj
BINPATH=bin

SRC=$(SRCPATH)/util.cpp \
    $(SRCPATH)/lodepng.cpp \
    $(SRCPATH)/fitcurve.cpp \
    $(SRCPATH)/GGVecLib.cpp \
    $(SRCPATH)/svg.cpp \
    $(SRCPATH)/graph.cpp \
    $(SRCPATH)/codeImage.cpp \
    $(SRCPATH)/bitmap.cpp \
    $(SRCPATH)/main.cpp
OBJ=$(OBJPATH)/util.o \
	$(OBJPATH)/lodepng.o \
	$(OBJPATH)/fitcurve.o \
	$(OBJPATH)/GGVecLib.o \
	$(OBJPATH)/svg.o \
	$(OBJPATH)/graph.o \
    $(OBJPATH)/codeImage.o \
    $(OBJPATH)/bitmap.o \
    $(OBJPATH)/main.o
EXEC=$(BINPATH)/b2v

INCLUDES=-I ./$(INCPATH)

default: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJPATH)/%.o: $(SRCPATH)/%.cpp $(INCPATH)/%.h
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean cleanall

clean:
	rm -f $(OBJPATH)/*.o

cleanall: clean
	rm -f $(EXEC)