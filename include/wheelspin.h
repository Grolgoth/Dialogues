#ifndef WHEELSPIN_H
#define WHEELSPIN_H
#include "rendereffect.h"

class WheelSpin : public RenderEffectCore
{
	friend class RenderEffect;

	private:
		WheelSpin(int pointsize, int xstart, int w, int ystart, int h, int framerate, bool right = false, SDL_Surface* glyph = nullptr);
		~WheelSpin();
		bool right;
		bool hasGlyph;
		int previousSpins = 0;
        std::vector<std::vector<int>> tileSets;
        SDL_Surface* glyph = nullptr;

        inline int* getPreviousSpins() {return &previousSpins;}
};

#endif // WHEELSPIN_H
