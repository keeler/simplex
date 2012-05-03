#include "GL/glut.h"
#include "../ImageLoader.hpp"
#include "TerrainPreview.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <cstdlib>
using namespace std;

#define PI 3.14159265f

#define DEFAULT_WINDOW_WIDTH 400
#define DEFAULT_WINDOW_HEIGHT 400

#define GLUT_SCROLL_UP 3
#define GLUT_SCROLL_DOWN 4

// Globals
bool _keyState[256] = { false };    // Assume no keys pressed at first
bool _mouseDown = false;
float _paintbrushRadius = 20;
int _imageWidth = 0;
int _imageHeight = 0;
string _imageFilename;
int _mousePosX = DEFAULT_WINDOW_WIDTH / 2;
int _mousePosY = DEFAULT_WINDOW_HEIGHT / 2;
float _changeIntensity = 30.0f;
unsigned char *_editImagePixels = NULL;		// Array of RGB triples
unsigned int _editImageTextureId;
bool _raiseHillMode = true;		// If not true, we're in lowerValley mode
bool _fileExists = false;		// Whether or not the file we're trying to load exists
TerrainPreview *_myTerrainPreview = NULL;
bool _previewMode = false;	// Start in edit mode
float _terrainViewAngle = 0.0f;

// OpenGL necessities
void init2DRendering();
void init3DRendering();
void handleResize( int width, int height );
void handleKeyDown( unsigned char key, int x, int y );
void handleKeyUp( unsigned char key, int x, int y );
void handleKeyPress();
void handleMouseMove( int x, int y );
void handleMouseDrag( int x, int y );
void handleMouseClick( int button, int state, int x, int y );
void updateTerrainViewAngle( int value );
void drawScene();
// Helper functions
void initializeEditImage();		// Allocates and initializes a new blank image
void cleanUp();
void floodFill( int r, int g, int b );
void raiseHill( float changeIntensity );
void lowerValley( float changeIntensity );
void twoPassGaussianBlur();    // Blurs entire image
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

	_imageFilename = argv[1];
	_imageWidth = atoi( argv[2] );
	_imageHeight = atoi( argv[3] );

	ifstream infile( _imageFilename.c_str() );
	// If file already exists, open it for editing
	if( infile )
	{
		_fileExists = true;
		infile.close();
	}
	if( _fileExists )
	{
		loadBitmap( _imageFilename, _editImagePixels, _imageWidth, _imageHeight );
	}

	_mousePosX = _imageWidth / 2;
	_mousePosY = _imageHeight / 2;

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( _imageWidth, _imageHeight );
	
	glutCreateWindow( "Heightmap Editor Tool" );
	init2DRendering();    // Start in 2D editing mode

	// If the file didn't exist, and nothing was loaded in, initialize a blank slate
	if( !_fileExists )
	{
		initializeEditImage();
	}

	_myTerrainPreview = new TerrainPreview( _imageWidth, _imageHeight, ( _imageWidth + _imageHeight ) / 5.0f );

	// Create a texture for the image so we can map it onto a quad.
	// This texture creation should only be done after init2DRendering() is called
	// since that function enables GL_TEXTURE_RECTANGLE_ARB
	glGenTextures( 1, &_editImageTextureId );
	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, _editImageTextureId );
	glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, _imageWidth, _imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _editImagePixels );

	glutDisplayFunc( drawScene );
	glutReshapeFunc( handleResize );
	glutKeyboardFunc( handleKeyDown );
	glutKeyboardUpFunc( handleKeyUp );
	glutPassiveMotionFunc( handleMouseMove );
	glutMotionFunc( handleMouseDrag );
	glutMouseFunc( handleMouseClick );
	glutTimerFunc( 25, updateTerrainViewAngle, 0 );

	cout << "Controls:" << endl;
	cout << "\tm :: Toggle hill-making mode/valley-making mode." << endl;
	cout << "\tb :: Blur the image (makes terrain smoother)." << endl;
	cout << "\tp :: Toggle edit mode/terrain preview mode." << endl;
	cout << "\ts :: Save the image to filename supplied at startup." << endl;
	cout << "\tscroll-wheel :: Increase/decrease paintbrush radius." << endl;
	
	glutMainLoop();
	return 0;
}

