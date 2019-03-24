#ifndef SPIN_H
#define SPIN_H
#include "rendereffect.h"

class Spin : public RenderEffectCore
{
	friend class RenderEffect;

	private:
		Spin(int pointsize, int xstart, int w, int ystart, int h, bool right = false, SDL_Surface* glyph = nullptr);
		~Spin();
		void next();
		bool right;
		bool hasGlyph;
		SDL_Surface* glyph = nullptr;
};

#endif // SPIN_H
