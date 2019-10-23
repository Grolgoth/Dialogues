#include "textprinter.h"
#include <custom_sdl_functions.h>
#include <sdl_ttf_custom.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <math.h>

TextPrinter::TextPrinter(File font, unsigned int font_px) : font(font), font_px(font_px), textOffset(font_px / 2), characters(true)
{
	if (!TTF_WasInit())
		throw "Can't create TextPrinter object. TTF library was not initialized. Call TTF_Init() first.";
	integrityCheck();
}

TextPrinter::TextPrinter(const TextPrinter& other)  : font(other.font)
{
	throw "Error. Textprinters should never be copied. There are way too many pointers to pixel images that are all going to be closed in the base objects destructor rendering the copied object useless. Create a new one.";
}

TextPrinter::~TextPrinter()
{
	for (SDL_Surface* glyph : glyphs.toStdVector())
		SDL_FreeSurface(glyph);
	if (subject.currentState != nullptr)
		SDL_FreeSurface(subject.currentState);
}

void TextPrinter::integrityCheck()
{
	if (font_px < 6)
		throw "ERROR: TextPrinter could not be created, specified font size is too small (font size cannot be smaller than 6 px).";
	if (!font.exists())
		throw "ERROR: TextPrinter could not be created, font specified at " + font.getAbsolutePath() + " could not be found.";
	loadFaceFromFile();
}

void TextPrinter::loadFaceFromFile()
{
	if (!font.exists())
		throw "Can't open this font because it doesn't exist: " + font.getAbsolutePath();
	TTF_Font* fontstruct = TTF_OpenFont(font.getAbsolutePath().c_str(), font_px);
	if (fontstruct == nullptr)
		throw TTF_GetError();
	FT_ULong charcode;
	FT_UInt gindex;
	SDL_Color defaultColor; defaultColor.a = 255; defaultColor.b = 0; defaultColor.g = 0; defaultColor.r = 0;

	charcode = FT_Get_First_Char(TTF_GetFontFace(fontstruct), &gindex );
	while ( gindex != 0 )
	{
		characters.push_back(charcode);
		int maxx, maxy;
		TTF_GlyphMetrics(fontstruct, charcode, nullptr, &maxx, nullptr, &maxy, nullptr);
		characterWidths.push_back(maxx);
		if (charcode == 69)
			text_w = maxx;
		text_h = 1.5 * (double)font_px;
		glyphs.push_back(TTF_RenderGlyph_Solid(fontstruct, charcode, defaultColor));
		charcode = FT_Get_Next_Char(TTF_GetFontFace(fontstruct), charcode, &gindex );
	}
	TTF_CloseFont(fontstruct);
}

void TextPrinter::setCurrentFPC(unsigned int fpc)
{
	subject.currentFPC = fpc;
}

SDL_Surface* TextPrinter::getPrinted()
{
	updateActiveRenderEffects();
	if (subject.currentPos < subject.convertedText.size())
	{
		do
		{
			updateRenderSettings();
			if (subject.framesSinceLastPrint == 0)
				printNext();
			if (++subject.framesSinceLastPrint >= subject.currentFPC)
				subject.framesSinceLastPrint = 0;
		} while(subject.currentFPC == 0 && subject.currentPos < subject.convertedText.size());
		if (finished())
			updateRenderSettings();
	}
	return subject.currentState;
}

static bool isDefaultColor(SDL_Color color)
{
	return color.r == 0 && color.g == 0 && color.b == 0;
}

void TextPrinter::setColor(SDL_Surface* target, SDL_Color color)
{
	SDL_Color source;
	if (changedColors.find(target) != changedColors.end())
	{
		SDL_Color changedSource = changedColors.find(target)->second;
		source.r = changedSource.r; source.g = changedSource.g; source.b = changedSource.b;
		changedColors.erase(changedColors.find(target));
	}
	else if (isDefaultColor(color))
		return;
	else
	{
		source.r = 0; source.g = 0; source.b = 0;
	}
	set_color(target, source, color);
	if (color.r != 0 || color.g != 0 || color.b != 0)
		changedColors.insert(std::pair<SDL_Surface*, SDL_Color>(target, color));
}

