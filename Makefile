C=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=shsel.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=shsel

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
