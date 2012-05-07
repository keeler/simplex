#ifndef _SOUND_HPP_
#define _SOUND_HPP_

#include "Math.hpp"
#include <string>
#include <AL/al.h>
#include <AL/alc.h>

class Sound
{
	public:
		Sound( const std::string & filename );
		~Sound();

		void setPosition( const Vector3f & position );
		void setVelocity( const Vector3f & velocity );

		void play();
		void pause();

	private:
		ALuint		mBufferId;
		ALuint		mSourceId;
		ALCdevice 	*mDevice;
		ALCcontext 	*mContext;
};

#endif
