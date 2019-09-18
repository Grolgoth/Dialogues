#ifndef RENDEREFFECTCORE_H_INCLUDED
#define RENDEREFFECTCORE_H_INCLUDED
#include <SDL.h>

class RenderEffectCore
{
	friend class RenderEffect;

	protected:
		RenderEffectCore(int pointsize, int xstart, int w, int ystart, int h, int framerate);
		virtual ~RenderEffectCore(){}
		int numSteps = 0;
		int currentStep = 0;
		int framerate = 15;
		int wait = 0;
		SDL_Rect clip;
		bool ready;
		int copies = 0;
		virtual void next();
		inline void setFramerate(int framerate) {this->framerate = framerate;}
};

#endif // RENDEREFFECTCORE_H_INCLUDED
