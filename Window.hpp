#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include "GL/glut.h"
#include "Event.hpp"
#include <queue>
#include <string>

class Window
{
	public:
		Window( unsigned int width, unsigned int height, const std::string & title );
		~Window();

		bool getEvent( Event & event ) const;
		void beginRendering();

		int getWidth() const { return mWidth; };
		int getHeight() const { return mHeight; };

	private:
		// GLUT callback functions for handling events
		static void handleResize( int width, int height );
		static void handleKeyDown( unsigned char key, int x, int y );
		static void handleKeyUp( unsigned char key, int x, int y );
		static void handleSpecialKeyDown( int key, int x, int y );
		static void handleSpecialKeyUp( int key, int x, int y );
		static void handleMouseMove( int x, int y );
		static void handleMouseDrag( int x, int y );
		static void handleMouseClick( int button, int state, int x, int y );

		static void init3dRenderMode();
		// All rendering should go inside this function, as it is the callback
		// passed to glutDisplayFunc.
		static void renderScene();

		unsigned int	mWidth;
		unsigned int	mHeight;
};

#endif
