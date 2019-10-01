#ifndef TEXTPRINTER_H
#define TEXTPRINTER_H
#include "rendereffect.h"
#include <SDL.h>
#include <File.h>
#include <fstring.h>
#include <vector.h>
#include <map>

class TextPrinter
{
	public:
		TextPrinter(File font, unsigned int font_px);
		~TextPrinter();

		/**
		text parameter can include instructions like colour, bouncy/wavy text etc.
		see Dialogues readme.txt to see options and syntax.
		*/
		void startNewText(std::string text, unsigned int boxW, unsigned int boxH, unsigned int effectSpeed);
		void setCurrentFPC(unsigned int fpc);
		SDL_Surface* getPrinted();
		inline bool finished() {return subject.currentPos == subject.convertedText.size();}

	private:
		TextPrinter(const TextPrinter& other);

		struct Subject
		{
			std::string text = "";
			unsigned int boxW = 0;
			unsigned int boxH = 0;
			SDL_Surface* currentState = nullptr;
			unsigned int xposOnSurface = 0;
			unsigned int yposOnSurface = 0;
			unsigned int currentPos = 0;
			std::vector<Uint16> convertedText;
			unsigned int framesSinceLastPrint = 0;
			std::vector<unsigned int> RenderEffectIndexes;
			std::vector<RenderEffect> renderEffects;
			SDL_Color currentColor;
			int indexCurrentRenderEffect = -1;
			unsigned int currentFPC = 5;
			int speed = 1;
		};
		File font;
		inline bool inRenderEffectIndexes(int index){return Vector<unsigned int>::fromStdVector(subject.RenderEffectIndexes).contains(index);}
		int font_px;
		int text_w = 0;
		int text_h = 0;
		int textOffset = 0;
		Vector<Uint16> characters;
		std::vector<unsigned int> characterWidths;
		Vector<SDL_Surface*> glyphs;
		std::map<SDL_Surface*, SDL_Color> changedColors;

		Subject subject;

		void integrityCheck();
		void loadFaceFromFile();
		void printNext();
		void handleNewLine();
		void checkText();
		void resetSubject();
		void updateRenderSettings();
		void processRenderEffect(RenderEffect effect, int index);
		void updateActiveRenderEffects();
		void setColor(SDL_Surface* target, SDL_Color color);
		void correctRenderEffectIndexes(std::vector<std::string> metaTexts);
		std::string extractMetaText(FString text, std::vector<unsigned int> metaTextStartIndexes, std::vector<unsigned int> metaTextCloseIndexes);
		std::string parse(std::string text);
};

#endif // TEXTPRINTER_H
