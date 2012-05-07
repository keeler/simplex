CC = g++
CFLAGS = -Wall -g
PROG = main

SRCS = main.cpp Math.cpp OrientedBoundingBox.cpp Octree.cpp Camera.cpp Texture.cpp ImageLoader.cpp Terrain.cpp Window.cpp Sound.cpp SoundLoader.cpp

LIBS = -lglut -lGLU -lopenal -lalut

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG) *~
