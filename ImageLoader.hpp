#ifndef IMAGE_LOADER_HPP
#define IMAGE_LOADER_HPP

#include <string>
#include "stdint.h"

// The magic number has been removed from the header struct because it causes
// weird alignment problems.
struct BmpFileMagic
{
	char magicNumber[2];
};

struct BmpFileHeader
{
	int32_t		fileSize;		// Size of bitmap file in bytes
	uint16_t	reserved1;
	uint16_t	reserved2;
	uint32_t	dataOffset;		// Starting address of byte where pixel data starts
};

struct BmpInfoHeader
{
	uint32_t	headerSize;
	int32_t		bitmapWidth;			// Image width in pixels
	int32_t		bitmapHeight;			// Image height in pixels
	uint16_t	numColorPlanes;			// Must be set to 1
	uint16_t	bitsPerPixel;			// Color depth of image (typically 1, 4, 8, 16, 24, or 32)
	uint32_t	compressionType;		// Compression method being used for BMP
	uint32_t	rawImageDataSize;		// Image size, or size of the raw pixel data, not the same as file size
	int32_t		horizontalResolution;	// In pixels per meter
	int32_t		verticalResolution;		// In pixels per meter
	uint32_t	numColorsInPalette;		// Number of colors in the color palette
	uint32_t	numImportantColors;		// Number of important colors used, 0 when ever color is important (usually ignored)
};

////////////////////////////////////////////////////////////////////////////////
// Image Format Loader Functions
////////////////////////////////////////////////////////////////////////////////
void loadBitmap( const std::string & filename, unsigned char *& pixels, int & width, int & height );
void saveBitmap( const std::string & filename, unsigned char *pixels, int width, int height );

#endif