void init2DRendering()
{
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glDepthMask( GL_TRUE );
	glEnable( GL_TEXTURE_RECTANGLE_ARB );
	// Make it so mouse arrow cursor doesn't show in the window
	glutSetCursor( GLUT_CURSOR_NONE );
}

void init3DRendering()
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
}

void handleResize( int width, int height )
{
	// If not preview mode, we're in edit mode, so we want the screen
	// to work in 2D
	if( !_previewMode )
	{
		init2DRendering();
		glViewport( 0, 0, width, height );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( 0, width, height, 0, -1, 1 );
		glMatrixMode( GL_MODELVIEW );
	}
	// Otherwise, we're trying to preview the terrain, and we want to
	// enable 3D viewpoint
	else
	{
		init3DRendering();
		glViewport( 0, 0, width, height );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 45.0, (float)width / (float)height, 1.0, 1000.0 );
	}
}

void updateTerrainViewAngle( int value )
{
	_terrainViewAngle += 3.0f;
	if( _terrainViewAngle > 360 )
	{
		_terrainViewAngle -= 360;
	}

	glutPostRedisplay();
	glutTimerFunc( 25, updateTerrainViewAngle, 0 );
}

void drawScene()
{
	handleResize( _imageWidth, _imageHeight );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	handleKeyPress();

	if( !_previewMode )
	{
		if( _mouseDown )
		{
			if( _raiseHillMode )
			{
				raiseHill( _changeIntensity );
			}
			else
			{
				lowerValley( _changeIntensity );
			}
		}

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
	}
	else
	{
		glTranslatef( 0.0f, 0.0f, -10.0f );
		glRotatef( 30.0f, 1.0f, 0.0f, 0.0f );
		glRotatef( -_terrainViewAngle, 0.0f, 1.0f, 0.0f );
		
		GLfloat ambientColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambientColor );
		
		GLfloat lightColor0[] = { 0.6f, 0.6f, 0.6f, 1.0f };
		GLfloat lightPos0[] = { -0.5f, 100 + ( _imageWidth + _imageHeight ) / 5.0f, 0.1f, 0.0f };
		glLightfv( GL_LIGHT0, GL_DIFFUSE, lightColor0 );
		glLightfv( GL_LIGHT0, GL_POSITION, lightPos0 );
		
		float scale = 5.0f / max(_imageWidth - 1, _imageHeight - 1);
		glScalef( scale, scale, scale );
		glTranslatef( -(float)( _imageWidth - 1 ) / 2, 0.0f, -(float)( _imageHeight - 1 ) / 2 );

		_myTerrainPreview->render();
	}

	glutSwapBuffers();
}

void handleKeyDown( unsigned char key, int x, int y )
{
	// Escape key
	if( key == 27 )
	{
		cleanUp();
		exit( 1 );
	}
	// Blurr the image
	if( key == 'b' )
	{
		twoPassGaussianBlur();
		_myTerrainPreview->setHeightmap( _editImagePixels );
	}
	// Change the function of the paintbrush
	if( key == 'm' )
	{
		_raiseHillMode = !_raiseHillMode;
	}
	// Save the image
	if( key == 's' )
	{
		saveBitmap( _imageFilename, _editImagePixels, _imageWidth, _imageHeight );
	}
	if( key == 'p' )
	{
		// If in edit mode, change mode to show preview of terrain
		if( !_previewMode )
		{
			init3DRendering();
			_myTerrainPreview->setHeightmap( _editImagePixels );
		}
		else
		{
			init2DRendering();
		}

		_previewMode = !_previewMode;
	}

	_keyState[key] = true;
}