void TextPrinter::printCharacter(SDL_Surface* glyph, int characterIndex)
{
	setColor(glyph, subject.currentColor);
	int extraX = 0;
	if (subject.indexCurrentRenderEffect != -1 && subject.renderEffects[subject.indexCurrentRenderEffect].type == RenderEffect::SHAKE)
	{
		int width = characterWidths[characterIndex];
		do
		{
			extraX += 3;
			width -= 30;
		}
		while (width > 30);
	}
	apply_surface(subject.xposOnSurface + extraX, subject.yposOnSurface, glyph, subject.currentState, nullptr);
	subject.xposOnSurface += characterWidths[characterIndex] + 2 * extraX;
}

void TextPrinter::printNext()
{
	Uint16 character = subject.convertedText[subject.currentPos];
	if (character == 10)
	{
		handleNewLine();
		return;
	}
	int characterIndex = characters.getIndex(character);
	if (characterIndex == -1)
		throw "Fatal Error: character could not be found even after the check if all characters were present passed successfully. (For character with UTF8 decimal value " + FString::fromInt(character).toStdString() + ").";
	if(subject.xposOnSurface != (unsigned)textOffset && subject.xposOnSurface + characterWidths[characterIndex] >= subject.boxW)
	{
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
	if (character == 32)
		subject.xposOnSurface += text_w;
	subject.currentPos ++;
}

void TextPrinter::handleNewLine()
{
	subject.xposOnSurface = textOffset;
	subject.yposOnSurface += text_h;
	subject.currentPos ++;
	subject.framesSinceLastPrint = 0;
}

unsigned char* U8StringToCharArray(std::string u8String)
{
	unsigned char* buffer = new unsigned char[u8String.length()];
	int i = 0;
	for (char a : u8String)
	{
		int ai = int(a);
		if (ai < 0)
		{
			ai = 256 + ai;
		}
		buffer[i] = ai;
		i++;
	}
	return buffer;
}

void TextPrinter::startNewText(std::string text, unsigned int boxW, unsigned int boxH, unsigned int effectSpeed)
{
	resetSubject();
	subject.boxW = boxW;
	subject.boxH = boxH;
	subject.currentState = createTransparentSurface(boxW, boxH);
	subject.speed = effectSpeed >= 3 ? 3 : effectSpeed;

	unsigned char* textp = U8StringToCharArray(text);
	checkText(textp, text.length());
	delete[] textp;
}

void TextPrinter::resetSubject()
{
	if (subject.currentState != nullptr)
		SDL_FreeSurface(subject.currentState);
	Subject clean;
	subject = clean;
	subject.currentColor.a = 255;
	subject.currentColor.b = 0;
	subject.currentColor.g = 0;
	subject.currentColor.r = 0;
	subject.xposOnSurface = textOffset;
	subject.yposOnSurface = textOffset;
}

void TextPrinter::updateRenderSettings()
{
	if (subject.indexCurrentRenderEffect != -1)
		subject.renderEffects[subject.indexCurrentRenderEffect].expandArea(subject.xposOnSurface);
	for (unsigned int i = 0; i < subject.RenderEffectIndexes.size(); i++)
	{
		if (subject.RenderEffectIndexes[i] == subject.currentPos)
		{
			processRenderEffect(subject.renderEffects[i], i);
			subject.RenderEffectIndexes[i] = -1;
		}
	}
}

void TextPrinter::processRenderEffect(RenderEffect effect, int index)
{
	if (effect.getType() == RenderEffect::FPC)
		subject.currentFPC = effect.getFpcValue();
	else if (effect.getType() == RenderEffect::COLOR)
		subject.currentColor = effect.getColor();
	else
	{
		subject.indexCurrentRenderEffect = index;
		subject.renderEffects[index].area.x = subject.xposOnSurface;
		subject.renderEffects[index].area.y = subject.yposOnSurface;
		subject.renderEffects[index].area.h = text_h;
		subject.renderEffects[index].state = RenderEffect::OPEN;
		if (index!= 0 && subject.renderEffects[index-1].state == RenderEffect::OPEN)
			subject.renderEffects[index-1].state = RenderEffect::SET;
	}
}

void TextPrinter::updateActiveRenderEffects()
{
	for (unsigned int i = 0; i < subject.renderEffects.size(); i++)
	{
		if (subject.renderEffects[i].isActive())
			subject.renderEffects[i].apply();
	}
}

Uint16 fourByteChar(unsigned char* bytes)
{
	Uint16 result = 0;
	int bigPictureValue = int(bytes[0]) - 240;
	for (double n = 2.0; bigPictureValue > 0; n--)
	{
		if (bigPictureValue < pow(2.0, n))
			continue;
		result += pow(2.0, n + 18.0);
		bigPictureValue -= pow (2.0, n);
	}
	for (unsigned int i = 1; i < 4; i++)
	{
		bigPictureValue = int(bytes[i]) - 128;
		for (double n = 5.0; bigPictureValue > 0; n--)
		{
			if (bigPictureValue < pow(2.0, n))
				continue;
			result += pow(2.0, n + ((3 - i) * 6));
			bigPictureValue -= pow (2.0, n);
		}
	}
	return result;
}

Uint16 threeByteChar(unsigned char* bytes)
{
	Uint16 result = 0;
	int bigPictureValue = int(bytes[0]) - 224;
	for (double n = 3.0; bigPictureValue > 0; n--)
	{
		if (bigPictureValue < pow(2.0, n))
			continue;
		result += pow(2.0, n + 12.0);
		bigPictureValue -= pow (2.0, n);
	}
	for (unsigned int i = 1; i < 3; i++)
	{
		bigPictureValue = int(bytes[i]) - 128;
		for (double n = 5.0; bigPictureValue > 0; n--)
		{
			if (bigPictureValue < pow(2.0, n))
				continue;
			result += pow(2.0, n + ((2 - i) * 6));
			bigPictureValue -= pow (2.0, n);
		}
	}
	return result;
}

Uint16 twoByteChar(unsigned char* bytes)
{
	Uint16 result = 0;
	int bigPictureValue = int(bytes[0]) - 192;
	for (double n = 4.0; bigPictureValue > 0; n--)
	{
		if (bigPictureValue < pow(2.0, n))
			continue;
		result += pow(2.0, n + 6.0);
		bigPictureValue -= pow (2.0, n);
	}
	bigPictureValue = int(bytes[1]) - 128;
	for (double n = 5.0; bigPictureValue > 0; n--)
	{
		if (bigPictureValue < pow(2.0, n))
			continue;
		result += pow(2.0, n);
		bigPictureValue -= pow (2.0, n);
	}
	return result;
}

void TextPrinter::checkText(unsigned char* text, unsigned int bytes)
{
	for (unsigned int i = 0; i < bytes; i++)
	{
		Uint16 result = 0;
		if (int(text[i]) == 0)
			break;
		if (text[i] > 0x00 && text[i] <= 0x7F)
			result = int(text[i]);
		else if (text[i] >= 0xC0 && text[i] <= 0xDF)
		{
			unsigned char refs[2] = {text[i], text[i + 1]};
			result = twoByteChar(refs);
			i++;
		}
		else if (text[i] >= 0xE0 && text[i] <= 0xEF)
		{
			unsigned char refs[3] = {text[i], text[i + 1], text[i + 2]};
			result = threeByteChar(refs);
			i += 2;
		}
		else if (text[i] >= 0xF0 && text[i] <= 0xF7)
		{
			unsigned char refs[4] = {text[i], text[i + 1], text[i + 2], text[i + 3]};
			result = fourByteChar(refs);
			i += 3;
		}
		else
			throw "Invalid UTF-8 encoding";
		if (!characters.contains(result) && result != 10 && result != 13)
		{
			std::string textstring = "";
			for (unsigned int i = 0; i < bytes; i++)
				textstring += text[i];
			std::cout << "Error: A certain character (" + FString::fromInt(result).toStdString() + ") was in this text string: " + textstring + " was not present in font " + font.getAbsolutePath() << " and will be omitted." << std::endl;
		}
		else
			subject.convertedText.push_back(result);
	}
	parse();
}

void TextPrinter::parse()
{
	FString textStr(subjectTextToSimpleString());
	std::vector<unsigned int> metaTextStartIndexes = textStr.findAll("/\\\\<");
	subject.RenderEffectIndexes = metaTextStartIndexes;
	std::vector<unsigned int> metaTextCloseIndexes = textStr.findAll("/\\\\>");
	if (metaTextCloseIndexes.size() != metaTextCloseIndexes.size())
		throw "Error: Cannot render following text " + textStr.toStdString() + " because its meta-format is invalid.";
	extractMetaText(textStr, metaTextStartIndexes, metaTextCloseIndexes);
}

bool TextPrinter::unescapedBackslashCheck(FString text)
{
	int startIndex = 0;
	while (text.indexOf("\\", true, startIndex) > -1)
	{
		startIndex = text.indexOf("\\", true, startIndex);
		if (unsigned(startIndex + 1) == text.length())
			return false;
		char next = text.charAt(startIndex + 1);
		if (next == '\\')
		{
			text = text.replace("\\\\", "\\", true, false);
			subject.convertedText.erase(subject.convertedText.begin() + startIndex);
		}
		else if(next == 'n')
		{
			text = text.replace("\\n", "\n");
			subject.convertedText.erase(subject.convertedText.begin() + startIndex, subject.convertedText.begin() + startIndex + 2);
			subject.convertedText.insert(subject.convertedText.begin() + startIndex, 10);
		}
		else
			return false;
		startIndex ++;
	}
	return true;
}

void TextPrinter::correctRenderEffectIndexes(std::vector<std::string> metaText)
{
	int total_offset = 0;
	for(unsigned int i = 0; metaText.size() > 1 && i < metaText.size() - 1; i++)
	{
		total_offset += metaText[i].length();
		subject.RenderEffectIndexes[i + 1] -= total_offset;
		subject.convertedText.erase(subject.convertedText.begin() + subject.RenderEffectIndexes[i], subject.convertedText.begin() + subject.RenderEffectIndexes[i] + metaText[i].length());
	}
	if (subject.RenderEffectIndexes.size() > 0)
		subject.convertedText.erase(subject.convertedText.begin() + subject.RenderEffectIndexes.back(), subject.convertedText.begin() + subject.RenderEffectIndexes.back() + metaText.back().length());
}

void TextPrinter::extractMetaText(FString text, std::vector<unsigned int> metaTextStartIndexes, std::vector<unsigned int> metaTextCloseIndexes)
{
	std::vector<std::string> metaText;
	for (unsigned int i = 0; i < metaTextStartIndexes.size(); i++)
	{
		FString sub = text.substring(metaTextStartIndexes[i] + 4, metaTextCloseIndexes[i]);
		try
		{
			subject.renderEffects.push_back(RenderEffect(sub, subject.currentState, font_px, subject.speed));
			metaText.push_back(text.substring(metaTextStartIndexes[i], metaTextCloseIndexes[i] + 4).toStdString());
		}
		catch (RenderEffect::UnsupportedTypeError error)
		{
			throw "Error: Cannot render following text " + text.toStdString() + " because following meta text is of unknown type: " + sub.toStdString();
		}
	}
	correctRenderEffectIndexes(metaText);
	if (!unescapedBackslashCheck(FString(subjectTextToSimpleString())))
		throw "Error: Cannot render following text " + text.toStdString() + " because it contains unescaped backslashes (\\) in illegal places.";
}

std::string TextPrinter::subjectTextToSimpleString()
{
	std::string text = "";
	for (Uint16 index : subject.convertedText)
	{
		if (index < 128)
			text += char(index);
		else
			text += "O";
	}
	return text;
}
