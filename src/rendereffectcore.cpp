#include "rendereffectcore.h"

RenderEffectCore::RenderEffectCore(int pointsize, int xstart, int w, int ystart, int h, int framerate) : framerate(framerate)
{
	numSteps = pointsize / 4;
	if (numSteps %2 != 0)
		numSteps ++;
	clip.x = xstart;
	clip.w = w;
	clip.y = ystart;
	clip.h = h;
	wait = framerate;
}

void RenderEffectCore::next()
{
	wait ++;
	ready = wait >= framerate;
	if (ready)
	{
		currentStep ++;
		if (currentStep > numSteps)
			currentStep = 1;
		wait = 0;
	}
}
