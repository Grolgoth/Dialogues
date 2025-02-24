#include "wordprinter.h"
#include <cctype>
#include <custom_sdl_functions.h>
#include <sdl_ttf_custom.h>
#include <ft2build.h>
#include <algorithm>
#include FT_FREETYPE_H

WordPrinter::WordPrinter(File font, unsigned int font_px, Mix_Chunk* sound) : TextPrinter(font, font_px, sound)
{
	//ctor
}

std::vector<WordPrinter::Word> WordPrinter::getWords(int surfaceNumber)
{
	if (words.find(surfaceNumber) == words.end())
	{
		std::cout << "ERROR in WordPrinter::getWords: wordset for surfaceNumber " << surfaceNumber << " doesn't exist!" << std::endl;
		return {};
	}
	return words[surfaceNumber];
}

void WordPrinter::startNewText(std::string text, unsigned int boxW, unsigned int boxH, unsigned int effectSpeed, int number)
{
	resetSubject();
	subject.boxW = boxW;
	subject.boxH = boxH;
	subject.currentState = createTransparentSurface(boxW, boxH);
	subject.speed = effectSpeed >= 3 ? 3 : effectSpeed;
	mute = false;

	selectedWords.clear();
	currentSurfaceNumber = number;
	currentWordIndex = -1;
	words[currentSurfaceNumber] = {};
	newWord(false, false, 0, true);

	unsigned char* textp = U8StringToCharArray(text);
	checkText(textp, text.length());
	delete[] textp;
	calcNumCharsConsideringFrameRate();
}

void WordPrinter::newWord(bool wasNewline, bool removeSpace, int spaceW, bool newSet)
{
	if (words[currentSurfaceNumber].size() > 0 && !newSet)
	{
		words[currentSurfaceNumber][currentWordIndex].y2 = words[currentSurfaceNumber][currentWordIndex].y1 + text_h;
		if (!wasNewline)
			words[currentSurfaceNumber][currentWordIndex].x2 = subject.xposOnSurface;
	}

	currentWordIndex++;
	Word word;
	word.x1 = subject.xposOnSurface;
	if (removeSpace)
		word.x1 += spaceCharExtraW + spaceW;
	word.y1 = subject.yposOnSurface;
	word.surfaceNumber = currentSurfaceNumber;
	words[currentSurfaceNumber].push_back(word);
}

std::string convertUint16ToUtf8(Uint16 value)
{
    std::string utf8;

    if (value < 0x80)
	{
        utf8 += value;
    }
    else if (value < 0x800)
	{
        utf8 += (value >> 6) | 0xC0;
        utf8 += (value & 0x3F) | 0x80;
    }
    else
	{
        utf8 += (value >> 12) | 0xE0;
        utf8 += ((value >> 6) & 0x3F) | 0x80;
        utf8 += (value & 0x3F) | 0x80;
    }
    if (value >= 0xD800 && value <= 0xDFFF)
	{
		std::cout << "ERROR! 4 byte utf-8 char don't know what to do with it! (in: wordPrinter::printNext())";
		return "";
	}
    std::transform(utf8.begin(), utf8.end(), utf8.begin(), ::tolower); //This does not work on multibyte chars in UTF8 string. It's possible to do, but I don't care for now.
    return utf8;
}

