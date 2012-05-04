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
	glutIdleFunc( Window::renderScene );
	glutKeyboardFunc( Window::handleKeyDown );
	glutKeyboardUpFunc( Window::handleKeyUp );
	glutSpecialFunc( Window::handleSpecialKeyDown );
	glutSpecialUpFunc( Window::handleSpecialKeyUp );
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
	memset( &event, 0, sizeof( Event ) );

	event.type = KEY_PRESSED;
	event.keyData.keyCode = static_cast<KeyCode>( key );
	event.keyData.isAscii = true;

	int mod = glutGetModifiers();
	if( ( mod & GLUT_ACTIVE_CTRL ) != 0 )
	{
		event.keyData.ctrlPressed = true;
	}
	if( ( mod & GLUT_ACTIVE_ALT ) != 0 )
	{
		event.keyData.altPressed = true;
	}
	if( ( mod & GLUT_ACTIVE_SHIFT ) != 0 )
	{
		event.keyData.shiftPressed = true;
	}

	eventQueue.push( event );
}

void Window::handleKeyUp( unsigned char key, int x, int y )
{
	Event event;
	memset( &event, 0, sizeof( Event ) );

	event.type = KEY_RELEASED;
	event.keyData.keyCode = static_cast<KeyCode>( key );
	event.keyData.isAscii = true;

	eventQueue.push( event );
}

void Window::handleSpecialKeyDown( int key, int x, int y )
{
	Event event;
	memset( &event, 0, sizeof( Event ) );

	event.type = KEY_PRESSED;
	switch( key )
	{
		case KEY_F1:
			event.keyData.keyCode = KEY_F1;
			break;
		case KEY_F2:
			event.keyData.keyCode = KEY_F2;
			break;
		case KEY_F3:
			event.keyData.keyCode = KEY_F3;
			break;
		case KEY_F4:
			event.keyData.keyCode = KEY_F4;
			break;
		case KEY_F5:
			event.keyData.keyCode = KEY_F5;
			break;
		case KEY_F6:
			event.keyData.keyCode = KEY_F6;
			break;
		case KEY_F7:
			event.keyData.keyCode = KEY_F7;
			break;
		case KEY_F8:
			event.keyData.keyCode = KEY_F8;
			break;
		case KEY_F9:
			event.keyData.keyCode = KEY_F9;
			break;
		case KEY_F10:
			event.keyData.keyCode = KEY_F10;
			break;
		case KEY_F11:
			event.keyData.keyCode = KEY_F11;
			break;
		case KEY_F12:
			event.keyData.keyCode = KEY_F12;
			break;
		case KEY_LEFT_ARROW:
			event.keyData.keyCode = KEY_LEFT_ARROW;
			break;
		case KEY_RIGHT_ARROW:
			event.keyData.keyCode = KEY_RIGHT_ARROW;
			break;
		case KEY_DOWN_ARROW:
			event.keyData.keyCode = KEY_DOWN_ARROW;
			break;
		case KEY_UP_ARROW:
			event.keyData.keyCode = KEY_UP_ARROW;
			break;
		case KEY_PAGE_UP:
			event.keyData.keyCode = KEY_PAGE_UP;
			break;
		case KEY_PAGE_DOWN:
			event.keyData.keyCode = KEY_PAGE_DOWN;
			break;
		case KEY_HOME:
			event.keyData.keyCode = KEY_HOME;
			break;
		case KEY_END:
			event.keyData.keyCode = KEY_END;
			break;
		case KEY_INSERT:
			event.keyData.keyCode = KEY_INSERT;
			break;
	}

	int mod = glutGetModifiers();
	if( ( mod & GLUT_ACTIVE_CTRL ) != 0 )
	{
		event.keyData.ctrlPressed = true;
	}
	if( ( mod & GLUT_ACTIVE_ALT ) != 0 )
	{
		event.keyData.altPressed = true;
	}
	if( ( mod & GLUT_ACTIVE_SHIFT ) != 0 )
	{
		event.keyData.shiftPressed = true;
	}

	eventQueue.push( event );
}

void Window::handleSpecialKeyUp( int key, int x, int y )
{
	Event event;
	memset( &event, 0, sizeof( Event ) );

	event.type = KEY_RELEASED;
	switch( key )
	{
		case KEY_F1:
			event.keyData.keyCode = KEY_F1;
			break;
		case KEY_F2:
			event.keyData.keyCode = KEY_F2;
			break;
		case KEY_F3:
			event.keyData.keyCode = KEY_F3;
			break;
		case KEY_F4:
			event.keyData.keyCode = KEY_F4;
			break;
		case KEY_F5:
			event.keyData.keyCode = KEY_F5;
			break;
		case KEY_F6:
			event.keyData.keyCode = KEY_F6;
			break;
		case KEY_F7:
			event.keyData.keyCode = KEY_F7;
			break;
		case KEY_F8:
			event.keyData.keyCode = KEY_F8;
			break;
		case KEY_F9:
			event.keyData.keyCode = KEY_F9;
			break;
		case KEY_F10:
			event.keyData.keyCode = KEY_F10;
			break;
		case KEY_F11:
			event.keyData.keyCode = KEY_F11;
			break;
		case KEY_F12:
			event.keyData.keyCode = KEY_F12;
			break;
		case KEY_LEFT_ARROW:
			event.keyData.keyCode = KEY_LEFT_ARROW;
			break;
		case KEY_RIGHT_ARROW:
			event.keyData.keyCode = KEY_RIGHT_ARROW;
			break;
		case KEY_DOWN_ARROW:
			event.keyData.keyCode = KEY_DOWN_ARROW;
			break;
		case KEY_UP_ARROW:
			event.keyData.keyCode = KEY_UP_ARROW;
			break;
		case KEY_PAGE_UP:
			event.keyData.keyCode = KEY_PAGE_UP;
			break;
		case KEY_PAGE_DOWN:
			event.keyData.keyCode = KEY_PAGE_DOWN;
			break;
		case KEY_HOME:
			event.keyData.keyCode = KEY_HOME;
			break;
		case KEY_END:
			event.keyData.keyCode = KEY_END;
			break;
		case KEY_INSERT:
			event.keyData.keyCode = KEY_INSERT;
			break;
	}

	int mod = glutGetModifiers();
	if( ( mod & GLUT_ACTIVE_CTRL ) != 0 )
	{
		event.keyData.ctrlPressed = true;
	}
	if( ( mod & GLUT_ACTIVE_ALT ) != 0 )
	{
		event.keyData.altPressed = true;
	}
	if( ( mod & GLUT_ACTIVE_SHIFT ) != 0 )
	{
		event.keyData.shiftPressed = true;
	}

	eventQueue.push( event );
}

void Window::handleMouseMove( int x, int y )
{
	Event event;
	memset( &event, 0, sizeof( Event ) );

	event.type = MOUSE_MOVE;
	event.mousePosX = x;
	event.mousePosY = y;

	eventQueue.push( event );
}

void Window::handleMouseDrag( int x, int y )
{
	Event event;
	memset( &event, 0, sizeof( Event ) );

	event.type = MOUSE_MOVE;
	event.mousePosX= x;
	event.mousePosY = y;

	eventQueue.push( event );
}

void Window::handleMouseClick( int button, int state, int x, int y )
{
	Event event;
	memset( &event, 0, sizeof( Event ) );

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
}
