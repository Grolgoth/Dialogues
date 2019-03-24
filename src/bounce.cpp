#include "bounce.h"

Bounce::Bounce(int pointsize, int xstart, int w, int ystart, int h, int framerate) : RenderEffectCore(pointsize, xstart, w, ystart, h, framerate)
{

}

bool Bounce::up()
{
	return currentStep <= numSteps / 2;
}
