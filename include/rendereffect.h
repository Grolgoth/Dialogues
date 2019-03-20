#ifndef RENDEREFFECT_H
#define RENDEREFFECT_H
#include <SDL2/SDL.h>

class FString;

class RenderEffect
{
	public:
		struct UnsupportedTypeError
		{

		};
		enum Type
		{
			NONE,
			WAVE,
			BOUNCE,
			SHAKE,
			SPIN,
			COLOR,
			FPC
		};
		RenderEffect(FString metaText);

	private:
		SDL_Color color;
		unsigned int fpcValue = 3;
		Type type = NONE;

		bool isValidMetaText(FString metaText);
};

#endif // RENDEREFFECT_H
