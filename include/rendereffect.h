#ifndef RENDEREFFECT_H
#define RENDEREFFECT_H
#include <SDL.h>
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
			BOUNCE,
			SHAKE,
			SPIN,
			SPIN2,
			COLOR,
			SILENT,
			FPC,
			EXTERNAL
		};
		enum State
		{
			INACTIVE,
			OPEN,
			SET
		};
		RenderEffect(FString metaText, SDL_Surface* target, int pointsize, int speed);
		RenderEffect(Type type, SDL_Surface* target, int pointsize, int speed);
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
		//Translates to the framerate at which we do stuff 1 is default and normal. Higher in this case actually means less delay frames so faster speed.
		int speed = 1;

		SDL_Surface* target;
		SDL_Rect area;

		std::vector<RenderEffectCore*> effects;

		bool isValidMetaText(FString metaText);
		void applyBounceEffect();
		void applySpinEffect();
		void applySafeSpinEffect();
		void applyShakeEffect();
};

#endif // RENDEREFFECT_H
