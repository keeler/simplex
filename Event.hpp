#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include "GL/glut.h"

enum EventType
{
	KEY_PRESSED,
	KEY_RELEASED,
	MOUSE_MOVE,
	MOUSE_CLICK,
	MOUSE_UNCLICK,
	MOUSE_SCROLL
};	

enum KeyCode
{
	KEY_a = 'a',
	KEY_b = 'b',
	KEY_c = 'c',
	KEY_d = 'd',
	KEY_e = 'e',
	KEY_f = 'f',
	KEY_g = 'g',
	KEY_h = 'h',
	KEY_i = 'i',
	KEY_j = 'j',
	KEY_k = 'k',
	KEY_l = 'l',
	KEY_m = 'm',
	KEY_n = 'n',
	KEY_o = 'o',
	KEY_p = 'p',
	KEY_q = 'q',
	KEY_r = 'r',
	KEY_s = 's',
	KEY_t = 't',
	KEY_u = 'u',
	KEY_v = 'v',
	KEY_w = 'w',
	KEY_x = 'x',
	KEY_y = 'y',
	KEY_z = 'z',
	KEY_A = 'A',
	KEY_B = 'B',
	KEY_C = 'C',
	KEY_D = 'D',
	KEY_E = 'E',
	KEY_F = 'F',
	KEY_G = 'G',
	KEY_H = 'H',
	KEY_I = 'I',
	KEY_J = 'J',
	KEY_K = 'K',
	KEY_L = 'L',
	KEY_M = 'M',
	KEY_N = 'N',
	KEY_O = 'O',
	KEY_P = 'P',
	KEY_Q = 'Q',
	KEY_R = 'R',
	KEY_S = 'S',
	KEY_T = 'T',
	KEY_U = 'U',
	KEY_V = 'V',
	KEY_W = 'W',
	KEY_X = 'X',
	KEY_Y = 'Y',
	KEY_Z = 'Z',
	KEY_0 = '0',
	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',
	KEY_4 = '4',
	KEY_5 = '5',
	KEY_6 = '6',
	KEY_7 = '7',
	KEY_8 = '8',
	KEY_9 = '9',
	KEY_EXCLAMATION = '!',
	KEY_AT = '@',
	KEY_HASH = '#',
	KEY_DOLLAR = '$',
	KEY_PERCENT = '%',
	KEY_CARET = '^',
	KEY_AMPERSAND = '&',
	KEY_TIMES = '*',
	KEY_OPEN_PAREN = '(',
	KEY_CLOSE_PAREN = ')',
	KEY_UNDERSCORE = '_',
	KEY_MINUS = '-', 
	KEY_EQUALS = '=',
	KEY_PLUS = '+',
	KEY_OPEN_BRACKET = '[',
	KEY_CLOSE_BRACKET = ']',
	KEY_OPEN_BRACE = '{',
	KEY_CLOSE_BRACE = '}',
	KEY_PIPE = '|',
	KEY_BACKSLASH = '\\',
	KEY_SEMICOLON = ';',
	KEY_COLON = ':',
	KEY_SINGLE_QUOTE = '\'',
	KEY_DOUBLE_QUOTE = '\"',
	KEY_COMMA = ',', 
	KEY_PERIOD = '.',
	KEY_DIVIDE = '/',
	KEY_LESS = '<',
	KEY_GREATER = '>',
	KEY_QUESTION = '?',
	KEY_ESCAPE = 27,
	KEY_TAB = '\t',
	KEY_RETURN = '\r',
	KEY_BACKSPACE = '\b',
	KEY_F1 = GLUT_KEY_F1,
	KEY_F2 = GLUT_KEY_F2,
	KEY_F3 = GLUT_KEY_F3,
	KEY_F4 = GLUT_KEY_F4,
	KEY_F5 = GLUT_KEY_F5,
	KEY_F6 = GLUT_KEY_F6,
	KEY_F7 = GLUT_KEY_F7,
	KEY_F8 = GLUT_KEY_F8,
	KEY_F9 = GLUT_KEY_F9,
	KEY_F10 = GLUT_KEY_F10,
	KEY_F11 = GLUT_KEY_F11,
	KEY_F12 = GLUT_KEY_F12,
	KEY_LEFT_ARROW = GLUT_KEY_LEFT,
	KEY_RIGHT_ARROW = GLUT_KEY_RIGHT,
	KEY_UP_ARROW = GLUT_KEY_UP,
	KEY_DOWN_ARROW = GLUT_KEY_DOWN,
	KEY_PAGE_UP = GLUT_KEY_PAGE_UP,
	KEY_PAGE_DOWN = GLUT_KEY_PAGE_DOWN,
	KEY_HOME = GLUT_KEY_HOME,
	KEY_END = GLUT_KEY_END,
	KEY_INSERT = GLUT_KEY_INSERT
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
	KeyCode		keyCode;
	bool		isAscii;
	bool		altPressed;
	bool		ctrlPressed;
	bool		shiftPressed;
};

struct Event
{
	EventType			type;
	KeyEvent			keyData;
	int					mousePosX;
	int					mousePosY;
	MouseButton			mouseButton;
};

#endif
