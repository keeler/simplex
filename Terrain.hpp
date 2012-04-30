/*
* Modified from http://www.videotutorialsrock.com
*/

#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "Vector3f.hpp"
#include "Texture.hpp"
#include <string>

class Terrain
{
	public:
		Terrain( const std::string & heightmapFilename, const std::string & textureFilename, const float heightScale );    // Heights of terrain will range from -heightScale / 2 to heightScale / 2
		~Terrain();

		void render() const;
		float getHeight( int x, int z ) const { return mHeightMap[x * mWidth + z]; };
		Vector3f getNormal( int x, int z ) const { return mNormals[x * mWidth + z]; };

	private:
		void computeNormals();

		float		*mHeightMap;
		Vector3f	*mNormals;
		int			mWidth;
		int			mLength;

		Texture		*mTexture;
};

#endif
