#include "ImageLoader.hpp"
#include "Debug.hpp"
#include <fstream>
#include <cmath>

void loadBitmap( const std::string & filename, unsigned char *& pixels, int & width, int & height )
{
	std::ifstream infile;

	infile.open( filename.c_str(), std::ios::in | std::ios::binary );
	fatalAssert( !infile.fail(), "Could not find \'" + filename + "\'." );

	// Verify magic number
	BmpFileMagic bmpFileMagic;
	infile.read( bmpFileMagic.magicNumber, 2 );
	fatalAssert( bmpFileMagic.magicNumber[0] == 'B' && bmpFileMagic.magicNumber[1] == 'M', "\'" + filename + "\' is not a bitmap file." );

	// Read file header
	BmpFileHeader bmpFileHeader;
	infile.read( (char *)&bmpFileHeader, sizeof( bmpFileHeader ) );

	// Read info header
	BmpInfoHeader bmpInfoHeader;
	infile.read( (char *)&bmpInfoHeader, sizeof( BmpInfoHeader ) );

	// Make sure we have a format appropriate for OpenGL
	fatalAssert( bmpInfoHeader.headerSize == 40, "Cannot load \'" + filename + "\', it is not a V3 format bitmap." );
	fatalAssert( bmpInfoHeader.bitsPerPixel == 24, "Cannot load \'" + filename + "\', it is not 24 bits per pixel." );
	fatalAssert( bmpInfoHeader.compressionType == 0, "Cannot load \'" + filename + "\', it is compressed." );

	// Width and height of image in pixels
	width = bmpInfoHeader.bitmapWidth;
	height = bmpInfoHeader.bitmapHeight;

	// Get the pixel data from the file
	int rowSize = 4 * ceil( ( bmpInfoHeader.bitsPerPixel * width ) / 32.0f );    // In bytes
	int pixelArraySize = rowSize * abs( height );
	char *rawPixelData = new char[pixelArraySize];
	infile.seekg( bmpFileHeader.dataOffset, std::ios_base::beg );    // Seek to where the raw image data is stored
	infile.read( rawPixelData, pixelArraySize );

	// Allocate the array that OpenGL expects, and transform the raw data into the format OpenGL wants
	pixels = new unsigned char[width * height * 3];    // Array of pixels, each with three values, R, G, and B
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			for( int c = 0; c < 3; c++ )
			{
				pixels[3 * ( width * y + x ) + c] = rawPixelData[rowSize * y + 3 * x + ( 2 - c )];
			}
		}
	}

	infile.close();
}
