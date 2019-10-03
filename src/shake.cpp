#include "shake.h"
#include <randomnumbergenerator.h>
#include <math.h>

Shake::Shake(int pointsize, int xstart, int w, int ystart, int h, int framerate) : RenderEffectCore(pointsize, xstart, w, ystart, h, framerate)
{
	numSteps = 48;
	direction = RandomNumberGenerator::randomInt(0, 3);
	if (RandomNumberGenerator::randomInt(0, 1));
		shakes = RandomNumberGenerator::randomInt(2, 4) * 6;
}

void Shake::next()
{
	wait ++;
	ready = wait >= framerate;
	if (ready)
	{
		currentStep ++;
		if (currentStep > numSteps)
		{
			currentStep = 1;
			direction = RandomNumberGenerator::randomInt(0, 3);
			shakes = RandomNumberGenerator::randomInt(2, 4) * 6;
			currentStep += round((24 - shakes) * (RandomNumberGenerator::randomDouble(0, 2.5, 1)));
		}
		wait = 0;
	}
}
