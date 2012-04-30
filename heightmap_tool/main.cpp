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
bool keyState[256] = { false };    // Assume no keys pressed at first
float paintbrushRadius = 20;
int imageWidth = 0;
int imageHeight = 0;
string imageFilename;
int mousePosX = DEFAULT_WINDOW_WIDTH / 2;
int mousePosY = DEFAULT_WINDOW_HEIGHT / 2;

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

	imageWidth = atoi( argv[2] );
	imageHeight = atoi( argv[3] );

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize( imageWidth, imageHeight );
	
	glutCreateWindow( "Heightmap Editor Tool" );
	initRendering();

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

	drawCircularPaintbrush( mousePosX, mousePosY, paintbrushRadius );
	
	glutSwapBuffers();
}

void handleKeyDown( unsigned char key, int x, int y )
{
	// Escape key
	if( key == 27 )
	{
		exit( 1 );
	}

	keyState[key] = true;
}

void handleKeyUp( unsigned char key, int x, int y )
{
	keyState[key] = false;
}

void handleKeyPress()
{
	if( keyState['w'] )
	{
		paintbrushRadius += 0.1f;
	}
	if( keyState['s'] )
	{
		if( paintbrushRadius >= 0.1f )
		{
			paintbrushRadius -= 0.1f;
		}
	}

	glutPostRedisplay();
}

void handleMouseMove( int x, int y )
{
	mousePosX = x;
	mousePosY = y;

	glutPostRedisplay();
}

void handleMouseDrag( int x, int y )
{
	mousePosX = x;
	mousePosY = y;

	glutPostRedisplay();
}

void handleMouseClick( int button, int state, int x, int y )
{
	mousePosX = x;
	mousePosY = y;

	glutPostRedisplay();
}

void drawCircularPaintbrush( int xPos, int yPos, float radius )
{
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_LINE_LOOP );
		glVertex2f( xPos + radius, yPos );
		for( int angle = 1; angle <= 360; angle += 1 )
		{
			glVertex2f( xPos + radius * cos( ( PI / 180 ) * angle ), yPos + radius * sin( ( PI / 180 ) * angle ) );
		}
	glEnd();
}
