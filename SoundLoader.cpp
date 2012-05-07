#include "SoundLoader.hpp"
#include "Debug.hpp"
#include <fstream>
#include <cstring>

void loadWav( const std::string & filename, unsigned char *& sampleData, int & dataSize, int & numChannels, int & bitsPerSample, int & sampleRate )
{
	std::ifstream infile;

	infile.open( filename.c_str(), std::ios::in | std::ios::binary );
	fatalAssert( !infile.fail(), "Could not find \'" + filename + "\'." );

	// Read header and ensure that this is actually a .wav file
	WavHeader wavHeader;
	infile.read( (char *)&wavHeader, sizeof( WavHeader ) );
	fatalAssert( strncmp( (char *)wavHeader.riffLetters, "RIFF", 4 ) == 0, "Not a WAVE file." );
	fatalAssert( strncmp( (char *)wavHeader.waveLetters, "WAVE", 4 ) == 0, "Not a WAVE file." );

	// Read the format subchunk, error check it, and extract the relevant data
	WavFormatSubchunk wavFormatSubchunk;
	infile.read( (char *)&wavFormatSubchunk, sizeof( WavFormatSubchunk ) );
	fatalAssert( strncmp( (char *)wavFormatSubchunk.fmtLetters, "fmt ", 4 ) == 0, "Invalid WAVE file." );

	numChannels = wavFormatSubchunk.numChannels;
	bitsPerSample = wavFormatSubchunk.bitsPerSample;
	sampleRate = wavFormatSubchunk.sampleRate;

	// Read the data subchunk, error check it, extract the data size, and then read the sample data
	WavDataSubchunk wavDataSubchunk;
	infile.read( (char *)&wavDataSubchunk, sizeof( WavDataSubchunk ) );
	fatalAssert( strncmp( (char *)wavDataSubchunk.dataLetters, "data", 4 ) == 0, "Invalid WAVE file." );

	dataSize = wavDataSubchunk.dataSize;
	sampleData = new unsigned char[dataSize];
	infile.read( (char *)sampleData, dataSize );

	infile.close();
}
