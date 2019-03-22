#include "bounce.h"

Bounce::Bounce(int pointsize, int xstart, int w, int ystart, int h) : RenderEffectCore(pointsize, xstart, w, ystart, h)
{

}

bool Bounce::up()
{
	return currentStep <= numSteps / 2;
}
