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
		inline std::vector<Word> getWords() {return words;}
		std::vector<int> selectedWords;
		inline int getCurrentSurfaceNumber() {return currentSurfaceNumber;}

		void startNewText(std::string text, unsigned int boxW, unsigned int boxH, unsigned int effectSpeed, int number);
		void clearWordsInIndicedSurface(int surface);

	private:
		void printNext();
		void newWord(bool wasNewline = false, bool removeSpace = false, int spaceW = 0);
		int currentWordIndex = -1;
		int currentSurfaceNumber = 0;

		std::vector<Word> words;
};

#endif // WORDPRINTER_H
