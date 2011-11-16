CC = g++
CFLAGS = -Wall
PROG = main

SRCS = main.cpp Vector3f.cpp OrientedBoundingBox.cpp

LIBS = -lglut -lGLU

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
