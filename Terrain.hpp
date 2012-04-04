#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "Vector3f.hpp"
#include <string>

class Terrain
{
	public:
		Terrain( const std::string & filename, float heightScale );    // Heights of terrain will range from -heightScale / 2 to heightScale / 2
		~Terrain();

		void render() const;
		float getHeight( int x, int z ) const { return mHeightMap[x * mWidth + z]; };
		Vector3f getNormal( int x, int z ) const { return mNormals[x * mWidth + z]; };

	private:
		void computeNormals();

		float		*mHeightMap;
		int			mWidth;
		int			mLength;

		Vector3f	*mNormals;
};

#endif
