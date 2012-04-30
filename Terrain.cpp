/*
* Modified from http://www.videotutorialsrock.com
*/

#include "Terrain.hpp"
#include "Texture.hpp"
#include "ImageLoader.hpp"
#include "GL/glut.h"

Terrain::Terrain( const std::string & heightmapFilename, const std::string & textureFilename, const float heightScale )
{
	mTexture = new Texture( textureFilename );

	char *pixels = NULL;
	loadBitmap( heightmapFilename, pixels, mWidth, mLength );

	mHeightMap = new float[mWidth * mLength];
	for( int z = 0; z < mLength; z++ )
	{
		for( int x = 0; x < mWidth; x++ )
		{
			// Only need one channel, arbitrarily take red channel
			unsigned char color = (unsigned char)pixels[3 * ( mWidth * x + z )];
			// Varies from - height / 2 to height / 2
			mHeightMap[mWidth * x + z] = heightScale * ( ( color / 255.0f ) - 0.5f );
		}
	}

	computeNormals();
}

Terrain::~Terrain()
{
	delete [] mHeightMap;
	delete [] mNormals;
	delete mTexture;
}

void Terrain::render() const
{
	mTexture->bind();

	// Texturing quads (each of which is 2 triangles) is a lot easier than
	// texturing triangles themselves. Texture a square group of quads, where
	// the reciprocal of the delta below is the number of quads along one edge
	// of the square.
	float textureXCoord = 0.0f;
	float textureYCoord = 0.0f;
	float textureCoordDelta = 0.1f;

	for( int z = 0; z < mLength - 1; z++ )
	{
		glBegin( GL_TRIANGLE_STRIP );
		for( int x = 0; x < mWidth; x++ )
		{
			Vector3f normal = mNormals[x * mWidth + z];
			glNormal3f( normal[0], normal[1], normal[2] );
			glTexCoord2f( textureXCoord, textureYCoord );
			glVertex3f( x, mHeightMap[mWidth * x + z], z );

			normal = mNormals[x * mWidth + ( z + 1 )];
			glNormal3f( normal[0], normal[1], normal[2] );
			glTexCoord2f( textureXCoord, textureYCoord + 0.25f );
			glVertex3f( x, mHeightMap[mWidth * x + ( z + 1 )], z + 1 );

			textureXCoord += textureCoordDelta;
		}
		glEnd();

		textureYCoord += textureCoordDelta;
	}
}

void Terrain::computeNormals()
{
	Vector3f *tempNormals = new Vector3f[mWidth * mLength];
	for( int x = 0; x < mWidth; x++ )
	{
		for( int z = 0; z < mLength; z++ )
		{
			Vector3f sum( 0.0f, 0.0f, 0.0f );

			Vector3f left( 0.0f, 0.0f, 0.0f );
			if( x > 0 )
			{
				left = Vector3f( -1.0, this->getHeight( x - 1, z ) - this->getHeight( x, z ), 0.0f );
			}

			Vector3f right( 0.0f, 0.0f, 0.0f );
			if( x < mWidth - 1 )
			{
				right = Vector3f( 1.0, this->getHeight( x + 1, z ) - this->getHeight( x, z ), 0.0f );
			}

			Vector3f out( 0.0f, 0.0f, 0.0f );
			if( z > 0 )
			{
				out = Vector3f( 0.0f, this->getHeight( x, z - 1 ) - this->getHeight( x, z ), -1.0f );
			}

			Vector3f in( 0, 0, 0 );
			if( z < mLength - 1 )
			{
				in = Vector3f( 0.0f, this->getHeight( x, z + 1 ) - this->getHeight( x, z ), 1.0f );
			}

			if( x > 0 && z > 0 )
			{
				sum += out.cross( left ).normalize();
			}
			if( x > 0 && z < mLength - 1 )
			{
				sum += left.cross( in ).normalize();
			}
			if( x < mWidth - 1 && z < mLength - 1 )
			{
				sum += in.cross( right ).normalize();
			}
			if( x< mWidth - 1 && z > 0 )
			{
				sum += right.cross( out ).normalize();
			}

			tempNormals[x * mWidth + z] = sum;
		}
	}

	// Average the normals to make the terrain look smoother
	mNormals = new Vector3f[mWidth * mLength];
	float dropOffRatio = 0.5f;		// How much of other normals we combine into this one
	for( int x = 0; x < mWidth; x++ )
	{
		for( int z = 0; z < mLength; z++ )
		{
			Vector3f sum = tempNormals[x * mWidth + z];

			if( x > 0 )
			{
				sum += tempNormals[( x - 1 ) * mWidth + z] * dropOffRatio;
			}
			if( x < mWidth - 1 )
			{
				sum += tempNormals[( x + 1 ) * mWidth + z] * dropOffRatio;
			}
			if( z > 0 )
			{
				sum += tempNormals[x * mWidth + ( z - 1 )] * dropOffRatio;
			}
			if( z < mLength - 1 )
			{
				sum += tempNormals[x * mWidth + ( z + 1 )] * dropOffRatio;
			}

			sum.normalize();
			mNormals[x * mWidth + z] = sum;
		}
	}
}
