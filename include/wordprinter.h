#ifndef WORDPRINTER_H
#define WORDPRINTER_H
#include "textprinter.h"

class WordPrinter : public TextPrinter
{
	public:
		struct Word {
			std::string id;
			int surfaceNumber;
			short x1, y1, x2, y2;
		};

		WordPrinter(File font, unsigned int font_px, Mix_Chunk* sound = nullptr);
		std::vector<int> selectedWords;
		inline int getCurrentSurfaceNumber() {return currentSurfaceNumber;}
		std::vector<Word> getWords(int surfaceNumber);
		int getTextWidth(int surfaceNumber);

		void startNewText(std::string text, unsigned int boxW, unsigned int boxH, unsigned int effectSpeed, int number);
		void trimWords(int surface);
		void clearWordsInIndicedSurface(int surface);

	private:
		void printNext();
		void newWord(bool wasNewline = false, bool removeSpace = false, int spaceW = 0, bool newSet = false);
		int currentWordIndex = -1;
		int currentSurfaceNumber = 0;

		std::map<int, std::vector<Word>> words;
		std::map<int, int> textWidths;
};

#endif // WORDPRINTER_H
