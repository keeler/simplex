#include "Math.hpp"
#include "OrientedBoundingBox.hpp"
#include "Octree.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Window.hpp"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <cstdlib>
#include "GL/glut.h"
using namespace std;

#include <iostream>
#include <stdlib.h>

bool keyState[256] = { false };
Terrain *_myTerrain;
Camera *_myCamera;
Window *_myWindow;

void handleEvents()
{
	Event event;
	while( _myWindow->getEvent( event ) )
	{
		if( event.type == KEY_DOWN && event.key.keyCode == ASCII_KEY )
		{
			if( event.key.asciiCode == 27 )
			{
				delete _myTerrain;
				delete _myCamera;
				delete _myWindow;
				exit( 1 );
			}
			keyState[event.key.asciiCode] = true;
		}
		else if( event.type == KEY_UP && event.key.keyCode == ASCII_KEY )
		{
			keyState[event.key.asciiCode] = false;
		}
	}

	if( keyState['w'] )
	{
		_myCamera->moveForward( 2.5f );
	}
	if( keyState['s'] )
	{
		_myCamera->moveBackward( 2.5f );
	}
	if( keyState['a'] )
	{
		_myCamera->moveLeft( 2.5f );
	}
	if( keyState['d'] )
	{
		_myCamera->moveRight( 2.5f );
	}
	if( keyState['j'] )
	{
		_myCamera->rotateYaw( -0.5f );
	}
	if( keyState['l'] )
	{
		_myCamera->rotateYaw( 0.5f );
	}
	if( keyState['i'] )
	{
		_myCamera->rotatePitch( -0.5f );
	}
	if( keyState['k'] )
	{
		_myCamera->rotatePitch( 0.5f );
	}

	glutPostRedisplay();
}

void Window::renderScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	handleEvents();
	_myCamera->look();
	_myTerrain->render();
	
	glutSwapBuffers();
}

int main( int argc, char** argv )
{
	_myWindow = new Window( 400, 400, "Terrain with Textures" );
	_myCamera = new Camera( Vector3f( 200, 0, 200 ), 0.0f, 0.0f );
	_myTerrain = new Terrain( "vtr.bmp", "terrain_texture.bmp", 20 );

	_myWindow->beginRendering();
	return 0;
}









