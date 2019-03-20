#include "dialoguesdl_functionality.h"
#include <SDL2/SDL.h>

DialogueSDL_Functionality::DialogueSDL_Functionality()
{
	//ctor
}

SDL_Surface* DialogueSDL_Functionality::transparentSurface(unsigned int w, unsigned int h)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
return SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#else
return SDL_CreateRGBSurface(0,w,h,32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
#endif
}

Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
{
    Uint32 *pixels = (Uint32*)surface->pixels;
    return pixels[(y * surface->w)+x];
}

void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
    Uint32 *pixels = (Uint32*)surface->pixels;
    pixels[(y * surface->w)+x] = pixel;
}

void DialogueSDL_Functionality::apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip)
{
    //Make a temporary rectangle to hold the offsets on the screen
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface( source, clip, destination, &offset );
}

void DialogueSDL_Functionality::setColor(SDL_Surface *surface, SDL_Color source, SDL_Color targetColor)
{
	SDL_Color* colors = surface->format->palette->colors;
	for (int i = 0; i < 256; i++)
		if (colors[i].r == source.r && colors[i].g == source.g && colors[i].b == source.b && colors[i].a > 127)
			colors[i] = targetColor;
	SDL_SetPaletteColors(surface->format->palette, colors, 0 , 256);
}

void DialogueSDL_Functionality::bounce(SDL_Surface* target, SDL_Rect clip, bool up)
{
	if (SDL_MUSTLOCK(target))
		SDL_LockSurface(target);
	int borderx = clip.x + clip.w;
	int bordery = clip.y + clip.h;
	Uint32 bench = get_pixel32(target, 0 , 0);
	if (up)
	{
		for(int x = clip.x; x < borderx; x++)
		{
			for(int y = clip.y + 1; y < bordery; y++)
			{
				Uint32 pixel = get_pixel32(target, x, y);
				put_pixel32(target, x, y - 1, pixel);
			}
		}
		for(int x = clip.x; x < borderx; x++)
		{
			put_pixel32(target, x, bordery - 1, bench);
		}
	}
	else
	{
		for(int x = clip.x; x < borderx; x++)
		{
			for(int y = bordery - 2; y >= clip.y; y--)
			{
				Uint32 pixel = get_pixel32(target, x, y);
				put_pixel32(target, x, y + 1, pixel);
			}
		}
		for(int x = clip.x; x < borderx; x++)
		{
			put_pixel32(target, x, clip.y, bench);
		}
	}
	SDL_UnlockSurface(target);
}
