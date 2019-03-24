#include "spin.h"

Spin::Spin(int pointsize, int xstart, int w, int ystart, int h, int framerate, bool right, SDL_Surface* glyph) : RenderEffectCore(pointsize, xstart, w, ystart, h, framerate), right(right), glyph(glyph)
{
	currentStep = right ? 328 : 32;
	hasGlyph = glyph != nullptr;
	wait = framerate;
}

Spin::~Spin()
{
	if (glyph != nullptr)
		SDL_FreeSurface(glyph);
}

void Spin::next()
{
	wait ++;
	ready = wait >= framerate;
	if (ready)
	{
		if (right)
		{
			currentStep -=4;
			if (currentStep == 32)
				currentStep = 328;
		}
		else
		{
			currentStep +=4;
			if (currentStep == 328)
				currentStep = 32;
		}
		wait = 0;
	}
}
