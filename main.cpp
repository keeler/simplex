#include "Vector3f.hpp"
#include "OrientedBoundingBox.hpp"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <cstdlib>
#include "GL/glut.h"
using namespace std;

// Should be anything more than 20, see lines 29-31, NUM_BOXES/20
const int NUM_BOXES = 500;

// Global; angle to rotate shapes by
float _angle = 30.0f;
OrientedBoundingBox * _myBoxes;
Vector3f * _rotationVectors;
float * _rotationRates;

void generateBoxes()
{
    _myBoxes = new OrientedBoundingBox[NUM_BOXES];

    Vector3f center;
    Vector3f edgeHalfLengths;
    Vector3f orientation[3];

    srand( time( NULL ) );
    for( int i = 0; i < NUM_BOXES; i++ )
    {
        center = Vector3f( (rand()%2?-1.0f:1.0f)*(rand()%(NUM_BOXES/20))/1.0,
                           (rand()%2?-1.0f:1.0f)*(rand()%(NUM_BOXES/20))/1.0,
                           (rand()%2?-1.0f:1.0f)*(rand()%(NUM_BOXES/20))/1.0 );
        edgeHalfLengths = Vector3f( 0.5f, 1.0f, 2.0f );

        orientation[0] = Vector3f( 1.0f, 0.0f, 0.0f );
        orientation[1] = Vector3f( 0.0f, 1.0f, 0.0f );
        orientation[2] = Vector3f( 0.0f, 0.0f, 1.0f );

        _myBoxes[i] = OrientedBoundingBox( center, edgeHalfLengths, orientation );
    }
}

void generateRotationVectors()
{
    _rotationVectors = new Vector3f[NUM_BOXES];

    for( int i = 0; i < NUM_BOXES; i++ )
    {
        _rotationVectors[i] = Vector3f( (rand()%2?-1.0f:1.0f)*(rand()%5)/1.0, (rand()%2?-1.0f:1.0f)*(rand()%5)/1.0, 0.0f );
    }

    _rotationRates = new float[NUM_BOXES];

    for( int i = 0; i < NUM_BOXES; i++ )
    {
        _rotationRates[i] = (rand()%5+3)/1.0;
    }
}

void handleKeypress( unsigned char key, int x, int y )
{
    switch( key )
    {
        // Escape key
        case 27:
            delete [] _myBoxes;
            delete [] _rotationVectors;
            delete [] _rotationRates;
            exit( 0 );
    }
}

void initRendering()
{
    // Makes shit render in 3d, man
    glEnable( GL_DEPTH_TEST );
	glEnable( GL_COLOR_MATERIAL );    // Allows us to color polygons

    // Use back-face culling
    glEnable( GL_CULL_FACE );

    glEnable( GL_LIGHTING );    // Allows us to use light
    glEnable( GL_LIGHT0 );
    glEnable( GL_NORMALIZE );

    glShadeModel( GL_SMOOTH );
}

void handleResize( int width, int height )
{
    // Tells OpenGL how to convert from coordinates to pixel values
    glViewport( 0, 0, width, height );
    // Switch to setting the camera perspective
    glMatrixMode( GL_PROJECTION );
    // Set the camera perspective
    glLoadIdentity();    // Reset the camera
    gluPerspective( 45.0, (double)width / (double)height, 1.0, 200.0 );
}

void drawScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	glTranslatef( 0.0f, 0.0f, -NUM_BOXES / 8 );
	glRotatef( _angle, 0.0f, 1.0f, 0.0f );
    glColor3f( 1.0f, 1.0f, 1.0f );

	// Add positioned light 0
	GLfloat lightColor0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPos0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightColor0 );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPos0 );

    bool hit[NUM_BOXES];
    for( int i = 0; i < NUM_BOXES; i++ )
    {
        hit[i] = false;
    }
    for( int i = 0; i < NUM_BOXES; i++ )
    {
        for( int j = 0; j < NUM_BOXES; j++ )
        {
            if( i == j )
            {
                continue;
            }
            else if( _myBoxes[i].collisionWith( _myBoxes[j] ) )
            {
                hit[i] = true;
                hit[j] = true;
            }
        }
    }

    for( int i = 0; i < NUM_BOXES; i++ )
    {
        if( hit[i] )
        {
            _myBoxes[i].draw( Vector3f( 1.0f, 0.0f, 0.0f ) );
        }
        else
        {
            _myBoxes[i].draw( Vector3f( 1.0f, 1.0f, 1.0f ) );
        }
    }

    glutSwapBuffers();
}

void updateAngles( int value )
{
	_angle += 0.5f;

	if( _angle > 360 )
	{
		_angle -= 360;
	}

    for( int i = 0; i < NUM_BOXES; i++ )
    {
        _myBoxes[i].rotate( _rotationVectors[i], _rotationRates[i] );
    }

	glutPostRedisplay();    // Tell GLUT that the scene has changed
	// Tell GLUT to call update again in 25 milliseconds
	glutTimerFunc( 10, updateAngles, 0 );
}

int main( int argc, char **argv )
{
    generateBoxes();
    generateRotationVectors();

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 400, 400 );

    cout << "Number of boxes: " << NUM_BOXES << endl;

    glutCreateWindow( "Collision Demo" );
    initRendering();

    glutDisplayFunc( drawScene );
    glutKeyboardFunc( handleKeypress );
    glutReshapeFunc( handleResize );

	// Add a timer to initiate the updating loop
	glutTimerFunc( 10, updateAngles, 0 );

    glutMainLoop();
    return 0;
}