void handleKeyUp( unsigned char key, int x, int y )
{
	_keyState[key] = false;
}

void handleKeyPress()
{
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

	glutPostRedisplay();
}

void handleMouseClick( int button, int state, int x, int y )
{
	_mousePosX = x;
	_mousePosY = y;

	if( button == GLUT_LEFT_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			_mouseDown = true;
		}
		else
		{
			_mouseDown = false;
		}
	}
	else if( button == GLUT_SCROLL_UP )
	{
		_paintbrushRadius += 2.0f;
	}
	else if( button == GLUT_SCROLL_DOWN )
	{
		if( _paintbrushRadius >= 2.0f )
		{
			_paintbrushRadius -= 2.0f;
		}
	}

	glutPostRedisplay();
}

void initializeEditImage()
{
	_editImagePixels = new unsigned char[_imageWidth * _imageHeight * 3];

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
				float distanceSquared = ( x - _mousePosX ) * ( x - _mousePosX ) + ( y - _mousePosY ) * ( y - _mousePosY );
				if( distanceSquared <= _paintbrushRadius * _paintbrushRadius )
				{
					// Note that it's ( _imageHeight - y ) because when you pass the
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

void raiseHill( float changeIntensity )
{
	for( int x = _mousePosX - _paintbrushRadius; x < _mousePosX + _paintbrushRadius; x++ )
	{
		for( int y = _mousePosY - _paintbrushRadius; y < _mousePosY + _paintbrushRadius; y++ )
		{
			// Check to ensure we don't access something outside the image
			if( x > 0 && x < _imageWidth && y > 0 && y < _imageHeight )
			{
				float distance = sqrt( pow( x - _mousePosX, 2 ) + pow( y - _mousePosY, 2 ) );
				if( distance <= _paintbrushRadius )
				{
					// Note that it's ( _imageHeight - y ) because when you pass the
					// pixel array to glTexSubImage2D, the vertical axis is flipped
					unsigned char delta = (0.5)*changeIntensity * ( ( -distance / _paintbrushRadius ) + 1 );
					// Red component
					if( _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 0] + delta < 255 )
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 0] += delta;
					}
					else
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 0] = 255;
					}

					// Green component
					if( _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 1] + delta < 255 )
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 1] += delta;
					}
					else
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 1] = 255;
					}

					// Blue component
					if( _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 2] + delta < 255 )
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 2] += delta;
					}
					else
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 2] = 255;
					}
				}
			}
		}
	}

	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, _editImageTextureId );
	glTexSubImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, _imageWidth, _imageHeight, GL_RGB, GL_UNSIGNED_BYTE, _editImagePixels );
}

void lowerValley( float changeIntensity )
{
	for( int x = _mousePosX - _paintbrushRadius; x < _mousePosX + _paintbrushRadius; x++ )
	{
		for( int y = _mousePosY - _paintbrushRadius; y < _mousePosY + _paintbrushRadius; y++ )
		{
			// Check to ensure we don't access something outside the image
			if( x > 0 && x < _imageWidth && y > 0 && y < _imageHeight )
			{
				float distance = sqrt( pow( x - _mousePosX, 2 ) + pow( y - _mousePosY, 2 ) );
				if( distance <= _paintbrushRadius )
				{
					// Note that it's ( _imageHeight - y ) because when you pass the
					// pixel array to glTexSubImage2D, the vertical axis is flipped
					unsigned char delta = (0.5)*changeIntensity * ( ( -distance / _paintbrushRadius ) + 1 );
					// Red component
					if( _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 0] - delta > 0 )
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 0] -= delta;
					}
					else
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 0] = 0;
					}

					// Green component
					if( _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 1] - delta > 0 )
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 1] -= delta;
					}
					else
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 1] = 0;
					}

					// Blue component
					if( _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 2] - delta > 0 )
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 2] -= delta;
					}
					else
					{
						_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + 2] = 0;
					}
				}
			}
		}
	}

	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, _editImageTextureId );
	glTexSubImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, _imageWidth, _imageHeight, GL_RGB, GL_UNSIGNED_BYTE, _editImagePixels );
}

