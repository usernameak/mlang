CC=g++
CFLAGS=-c -Wall -O3
LDFLAGS=
SOURCES=main.cpp runtime.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mlang

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@