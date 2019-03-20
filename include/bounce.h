#ifndef BOUNCE_H
#define BOUNCE_H


class Bounce
{
	friend class RenderEffect;

private:
	Bounce(int pointsize, int xstart, int xend, int ystart, int yend)
	{
		numSteps = pointsize / 4;
		if (numSteps %2 != 0)
			numSteps ++;
		clip.x = xstart;
		clip.w = xend;
		clip.y = ystart;
		clip.h = yend;
		wait = framerate;
	}
	int numSteps = 0;
	int currentStep = 0;
	int framerate = 15;
	int wait = 0;
	SDL_Rect clip;
	bool ready;
	void next()
	{
		wait ++;
		ready = wait >= framerate;
		if (ready)
		{
			currentStep ++;
			if (currentStep > numSteps)
				currentStep = 1;
			wait = 0;
		}
	}
	bool up() {return currentStep <= numSteps / 2;}
};

#endif // BOUNCE_H