void WordPrinter::printNext()
{
	Uint16 character = subject.convertedText[subject.currentPos];
	if (character == 10 && currentWordIndex >= 0 && words[currentSurfaceNumber].size() > 0)
	{
		words[currentSurfaceNumber][currentWordIndex].x2 = subject.xposOnSurface;
		handleNewLine();
		newWord(true);
		return;
	}
	else if (character == 10)
	{
		handleNewLine();
		return;
	}
	int characterIndex = characters.getIndex(character);
	if (characterIndex == -1)
		throw "Fatal Error: character could not be found even after the check if all characters were present passed successfully. (For character with UTF8 decimal value " + FString::fromInt(character).toStdString() + ").";
	if(subject.xposOnSurface != (unsigned)textOffset && subject.xposOnSurface + characterWidths[characterIndex] >= subject.boxW)
	{
		int lettersToBorder = 0;
		bool fit = true;
		while (subject.currentPos > 0 && subject.convertedText[subject.currentPos] != 32)
		{
			subject.currentPos--;
			lettersToBorder++;
			if (subject.convertedText[subject.currentPos] == 10 || subject.currentPos == 1)
			{
				fit = false;
				break;
			}
		}
		subject.currentPos += lettersToBorder;
		if (fit)
		{
			for (int i = 0; i < lettersToBorder - 1; i++)
				removeChar(subject.currentPos - 1);
		}
		subject.lineOffsetsToBoxW.push_back(subject.boxW - subject.xposOnSurface);
		subject.xposOnSurface = textOffset;
		subject.yposOnSurface += text_h;
		subject.lines++;
		if (currentWordIndex >= 0 && words[currentSurfaceNumber].size() > 0)
		{
			if (fit)
				words[currentSurfaceNumber][currentWordIndex].id = "";
			words[currentSurfaceNumber][currentWordIndex].y1 += text_h;
			words[currentSurfaceNumber][currentWordIndex].x1 = textOffset;
		}
		if (subject.indexCurrentRenderEffect >= 0 && subject.indexCurrentRenderEffect < subject.renderEffects.size())
		{
			if (subject.renderEffects.size() > 0 && subject.renderEffects[subject.indexCurrentRenderEffect].state == RenderEffect::OPEN && !inRenderEffectIndexes(subject.currentPos))
			{
				RenderEffect::Type previousType = subject.renderEffects[subject.indexCurrentRenderEffect].type;
				subject.RenderEffectIndexes.insert(subject.RenderEffectIndexes.begin() + subject.indexCurrentRenderEffect, subject.currentPos);
				subject.renderEffects.insert(subject.renderEffects.begin() + subject.indexCurrentRenderEffect, RenderEffect(previousType, subject.currentState, font_px, subject.speed));
				subject.indexCurrentRenderEffect++;
				updateRenderSettings();
			}
		}
		while (fit && lettersToBorder > 1)
		{
			printNext();
			lettersToBorder--;
		}
	}
	SDL_Surface* glyph = glyphs.get((unsigned)characterIndex);
	printCharacter(glyph, characterIndex);
	// space
	if (character != 32 && currentWordIndex >= 0 && words[currentSurfaceNumber].size() > 0)
		words[currentSurfaceNumber][currentWordIndex].id += convertUint16ToUtf8(character);
	else if (subject.currentPos < subject.convertedText.size() - 1)
	{
		if (currentWordIndex >= 0 && words[currentSurfaceNumber].size() > 0)
			newWord(false, true, characterWidths[characterIndex]);
		subject.xposOnSurface += spaceCharExtraW;
	}
	subject.currentPos ++;

	if (subject.currentPos >= subject.convertedText.size() && currentWordIndex >= 0 && words.size() > 0)
	{
		words[currentSurfaceNumber][currentWordIndex].y2 = words[currentSurfaceNumber][currentWordIndex].y1 + text_h;
		words[currentSurfaceNumber][currentWordIndex].x2 = subject.xposOnSurface;
		textWidths[currentSurfaceNumber] = getPrintedTextW();
	}
}

void WordPrinter::trimWords(int surface)
{
	for (int i = 0; i < words[surface].size(); i++)
	{
		if (!finished() && i == words[surface].size() - 1)
			return;
		for (int counter = 0; counter < 2; counter++)
		{
			char c = words[surface][i].id[0]; //first time we check the first char
			if (counter == 1)
				c = words[surface][i].id.back(); //second time we check the last char
			if (!std::isalpha(c)) //subtract width from thw word's box if the character is not alpha or ' or -
			{
				if (c >= '0' && c <= '9') // is a digit; leave it alone
					continue;
				Uint16 char16 = 0;
				int characterIndex = 0;
				int width = 0;
				bool oneByteChar = true;
				if (c > 0x00 && c <= 0x7F)
					char16 = int(c);
				if (char16 != 0) // one byte char
				{
					characterIndex = characters.getIndex(char16);
					width = characterWidths[characterIndex];
				}
				else // multiple byte char; not a punctuation mark but actually part of the word. Leave it alone
					continue;

				if (counter == 0)
				{
					words[surface][i].x1 += width; //first char needs to be removed so the box beginning gets a +
					words[surface][i].id.erase(0, 1);
				}
				else
				{
					words[surface][i].x2 -= width; //last char needs to be removed so shrink the box width (x2 gets a -)
					words[surface][i].id.pop_back();
				}
			}
		}
	}
}

void WordPrinter::clearWordsInIndicedSurface(int surface)
{
	words[surface] = {};
	selectedWords.clear();
	textWidths[surface] = -1;
}

int WordPrinter::getTextWidth(int surfaceNumber)
{
	if (textWidths.find(surfaceNumber) != textWidths.end())
		return textWidths[surfaceNumber];
	return -1;
}
