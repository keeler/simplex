#include "GL/glut.h"
#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <cstdlib>
#include "../Texture.hpp"
using namespace std;

#define PI 3.14159265f

#define DEFAULT_WINDOW_WIDTH 400
#define DEFAULT_WINDOW_HEIGHT 400

// Globals
bool _keyState[256] = { false };    // Assume no keys pressed at first
bool _mouseDown = false;
float _paintbrushRadius = 20;
int _imageWidth = 0;
int _imageHeight = 0;
string _imageFilename;
int _mousePosX = DEFAULT_WINDOW_WIDTH / 2;
int _mousePosY = DEFAULT_WINDOW_HEIGHT / 2;
char *_editImagePixels = NULL;		// Array of RGB triples
unsigned int _editImageTextureId;

// OpenGL necessities
void initRendering();
void handleResize( int width, int height );
void handleKeyDown( unsigned char key, int x, int y );
void handleKeyUp( unsigned char key, int x, int y );
void handleKeyPress();
void handleMouseMove( int x, int y );
void handleMouseDrag( int x, int y );
void handleMouseClick( int button, int state, int x, int y );
void drawScene();
// Helper functions
void initializeEditImage();		// Only call after image width and height are determined, AND initRenering() has been called
void cleanUp();
void floodFill( int r, int g, int b );
void drawCircularPaintbrush( int xPos, int yPos, float radius );

int main( int argc, char** argv )
{
	if( argc < 4 )
	{
		cout << "usage: heightmap_tool <filename> <width> <height>" << endl;
		cout << "\tIf <filename> exists, it will be edited." << endl;
		cout << "\tIf <filename> doesn't exist, it will be created." << endl;
		exit( 1 );
	}

	_imageWidth = atoi( argv[2] );
	_imageHeight = atoi( argv[3] );

	_mousePosX = _imageWidth / 2;
	_mousePosY = _imageHeight / 2;

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize( _imageWidth, _imageHeight );
	
	glutCreateWindow( "Heightmap Editor Tool" );
	initRendering();
	initializeEditImage();

	glutDisplayFunc( drawScene );
	glutReshapeFunc( handleResize );
	glutKeyboardFunc( handleKeyDown );
	glutKeyboardUpFunc( handleKeyUp );
	glutPassiveMotionFunc( handleMouseMove );
	glutMotionFunc( handleMouseDrag );
	glutMouseFunc( handleMouseClick );
	
	glutMainLoop();
	return 0;
}

void initRendering()
{
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glDepthMask( GL_TRUE );
	glEnable( GL_TEXTURE_RECTANGLE_ARB );
	glClearColor( 0.48f, 0.71f, 0.73f, 1.0f );
	// Make it so mouse arrow cursor doesn't show in the window
	glutSetCursor( GLUT_CURSOR_NONE );
}

void handleResize( int width, int height )
{
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, width, height, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
}

void drawScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	handleKeyPress();

	glColor3f( 1.0f, 1.0f, 1.0f );
	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, _editImageTextureId );
	// Render the image on a quad
	glBegin( GL_QUADS );
		glTexCoord2i( 0, _imageWidth );                           
		glVertex2i( 0, 0 );
		glTexCoord2i( _imageWidth, _imageHeight );     
		glVertex2i( _imageWidth, 0 );
		glTexCoord2i( _imageWidth, 0 );    
		glVertex2i( _imageWidth, _imageHeight );
		glTexCoord2i( 0, 0 );          
		glVertex2i( 0, _imageHeight );
	glEnd();

	drawCircularPaintbrush( _mousePosX, _mousePosY, _paintbrushRadius );
	
	glutSwapBuffers();
}

void handleKeyDown( unsigned char key, int x, int y )
{
	// Escape key
	if( key == 27 )
	{
		exit( 1 );
	}

	_keyState[key] = true;
}

void handleKeyUp( unsigned char key, int x, int y )
{
	_keyState[key] = false;
}

void handleKeyPress()
{
	if( _keyState['w'] )
	{
		_paintbrushRadius += 1.0f;
	}
	if( _keyState['s'] )
	{
		if( _paintbrushRadius >= 1.0f )
		{
			_paintbrushRadius -= 1.0f;
		}
	}

	glutPostRedisplay();
}

void handleMouseMove( int x, int y )
{
	_mousePosX = x;
	_mousePosY = y;

	glutPostRedisplay();
}

void handleMouseDrag( int x, int y )
{
	_mousePosX = x;
	_mousePosY = y;

	floodFill( 255, 255, 255 );

	glutPostRedisplay();
}

void handleMouseClick( int button, int state, int x, int y )
{
	_mousePosX = x;
	_mousePosY = y;

	floodFill( 255, 255, 255 );

	glutPostRedisplay();
}

void initializeEditImage()
{
	_editImagePixels = new char[_imageWidth * _imageHeight * 3];

	srand( time( NULL ) );

	for( int x = 0; x < _imageWidth; x++ )
	{
		for( int y = 0; y < _imageHeight; y++ )
		{
			for( int i = 0; i < 3; i++ )
			{
				_editImagePixels[3 * ( y * _imageWidth + x ) + i] = 128;   // 128 on each of RGB is half grey
			}
		}
	}

	glGenTextures( 1, &_editImageTextureId );
	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, _editImageTextureId );
	glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, _imageWidth, _imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _editImagePixels );
}

void cleanUp()
{
	if( _editImagePixels != NULL )
	{
		delete [] _editImagePixels;
	}
}

void floodFill( int r, int g, int b )
{
	for( int x = _mousePosX - _paintbrushRadius; x < _mousePosX + _paintbrushRadius; x++ )
	{
		for( int y = _mousePosY - _paintbrushRadius; y < _mousePosY + _paintbrushRadius; y++ )
		{
			// Check to ensure we don't access something outside the image
			if( x > 0 && x < _imageWidth && y > 0 && y < _imageHeight )
			{
				if( ( ( x - _mousePosX ) * ( x - _mousePosX ) + ( y - _mousePosY ) * ( y - _mousePosY ) ) <= _paintbrushRadius * _paintbrushRadius )
				{
					// Note that it's ( _imageHeight - _mousePosY ) because when you pass the
					// pixel array to glTexSubImage2D, the vertical axis is flipped
					_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 0] = r;
					_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 1] = g;
					_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 2] = b;
				}
			}
		}
	}

	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, _editImageTextureId );
	glTexSubImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, _imageWidth, _imageHeight, GL_RGB, GL_UNSIGNED_BYTE, _editImagePixels );
}

void drawCircularPaintbrush( int xPos, int yPos, float radius )
{
	// Draw the circular outline
	glColor3f( 1.0f, 0.0f, 0.0f );
	glBegin( GL_LINE_LOOP );
		glVertex2f( xPos + radius, yPos );
		for( int angle = 1; angle <= 360; angle += 1 )
		{
			glVertex2f( xPos + radius * cos( ( PI / 180 ) * angle ), yPos + radius * sin( ( PI / 180 ) * angle ) );
		}
	glEnd();

	// Draw the crosshair
	glBegin( GL_LINES );
		// Horizontal part
		glVertex2f( xPos - 5, yPos );    // Left vertex
		glVertex2f( xPos + 5, yPos );    // Right vertex
		// Vertical part
		glVertex2f( xPos, yPos + 5 );    // Bottom vertex
		glVertex2f( xPos, yPos - 5 );    // Top vertex
	glEnd();
}
