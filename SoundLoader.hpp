#ifndef _SOUND_LOADER_HPP_
#define _SOUND_LOADER_HPP_

#include <string>
#include "stdint.h"

struct WavHeader
{
	uint8_t		riffLetters[4];		// Should contain "RIFF" in ASCII
	uint32_t	chunkSize;
	uint8_t		waveLetters[4];		// Should contain "WAVE" in ASCII
}__attribute__((packed));

struct WavFormatSubchunk
{
	uint8_t		fmtLetters[4];		// Should contain "fmt "
	uint32_t	formatChunkSize;		// 16 for PCM. Size of the rest of the format subchunk
	uint16_t	audioFormat;		// We only care abput PCM = 1, which means no compression
	uint16_t	numChannels;		// Mono = 1, Stereo = 2, etc.
	uint32_t	sampleRate;			// Probably 44100 Hz
	uint32_t	byteRate;			// == sampleRate * numChannels * bitsPerSample / 8
	uint16_t	blockAlign;			// Number of bytes for one sample including all channels. == numChannels * bitsPerSample / 8
	uint16_t	bitsPerSample;		// Number of bits in each sample
}__attribute__((packed));

struct WavDataSubchunk
{
	uint8_t		dataLetters[4];		// Should contain "data"
	uint32_t	dataSize;			// Number of bytes in the sample data, which will follow immediately after this word in the file
}__attribute__((packed));

void loadWav( const std::string & filename, unsigned char *& sampleData, int & dataSize, int & numChannels, int & bitsPerSample, int & sampleRate );

#endif
