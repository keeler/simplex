#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "../Math.hpp"
#include <string>

class TerrainPreview
{
	public:
		TerrainPreview( int width, int height, const float heightScale );    // Heights of terrain will range from -heightScale / 2 to heightScale / 2
		~TerrainPreview();

		void setHeightmap( unsigned char *pixels );
		void render() const;
		float getHeight( int x, int z ) const { return mHeightMap[x * mWidth + z]; };
		Vector3f getNormal( int x, int z ) const { return mNormals[x * mWidth + z]; };
		float getHighestValue() const { return mHighestValue; };

	private:
		void computeNormals();

		float		*mHeightMap;
		Vector3f	*mNormals;
		int			mWidth;
		int			mLength;
		float		mHeightScale;
		float		mHighestValue;
};

#endif
