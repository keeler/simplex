#include "Math.hpp"
#include "OrientedBoundingBox.hpp"
#include "Octree.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Window.hpp"
#include "Sound.hpp"
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
Sound *_mySound;
Vector3f *_sourcePos;

void handleEvents()
{
	Event event;
	while( _myWindow->getEvent( event ) )
	{
		if( event.type == KEY_PRESSED && event.keyData.isAscii )
		{
			if( event.keyData.keyCode == KEY_ESCAPE )
			{
				delete _myTerrain;
				delete _myCamera;
				delete _myWindow;
				delete _mySound;
				exit( 1 );
			}
			keyState[event.keyData.keyCode] = true;
		}
		else if( event.type == KEY_RELEASED && event.keyData.isAscii )
		{
			keyState[event.keyData.keyCode] = false;
		}
	}

	if( keyState['g'] )
	{
		*_sourcePos += Vector3f( 2, 0, 0 );
		_mySound->setPosition( *_sourcePos );
		_mySound->setVelocity( Vector3f( 2, 0, 0 ) );
	}
	if( keyState['f'] )
	{
		*_sourcePos -= Vector3f( 2, 0, 0 );
		_mySound->setPosition( *_sourcePos );
		_mySound->setVelocity( Vector3f( -2, 0, 0 ) );
	}
	else
	{
		_mySound->setVelocity( Vector3f( 0, 0, 0 ) );
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
		_myCamera->rotateYaw( -2.5f );
	}
	if( keyState['l'] )
	{
		_myCamera->rotateYaw( 2.5f );
	}
	if( keyState['i'] )
	{
		_myCamera->rotatePitch( -2.5f );
	}
	if( keyState['k'] )
	{
		_myCamera->rotatePitch( 2.5f );
	}
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
	_myTerrain = new Terrain( "resources/terrainmap.bmp", "resources/terrain_texture.bmp", 50 );
	_mySound = new Sound( "resources/British Beercan, Jamaican Bacon.wav" );
	_sourcePos = new Vector3f( 0, 0, 0 );

	_mySound->play();

	_myWindow->beginRendering();
	return 0;
}
