#include "wheelspin.h"
#include <custom_sdl_functions.h>

WheelSpin::WheelSpin(int pointsize, int xstart, int w, int ystart, int h, int framerate, bool right, SDL_Surface* glyph) : RenderEffectCore(pointsize, xstart, w, ystart, h, framerate), right(right), glyph(glyph)
{
    tileSets = getTileSetsOfSurface(w, h, &numSteps);
    hasGlyph = glyph != nullptr;
}

WheelSpin::~WheelSpin()
{
    if (glyph != nullptr)
		SDL_FreeSurface(glyph);
}
