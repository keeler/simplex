#include "Texture.hpp"
#include "ImageLoader.hpp"
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

Texture::Texture( const std::string filename )
{
	this->loadBmp( filename );
}

Texture::~Texture()
{
}

void Texture::loadBmp( const std::string & filename )
{
	char *pixels = NULL;
	int width;
	int height;

	loadBitmap( filename, pixels, width, height );

	mFilename = filename;
	glGenTextures( 1, &mTextureId );
	glBindTexture( GL_TEXTURE_2D, mTextureId );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

	if( pixels != NULL )
	{
		delete [] pixels;
	}
}
