CC = g++
CFLAGS = -Wall -g
PROG = main

SRCS = main.cpp ../Texture.cpp ../ImageLoader.cpp

LIBS = -lglut -lGLU

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG) *~
