#include "wordprinter.h"
#include <custom_sdl_functions.h>
#include <sdl_ttf_custom.h>
#include <ft2build.h>
#include FT_FREETYPE_H

WordPrinter::WordPrinter(File font, unsigned int font_px, Mix_Chunk* sound) : TextPrinter(font, font_px, sound)
{
	//ctor
}

void WordPrinter::startNewText(std::string text, unsigned int boxW, unsigned int boxH, unsigned int effectSpeed, int number)
{
	resetSubject();
	subject.boxW = boxW;
	subject.boxH = boxH;
	subject.currentState = createTransparentSurface(boxW, boxH);
	subject.speed = effectSpeed >= 3 ? 3 : effectSpeed;
	mute = false;

	currentWordIndex = -1;
	newWord();
	currentSurfaceNumber = number;

	unsigned char* textp = U8StringToCharArray(text);
	checkText(textp, text.length());
	delete[] textp;
	calcNumCharsConsideringFrameRate();
}

void WordPrinter::newWord(bool wasNewline, bool removeSpace, int spaceW)
{
	if (words.size() > 0)
	{
		words[currentWordIndex].y2 = words[currentWordIndex].y1 + text_h;
		if (!wasNewline)
			words[currentWordIndex].x2 = subject.xposOnSurface;
	}

	currentWordIndex++;
	Word word;
	word.x1 = subject.xposOnSurface;
	if (removeSpace)
		word.x1 += spaceCharExtraW + spaceW;
	word.y1 = subject.yposOnSurface;
	word.surfaceNumber = currentSurfaceNumber;
	words.push_back(word);
}

std::string convertUint16ToUtf8(Uint16 value) {
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
	if (character == 10)
	{
		words[currentWordIndex].x2 = subject.xposOnSurface;
		handleNewLine();
		newWord(true);
		return;
	}
	int characterIndex = characters.getIndex(character);
	if (characterIndex == -1)
		throw "Fatal Error: character could not be found even after the check if all characters were present passed successfully. (For character with UTF8 decimal value " + FString::fromInt(character).toStdString() + ").";
	if(subject.xposOnSurface != (unsigned)textOffset && subject.xposOnSurface + characterWidths[characterIndex] >= subject.boxW)
	{
		subject.lineOffsetsToBoxW.push_back(subject.boxW - subject.xposOnSurface);
		subject.xposOnSurface = textOffset;
		subject.yposOnSurface += text_h;
		if (subject.renderEffects.size() > 0 && subject.renderEffects[subject.indexCurrentRenderEffect].state == RenderEffect::OPEN && !inRenderEffectIndexes(subject.currentPos))
		{
			RenderEffect::Type previousType = subject.renderEffects[subject.indexCurrentRenderEffect].type;
			subject.RenderEffectIndexes.insert(subject.RenderEffectIndexes.begin() + subject.indexCurrentRenderEffect, subject.currentPos);
			subject.renderEffects.insert(subject.renderEffects.begin() + subject.indexCurrentRenderEffect, RenderEffect(previousType, subject.currentState, font_px, subject.speed));
			subject.indexCurrentRenderEffect++;
			updateRenderSettings();
		}
	}
	SDL_Surface* glyph = glyphs.get((unsigned)characterIndex);
	printCharacter(glyph, characterIndex);
	// space
	if (character != 32)
		words[currentWordIndex].id += convertUint16ToUtf8(character);
	else if (subject.currentPos < subject.convertedText.size() - 1)
	{
		newWord(false, true, characterWidths[characterIndex]);
		subject.xposOnSurface += spaceCharExtraW;
	}
	subject.currentPos ++;

	if (subject.currentPos >= subject.convertedText.size())
	{
		words[currentWordIndex].y2 = words[currentWordIndex].y1 + text_h;
		words[currentWordIndex].x2 = subject.xposOnSurface;
	}
}
