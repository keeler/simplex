#include "Window.hpp"
#include <cstring>

std::queue<Event> eventQueue;

Window::Window( unsigned int width, unsigned int height, const std::string & title ) :
	mWidth( width ),
	mHeight( height )
{
	int argc;
	char **argv = NULL;

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( width, height );
	glutCreateWindow( title.c_str() );

	Window::init3dRenderMode();

	glutReshapeFunc( Window::handleResize );
	glutDisplayFunc( Window::renderScene );
	glutKeyboardFunc( Window::handleKeyDown );
	glutSpecialFunc( Window::handleSpecialKey );
	glutKeyboardUpFunc( Window::handleKeyUp );
	glutPassiveMotionFunc( Window::handleMouseMove );
	glutMotionFunc( Window::handleMouseDrag );
	glutMouseFunc( Window::handleMouseClick );
}

Window::~Window()
{
}

void Window::beginRendering()
{
	glutMainLoop();
}

bool Window::getEvent( Event & event ) const
{
	if( !eventQueue.empty() )
	{
		event = eventQueue.front();
		eventQueue.pop();
		return true;
	}
	else
	{
		return false;
	}
}

void Window::init3dRenderMode()
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
}

void Window::handleResize( int width, int height )
{
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0, (float)width / (float)height, 1.0, 1000.0 );
}

void Window::handleKeyDown( unsigned char key, int x, int y )
{
	Event event;
	memset( &event, -1, sizeof( Event ) );

	event.type = KEY_DOWN;
	event.key.keyCode = ASCII_KEY;
	event.key.asciiCode = key;

	eventQueue.push( event );
	glutPostRedisplay();
}

void Window::handleSpecialKey( int key, int x, int y )
{
	Event event;
	memset( &event, -1, sizeof( Event ) );

	event.type = KEY_DOWN;
	switch( key )
	{
		case F1:
			event.key.keyCode = F1;
			break;
		case F2:
			event.key.keyCode = F2;
			break;
		case F3:
			event.key.keyCode = F3;
			break;
		case F4:
			event.key.keyCode = F4;
			break;
		case F5:
			event.key.keyCode = F5;
			break;
		case F6:
			event.key.keyCode = F6;
			break;
		case F7:
			event.key.keyCode = F7;
			break;
		case F8:
			event.key.keyCode = F8;
			break;
		case F9:
			event.key.keyCode = F9;
			break;
		case F10:
			event.key.keyCode = F10;
			break;
		case F11:
			event.key.keyCode = F11;
			break;
		case F12:
			event.key.keyCode = F12;
			break;
		case LEFT_ARROW:
			event.key.keyCode = LEFT_ARROW;
			break;
		case RIGHT_ARROW:
			event.key.keyCode = RIGHT_ARROW;
			break;
		case DOWN_ARROW:
			event.key.keyCode = DOWN_ARROW;
			break;
		case UP_ARROW:
			event.key.keyCode = UP_ARROW;
			break;
		case PAGE_UP:
			event.key.keyCode = PAGE_UP;
			break;
		case PAGE_DOWN:
			event.key.keyCode = PAGE_DOWN;
			break;
		case HOME:
			event.key.keyCode = HOME;
			break;
		case END:
			event.key.keyCode = END;
			break;
		case INSERT:
			event.key.keyCode = INSERT;
			break;
	}

	eventQueue.push( event );
	glutPostRedisplay();
}

void Window::handleKeyUp( unsigned char key, int x, int y )
{
	Event event;
	memset( &event, -1, sizeof( Event ) );

	event.type = KEY_UP;
	event.key.keyCode = ASCII_KEY;
	event.key.asciiCode = key;

	eventQueue.push( event );
	glutPostRedisplay();
}

void Window::handleMouseMove( int x, int y )
{
	Event event;
	memset( &event, -1, sizeof( Event ) );

	event.type = MOUSE_MOVE;
	event.mousePosX = x;
	event.mousePosY = y;

	eventQueue.push( event );
	glutPostRedisplay();
}

void Window::handleMouseDrag( int x, int y )
{
	Event event;
	memset( &event, -1, sizeof( Event ) );

	event.type = MOUSE_MOVE;
	event.mousePosX= x;
	event.mousePosY = y;

	eventQueue.push( event );
	glutPostRedisplay();
}

void Window::handleMouseClick( int button, int state, int x, int y )
{
	Event event;
	memset( &event, -1, sizeof( Event ) );

	event.mousePosX = x;
	event.mousePosY = y;
	switch( button )
	{
		case LEFT_BUTTON:
			event.mouseButton = LEFT_BUTTON;
			break;
		case RIGHT_BUTTON:
			event.mouseButton = RIGHT_BUTTON;
			break;
		case SCROLL_UP:
			event.mouseButton = SCROLL_UP;
			break;
		case SCROLL_DOWN:
			event.mouseButton = SCROLL_DOWN;
			break;
	}

	if( state == GLUT_DOWN )
	{
		event.type = MOUSE_CLICK;
	}
	else if( state == GLUT_UP )
	{
		event.type = MOUSE_UNCLICK;
	}
	else
	{
		event.type = MOUSE_SCROLL;
	}

	eventQueue.push( event );
	glutPostRedisplay();
}

