#include "spin.h"

Spin::Spin(int pointsize, int xstart, int w, int ystart, int h, bool right, SDL_Surface* glyph) : RenderEffectCore(pointsize, xstart, w, ystart, h), right(right), glyph(glyph)
{
	currentStep = right ? 360 : 0;
	framerate = 3;
	hasGlyph = glyph != nullptr;
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
			currentStep -=2;
			if (currentStep == 0)
				currentStep = 360;
		}
		else
		{
			currentStep +=2;
			if (currentStep == 360)
				currentStep = 0;
		}
		wait = 0;
	}
}
