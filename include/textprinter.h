#ifndef TEXTPRINTER_H
#define TEXTPRINTER_H
#include "rendereffect.h"
#include <SDL.h>
#include <File.h>
#include <fstring.h>
#include <vector.h>
#include <map>

class Mix_Chunk;

class TextPrinter
{
	public:
		TextPrinter(File font, unsigned int font_px, Mix_Chunk* sound = nullptr);
		~TextPrinter();

		/**
		text parameter can include instructions like colour, bouncy/wavy text etc.
		see Dialogues readme.txt to see options and syntax.
		*/
		void startNewText(std::string text, unsigned int boxW, unsigned int boxH, unsigned int effectSpeed);
		void appendText(std::string text);
		void setCurrentFPC(unsigned int fpc);
		void setExternalEffectInt(int* external);
		void finish();
		void removeLastChar();
		void cleanEntered(); // this function seems bad and unnecessary
		SDL_Surface* getPrinted();
		SDL_Surface* printCharacters(unsigned int amount);
		SDL_Surface* getPrintedPure(std::string entered = ""); // this function seems bad and unnecessary
		inline unsigned int getTextLength() {return subject.convertedText.size();}
		inline unsigned int getPrintedTextW() {return subject.xposOnSurface;}
		inline bool finished() {return subject.currentPos == subject.convertedText.size();}
		inline unsigned int getNumCharsPrinted() {return subject.currentPos;}
		inline int getNumCharsConsideringFrameRate() {return subject.numCharsConsideringFrameRate;}
		inline int getCurrentFPC() {return subject.currentFPC;}

	private:
		TextPrinter(const TextPrinter& other);

		struct Subject
		{
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
			std::vector<int> lineOffsetsToBoxW; //list of how many pixels were left between the last character of a line and the box max width
			SDL_Color currentColor;
			int indexCurrentRenderEffect = -1;
			unsigned int currentFPC = 1;
			unsigned int numCharsConsideringFrameRate = 0;
			int speed = 1;
		};
		File font;
		inline bool inRenderEffectIndexes(int index){return Vector<unsigned int>::fromStdVector(subject.RenderEffectIndexes).contains(index);}
		int font_px;
		Mix_Chunk* sound = nullptr;
		bool mute = false;
		int spaceCharExtraW = 0;
		int text_h = 0;
		int textOffset = 0;
		int* externalEffectInt = nullptr; //Can be used to change different things such as npc portrait index or whatever else you want
		Vector<Uint16> characters;
		std::vector<unsigned int> characterWidths;
		Vector<SDL_Surface*> glyphs;
		std::map<SDL_Surface*, SDL_Color> changedColors;

		Subject subject;

		bool unescapedBackslashCheck(FString text);
		void integrityCheck();
		void loadFaceFromFile();
		void printNext();
		void printCharacter(SDL_Surface* glyph, int characterIndex);
		void handleNewLine();
		void checkText(unsigned char* text, unsigned int bytes);
		void resetSubject();
		void updateRenderSettings();
		void processRenderEffect(RenderEffect effect, int index);
		void updateActiveRenderEffects();
		void setColor(SDL_Surface* target, SDL_Color color);
		void correctRenderEffectIndexes(std::vector<std::string> metaTexts);
		void extractMetaText(FString text, std::vector<unsigned int> metaTextStartIndexes, std::vector<unsigned int> metaTextCloseIndexes);
		void shiftRenderIndexes(int requisite, int amount);
		void parse();
		void calcNumCharsConsideringFrameRate();
		std::string subjectTextToSimpleString();
		std::vector<unsigned int> setMetaTextIndexes(FString text, bool start);
};

#endif // TEXTPRINTER_H
