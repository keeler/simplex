CC = g++
CFLAGS = -Wall -g
PROG = main

SRCS = main.cpp TerrainPreview.cpp ../ImageLoader.cpp ../Math.cpp

LIBS = -lglut -lGLU

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG) *~
