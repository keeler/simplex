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

void saveBitmap( const std::string & filename, unsigned char *pixels, int width, int height )
{
	std::ofstream outfile;

	outfile.open( filename.c_str(), std::ios::out | std::ios::binary );
	fatalAssert( !outfile.fail(), "Could not open \'" + filename + "\'." );

	// Write magic number
	BmpFileMagic bmpFileMagic;
	bmpFileMagic.magicNumber[0] = 'B';
	bmpFileMagic.magicNumber[1] = 'M';
	outfile.write( bmpFileMagic.magicNumber, 2 );

	// Write file header
	BmpFileHeader bmpFileHeader;
	bmpFileHeader.fileSize = sizeof( BmpFileMagic ) + sizeof( BmpFileHeader ) + sizeof( BmpInfoHeader ) + width * height * 3;		// Size of bitmap file in bytes
	bmpFileHeader.reserved1 = 0;
	bmpFileHeader.reserved2 = 0;
	bmpFileHeader.dataOffset = sizeof( BmpFileMagic ) + sizeof( BmpFileHeader ) + sizeof( BmpInfoHeader );		// Starting address of byte where pixel data starts
	outfile.write( (char *)&bmpFileHeader, sizeof( BmpFileHeader ) );

	// Write info header
	BmpInfoHeader bmpInfoHeader;
	bmpInfoHeader.headerSize = 40;
	bmpInfoHeader.bitmapWidth = width;
	bmpInfoHeader.bitmapHeight = height;
	bmpInfoHeader.numColorPlanes = 1;
	bmpInfoHeader.bitsPerPixel = 24;
	bmpInfoHeader.compressionType = 0;
	bmpInfoHeader.rawImageDataSize = width * height * 3;
	bmpInfoHeader.horizontalResolution = 2835;    // 0x130B0000
	bmpInfoHeader.verticalResolution = 2835;
	bmpInfoHeader.numColorsInPalette = 0;
	bmpInfoHeader.numImportantColors = 0;
	outfile.write( (char *)&bmpInfoHeader, sizeof( BmpInfoHeader ) );

	// Write pixel data to file
	int rowSize = 4 * ceil( ( bmpInfoHeader.bitsPerPixel * width ) / 32.0f );    // In bytes
	int pixelArraySize = rowSize * abs( height );
	char *rawPixelData = new char[pixelArraySize];

	// Transform the RGB triples from OpenGL format to bitmap format
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			for( int c = 0; c < 3; c++ )
			{
				rawPixelData[rowSize * y + 3 * x + ( 2 - c )] = pixels[3 * ( width * y + x ) + c];
			}
		}
	}

	outfile.write( rawPixelData, pixelArraySize );

	outfile.close();
}
