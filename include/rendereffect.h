#ifndef RENDEREFFECT_H
#define RENDEREFFECT_H
#include <SDL2/SDL.h>
#include <vector>
#include "rendereffectcore.h"

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
		enum State
		{
			INACTIVE,
			OPEN,
			SET
		};
		RenderEffect(FString metaText, SDL_Surface* target, int pointsize);
		RenderEffect(const RenderEffect& other);
		~RenderEffect();
		inline bool isActive() {return state != INACTIVE;}
		inline Type getType() {return type;}
		inline unsigned int getFpcValue() {return fpcValue;}
		inline SDL_Color getColor() {return color;}
		void apply();
		void expandArea(int expansion);

	private:
		friend class TextPrinter;
		Type type = NONE;
		State state = INACTIVE;
		SDL_Color color;
		unsigned int fpcValue = 3;
		int pointsize;

		SDL_Surface* target;
		SDL_Rect area;

		std::vector<RenderEffectCore*> effects;

		bool isValidMetaText(FString metaText);
		void applyBounceEffect();
		void applySpinEffect();
};

#endif // RENDEREFFECT_H
