#ifndef SHAKE_H
#define SHAKE_H
#include "rendereffectcore.h"

class Shake : public RenderEffectCore
{
	friend class RenderEffect;

	private:
		Shake(int pointsize, int xstart, int w, int ystart, int h, int framerate);
		~Shake(){}
		void next();
		inline bool isCoolingDown() {return shakes == 0;}
		// 0 = up, 1 = right, 2 = down, 3 = right
		int direction = 0;
		int shakes = 18;
		int pixelShifts = 1;
};

#endif // BOUNCE_H

