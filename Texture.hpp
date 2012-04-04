#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

class Texture
{
	public:
		Texture( const std::string filename );
		~Texture();

		void loadBmp( const std::string & filename );
		void bind() const;

		int getTextureId() const { return mTextureId; };
		std::string getFilename() const { return mFilename; };

	private:
		std::string		mFilename;
		unsigned int	mTextureId;
};

#endif
