#ifndef TEXTPRINTER_H
#define TEXTPRINTER_H
#include "rendereffect.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <File.h>
#include <fstring.h>
#include <vector.h>

class TextPrinter
{
	public:
		TextPrinter(File font, unsigned int font_px, bool highQuality = false);
		~TextPrinter();

		/**
		text parameter can include instructions like colour, bouncy/wavy text etc.
		see Dialogues readme.txt to see options and syntax.
		*/
		void startNewText(std::string text, unsigned int boxW);
		void setCurrentFPC(unsigned int fpc);
		SDL_Surface* getPrinted();

	private:
		struct Subject
		{
			std::string text = "";
			unsigned int boxW = 0;
			SDL_Surface* currentState = nullptr;
			unsigned int currentPos = 0;
			std::vector<Uint16> convertedText;
			unsigned int buildSteps = 0;
			std::vector<unsigned int> RenderEffectIndexes;
			std::vector<RenderEffect> renderEffects;
			SDL_Color currentColor;
			RenderEffect currentRenderEffect = RenderEffect(FString("NONE"));
			unsigned int currentFPC = 3;

		};
		File font;
		int font_px;
		bool fancy;
		Vector<Uint16> characters;

		Subject subject;

		void integrityCheck();
		void loadFaceFromFile();
		void checkText();
		void resetSubject();
		std::string extractMetaText(FString text, std::vector<unsigned int> metaTextStartIndexes, std::vector<unsigned int> metaTextCloseIndexes);
		std::string parse(std::string text);
};

#endif // TEXTPRINTER_H
