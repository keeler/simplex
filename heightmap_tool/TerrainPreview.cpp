#include "TerrainPreview.hpp"
#include "../Texture.hpp"
#include "GL/glut.h"

TerrainPreview::TerrainPreview( int width, int height, const float heightScale ) :
	mWidth( width ),
	mLength( height ),
	mHeightScale( heightScale )
{
	mHeightMap = new float[mWidth * mLength];
}

TerrainPreview::~TerrainPreview()
{
	delete [] mHeightMap;
	delete [] mNormals;
}

void TerrainPreview::setHeightmap( unsigned char *pixels )
{
	mHighestValue = -0.5f * mHeightScale;
	for( int z = 0; z < mLength; z++ )
	{
		for( int x = 0; x < mWidth; x++ )
		{
			// Only need one channel, arbitrarily take red channel
			unsigned char color = pixels[3 * ( mWidth * x + z )];
			// Varies from - height / 2 to height / 2
			mHeightMap[mWidth * x + z] = mHeightScale * ( ( color / 255.0f ) - 0.5f );
			if( mHeightMap[mWidth * x + z] > mHighestValue )
			{
				mHighestValue = mHeightMap[mWidth * x + z];
			}
		}
	}

	computeNormals();
}

void TerrainPreview::render() const
{
	glColor3f( 0.3f, 0.9f, 0.0f );
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

void TerrainPreview::computeNormals()
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
