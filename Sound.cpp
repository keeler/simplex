#include "Sound.hpp"
#include "SoundLoader.hpp"
#include "Debug.hpp"
#include <cstdio>
#include <cstring>
#include <AL/alut.h>

ALenum determineAlFormat( int numChannels, int bitsPerSample );

Sound::Sound( const std::string & filename )
{
	mDevice = alcOpenDevice( NULL );
	mContext = alcCreateContext( mDevice, NULL );
	alcMakeContextCurrent( mContext );

	alListener3f( AL_POSITION, 0.0f, 0.0f, 0.0f );
	alListener3f( AL_VELOCITY, 0.0f, 0.0f, 0.0f );
	alListener3f( AL_ORIENTATION, 0.0f, 0.0f, -1.0f );

	alGenSources( 1, &mSourceId );

	alSourcef( mSourceId, AL_PITCH, 1.0f );
	alSourcef( mSourceId, AL_GAIN, 1.0f );
	alSource3f( mSourceId, AL_POSITION, 0.0f, 0.0f, 0.0f );
	alSource3f( mSourceId, AL_VELOCITY, 0.0f, 0.0f, 0.0f );
	alSourcei( mSourceId, AL_LOOPING, AL_TRUE );

	alGenBuffers( 1, &mBufferId );

	unsigned char *sampleData;
	int dataSize;
	int numChannels;
	int bitsPerSample;
	int sampleRate;

	// Load WAV file from disk
	loadWav( filename, sampleData, dataSize, numChannels, bitsPerSample, sampleRate );
	ALenum format = determineAlFormat( numChannels, bitsPerSample );
	alBufferData( mBufferId, format, sampleData, dataSize, sampleRate );

	delete [] sampleData;

	alSourcei( mSourceId, AL_BUFFER, mBufferId );
}

Sound::~Sound()
{
	alDeleteSources( 1, &mSourceId );
	alDeleteBuffers( 1, &mBufferId );
	alcDestroyContext( mContext );
	alcCloseDevice( mDevice );
}

void Sound::setPosition( const Vector3f & position )
{
	alSource3f( mSourceId, AL_POSITION, position[0], position[1], position[2] );
}

void Sound::setVelocity( const Vector3f & velocity )
{
	alSource3f( mSourceId, AL_VELOCITY, velocity[0], velocity[1], velocity[2] );
}

void Sound::play()
{
	alSourcePlay( mSourceId );
}

void Sound::pause()
{
	alSourcePause( mSourceId );
}

ALenum determineAlFormat( int numChannels, int bitsPerSample )
{
	if( numChannels == 2 )
	{
		if( bitsPerSample == 16 )
		{
			return AL_FORMAT_STEREO16;
		}
		else if( bitsPerSample == 8 );
		{
			return AL_FORMAT_STEREO8;
		}
	}
	else if( numChannels == 1 )
	{
		if( bitsPerSample == 16 )
		{
			return AL_FORMAT_MONO16;
		}
		else if( bitsPerSample == 8 );
		{
			return AL_FORMAT_MONO8;
		}
	}
	else
	{
		alwaysAssert( "Unsupported number of channels or bits per sample in sound file." );
	}
}
