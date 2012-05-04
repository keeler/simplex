#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include "GL/glut.h"

enum EventType
{
	KEY_DOWN,
	KEY_UP,
	MOUSE_MOVE,
	MOUSE_CLICK,
	MOUSE_UNCLICK,
	MOUSE_SCROLL
};	

enum KeyCode
{
	ASCII_KEY,
	F1 = GLUT_KEY_F1,
	F2 = GLUT_KEY_F2,
	F3 = GLUT_KEY_F3,
	F4 = GLUT_KEY_F4,
	F5 = GLUT_KEY_F5,
	F6 = GLUT_KEY_F6,
	F7 = GLUT_KEY_F7,
	F8 = GLUT_KEY_F8,
	F9 = GLUT_KEY_F9,
	F10 = GLUT_KEY_F10,
	F11 = GLUT_KEY_F11,
	F12 = GLUT_KEY_F12,
	LEFT_ARROW = GLUT_KEY_LEFT,
	RIGHT_ARROW = GLUT_KEY_RIGHT,
	UP_ARROW = GLUT_KEY_UP,
	DOWN_ARROW = GLUT_KEY_DOWN,
	PAGE_UP = GLUT_KEY_PAGE_UP,
	PAGE_DOWN = GLUT_KEY_PAGE_DOWN,
	HOME = GLUT_KEY_HOME,
	END = GLUT_KEY_END,
	INSERT = GLUT_KEY_INSERT
};

enum MouseButton
{
	LEFT_BUTTON = GLUT_LEFT_BUTTON,
	RIGHT_BUTTON = GLUT_RIGHT_BUTTON,
	SCROLL_UP = 3,
	SCROLL_DOWN = 4
};

struct KeyEvent
{
	KeyCode			keyCode;
	unsigned char	asciiCode;
};

struct Event
{
	EventType			type;
	KeyEvent			key;
	int					mousePosX;
	int					mousePosY;
	MouseButton			mouseButton;
};

#endif
