#include "Terrain.hpp"
#include "ImageLoader.hpp"
#include "GL/glut.h"

Terrain::Terrain( const std::string & filename, float heightScale )
{
	char *pixels = NULL;

	loadBitmap( filename, pixels, mWidth, mLength );

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
	if( mHeightMap != NULL )
	{
		delete [] mHeightMap;
	}
	if( mNormals != NULL )
	{
		delete [] mNormals;
	}
}

void Terrain::render() const
{
	glColor3f( 0.3f, 0.5f, 0.7f );
	for( int z = 0; z < mLength - 1; z++ )
	{
		glBegin( GL_TRIANGLE_STRIP );
		for( int x = 0; x < mWidth; x++ )
		{
			Vector3f normal = mNormals[x * mWidth + z];
			glNormal3f( normal[0], normal[1], normal[2] );
			glVertex3f( x, mHeightMap[mWidth * x + z], z );

			normal = mNormals[x * mWidth + ( z + 1 )];
			glNormal3f( normal[0], normal[1], normal[2] );
			glVertex3f( x, mHeightMap[mWidth * x + ( z + 1 )], z + 1 );
		}
		glEnd();
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
				sum += tempNormals[x * mWidth + ( z + 1)] * dropOffRatio;
			}

			sum.normalize();
			mNormals[x * mWidth + z] = sum;
		}
	}
}
