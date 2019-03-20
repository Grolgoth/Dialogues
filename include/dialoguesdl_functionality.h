#ifndef DIALOGUESDL_FUNCTIONALITY_H
#define DIALOGUESDL_FUNCTIONALITY_H

struct SDL_Surface;
struct SDL_Rect;
struct SDL_Color;

class DialogueSDL_Functionality
{
	public:
		DialogueSDL_Functionality();

	protected:

	private:
		friend class TextPrinter;
		friend class RenderEffect;
		static SDL_Surface* transparentSurface(unsigned int w, unsigned int h);
		static void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip);
		static void setColor(SDL_Surface* surface, SDL_Color sourceColor, SDL_Color targetColor);
		static void bounce(SDL_Surface* target, SDL_Rect clip, bool up);
};

#endif // DIALOGUESDL_FUNCTIONALITY_H
