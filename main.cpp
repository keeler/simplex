#include "Vector3f.hpp"
#include "OrientedBoundingBox.hpp"
#include "Octree.hpp"
#include "Camera.hpp"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <cstdlib>
#include <map>
#include "GL/glut.h"
using namespace std;

// Should be anything more than 20, see lines 29-31, NUM_BOXES/20
const int NUM_BOXES = 3000;

bool keyState[256] = { false };

// For mouse motion
bool _mouseWarped = false;
float _mouseSensitivity = 0.1f;

int _index = 0;
float _angle = 30.0f;
OrientedBoundingBox * _myBoxes;
Vector3f * _rotationVectors;
float * _rotationRates;

Octree * _myOctree;
Camera * _myCamera;

void generateBoxes()
{
    _myBoxes = new OrientedBoundingBox[NUM_BOXES];

    Vector3f center;
    Vector3f edgeHalfLengths;
    Quaternion defaultOrientation;

    srand( time( NULL ) );
    for( int i = 0; i < NUM_BOXES; i++ )
    {
        center = Vector3f( (rand()%2?-1.0f:1.0f)*(rand()%(NUM_BOXES/20))/1.0,
                           (rand()%2?-1.0f:1.0f)*(rand()%(NUM_BOXES/20))/1.0,
                           (rand()%2?-1.0f:1.0f)*(rand()%(NUM_BOXES/20))/1.0 );
        edgeHalfLengths = Vector3f( 0.5f, 1.0f, 2.0f );

        _myBoxes[i] = OrientedBoundingBox( center, edgeHalfLengths, defaultOrientation );
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
        _rotationRates[i] = (rand()%7+3)/1.0;
    }
}

void keyDown( unsigned char key, int x, int y )
{
	if( key == 27 )
	{
        delete [] _myBoxes;
        delete [] _rotationVectors;
        delete [] _rotationRates;
        delete _myOctree;
		delete _myCamera;
		exit( 1 );
	}

	keyState[ key ] = true;
}

void keyUp( unsigned char key, int x, int y )
{
	keyState[ key ] = false;
}

void handleKeyPress()
{
	if( keyState['w'] )
	{
		_myCamera->moveForward( 5.0f );
	}
	if( keyState['s'] )
	{
		_myCamera->moveBackward( 5.0f );
	}
	if( keyState['a'] )
	{
		_myCamera->moveLeft( 5.0f );
	}
	if( keyState['d'] )
	{
		_myCamera->moveRight( 5.0f );
    }

	glutPostRedisplay();
}

void mouseMove( int x, int y )
{
	int width = glutGet( GLUT_WINDOW_WIDTH );
	int height = glutGet( GLUT_WINDOW_HEIGHT );

	if( !_mouseWarped )
	{
		int deltaX = x - width / 2;
		int deltaY = y - height / 2;

		_myCamera->rotateYaw( _mouseSensitivity * deltaX );
		_myCamera->rotatePitch( _mouseSensitivity * deltaY );

		glutWarpPointer( width / 2, height / 2 );
		_mouseWarped = true;
	}
	else
	{
		_mouseWarped = false;
	}

	glutPostRedisplay();
}

void initRendering()
{
    // Makes shit render in 3d, man
    glEnable( GL_DEPTH_TEST );
	glEnable( GL_COLOR_MATERIAL );    // Allows us to color polygons

    // Use back-face culling
    glCullFace( GL_BACK );

    glEnable( GL_LIGHTING );    // Allows us to use light
    glEnable( GL_LIGHT0 );
  //  glEnable( GL_NORMALIZE );

    glShadeModel( GL_SMOOTH );

	// Make it so mouse doesn't show up when we move around
	glutSetCursor( GLUT_CURSOR_NONE ); 
}

void handleResize( int width, int height )
{
    // Tells OpenGL how to convert from coordinates to pixel values
    glViewport( 0, 0, width, height );
    // Switch to setting the camera perspective
    glMatrixMode( GL_PROJECTION );
    // Set the camera perspective
    glLoadIdentity();    // Reset the camera
    gluPerspective( 45.0, (double)width / (double)height, 1.0, 1000.0 );
}

void drawScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	handleKeyPress();
	_myCamera->look();

	glTranslatef( 0.0f, 0.0f, -30.0f );
    glColor3f( 1.0f, 1.0f, 1.0f );
	// Add positioned light 0
	GLfloat lightColor0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPos0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightColor0 );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPos0 );

 //   _myOctree->drawOctreeFrame( Vector3f( 1.0f, 1.0f, 1.0f ) );

    map<OrientedBoundingBox *, bool> hit;
    for( int i = 0; i < NUM_BOXES; i++ )
    {
        hit[ &_myBoxes[i] ] = false;
    }

    vector<BoxPair> collisionPairs;
    _myOctree->getPotentialCollisionPairs( collisionPairs );
 //   cout << "Potential collisions " << collisionPairs.size() << endl;
    for( unsigned int i = 0; i < collisionPairs.size(); i++ )
    {
        if( collisionPairs[i].box1->collisionWith( *collisionPairs[i].box2 ) )
        {
            hit[ collisionPairs[i].box1 ] = true;
            hit[ collisionPairs[i].box2 ] = true;
        }
    }

    for( map<OrientedBoundingBox *, bool>::iterator it = hit.begin();
         it != hit.end();
         ++it )
    {
        if( it->second )
        {
            it->first->draw( Vector3f( 1.0f, 0.0f, 0.0f ) );
        }
        else
        {
            it->first->draw( Vector3f( 0.0f, 1.0f, 1.0f ) );
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
	glutTimerFunc( 25, updateAngles, 0 );
}

int main( int argc, char **argv )
{
    generateBoxes();
    generateRotationVectors();

    _myOctree = new Octree( Vector3f( -NUM_BOXES / 20.0f, -NUM_BOXES / 20.0f, -NUM_BOXES / 20.0f ),
                            Vector3f( NUM_BOXES / 20.0f, NUM_BOXES / 20.0f, NUM_BOXES / 20.0f ) );
    for( int i = 0; i < NUM_BOXES; i++ )
    {
        _myOctree->addBox( &_myBoxes[i] );
    }

	_myCamera = new Camera( Vector3f( 0.0f, 0.0f, 0.0f ), 0.0f, 0.0f );
	_myCamera->setPosition( Vector3f( 0.0f, 0.0f, -100.0f ) );

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 400, 400 );

    cout << "Number of boxes: " << NUM_BOXES << endl;

    glutCreateWindow( "Octree Demo" );
    initRendering();

    glutDisplayFunc( drawScene );
    glutKeyboardFunc( keyDown );
	glutKeyboardUpFunc( keyUp );
	glutReshapeFunc( handleResize );

	glutPassiveMotionFunc( mouseMove );

	// Add a timer to initiate the updating loop
	glutTimerFunc( 25, updateAngles, 0 );

    glutMainLoop();
    return 0;
}