// Blurs entire image using two-pass Gaussian blur (using standard deviation of 2.7)
void twoPassGaussianBlur()
{
	// Blur horizontally for each row
	for( int y = 0; y < _imageHeight; y++ )
	{
		for( int x = 0; x < _imageWidth; x++ )
		{
			// Take 9 samples, set current pixel to Gaussian average of these samples
			float sum[3] = { 0 };    // One for r, g, and b
			if( x - 4 > 0 )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x - 4 ) + i] * 0.05f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.05f;
				}
			}
			if( x - 3 > 0 )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x - 3 ) + i] * 0.09f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.09f;
				}
			}
			if( x - 2 > 0 )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x - 2 ) + i] * 0.12f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.12f;
				}
			}
			if( x - 1 > 0 )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x - 1 ) + i] * 0.15f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.15f;
				}
			}
			// Center sample
			for( int i = 0; i < 3; i++ )
			{
				sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.16f;
			}
			if( x + 1 < _imageWidth )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x + 1 ) + i] * 0.15f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.15f;
				}
			}
			if( x + 2 < _imageWidth )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x + 2 ) + i] * 0.12f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.12f;
				}
			}
			if( x + 3 < _imageWidth )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x + 3 ) + i] * 0.09f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.09f;
				}
			}
			if( x + 4 < _imageWidth )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x + 4 ) + i] * 0.05f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.05f;
				}
			}

			// Set the value of the pixel
			for( int i = 0; i < 3; i++ )
			{
				if( sum[i] < 255 )
				{
					_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] = sum[i];
				}
				else
				{
					_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] = 255;
				}
			}
		}
	}

	// Blur vertically for each column
	for( int x = 0; x < _imageWidth; x++ )
	{
		for( int y = 0; y < _imageHeight; y++ )
		{
			// Take 9 samples, set current pixel to Gaussian average of these samples
			float sum[3] = { 0 };    // One for r, g, and b
			if( y - 4 > 0 )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - ( y - 4 ) ) * _imageWidth + x ) + i] * 0.05f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.05f;
				}
			}
			if( y - 3 > 0 )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - ( y - 3 ) ) * _imageWidth + x ) + i] * 0.09f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.09f;
				}
			}
			if( y - 2 > 0 )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - ( y - 2 ) ) * _imageWidth + x ) + i] * 0.12f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.12f;
				}
			}
			if( y - 1 > 0 )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - ( y - 1 ) ) * _imageWidth + x ) + i] * 0.15f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.15f;
				}
			}
			// Center sample
			for( int i = 0; i < 3; i++ )
			{
				sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.16f;
			}
			if( y + 1 < _imageHeight )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - ( y + 1 ) ) * _imageWidth + x ) + i] * 0.15f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.15f;
				}
			}
			if( y + 2 < _imageHeight )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - ( y + 2 ) ) * _imageWidth + x ) + i] * 0.12f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.12f;
				}
			}
			if( y + 3 < _imageHeight )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - ( y + 3 ) ) * _imageWidth + x ) + i] * 0.09f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.09f;
				}
			}
			if( y + 4 < _imageHeight )
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - ( y + 4 ) ) * _imageWidth + x ) + i] * 0.05f;
				}
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					sum[i] += _editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] * 0.05f;
				}
			}

			// Set the value of the pixel
			for( int i = 0; i < 3; i++ )
			{
				if( sum[i] < 255 )
				{
					_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] = sum[i];
				}
				else
				{
					_editImagePixels[3 * ( ( _imageHeight - y ) * _imageWidth + x ) + i] = 255;
				}
			}
		}
	}

	// Apply the blurred image
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
