CC = g++
CFLAGS = -Wall -g
PROG = main

SRCS = main.cpp Vector3f.cpp Quaternion.cpp OrientedBoundingBox.cpp Octree.cpp Camera.cpp

LIBS = -lglut -lGLU

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
