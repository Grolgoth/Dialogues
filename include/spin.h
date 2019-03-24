#ifndef SPIN_H
#define SPIN_H
#include "rendereffect.h"

class Spin : public RenderEffectCore
{
	friend class RenderEffect;

	private:
		Spin(int pointsize, int xstart, int w, int ystart, int h, bool right = false);
		~Spin(){}
		void next();
		bool right;
};

#endif // SPIN_H
