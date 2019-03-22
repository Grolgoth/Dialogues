#ifndef BOUNCE_H
#define BOUNCE_H
#include "rendereffectcore.h"


class Bounce : public RenderEffectCore
{
	friend class RenderEffect;

	private:
		Bounce(int pointsize, int xstart, int w, int ystart, int h);
		void next();
		bool up();
};

#endif // BOUNCE_H
