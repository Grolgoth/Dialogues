#include "spin.h"

Spin::Spin(int pointsize, int xstart, int w, int ystart, int h, bool right) : RenderEffectCore(pointsize, xstart, w, ystart, h), right(right)
{
	currentStep = right ? 360 : 0;
	framerate = 3;
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
