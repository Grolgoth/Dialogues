#include "textprinter.h"
#include <custom_sdl_functions.h>
#include <sdl_ttf_custom.h>
#include <ft2build.h>
#include FT_FREETYPE_H

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
	TTF_Font* fontstruct = TTF_OpenFont(font.getAbsolutePath().c_str(), font_px);
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
		if (subject.renderEffects.size() > 0 && subject.renderEffects[subject.indexCurrentRenderEffect].state == RenderEffect::OPEN)
		{
			RenderEffect::Type previousType = subject.renderEffects[subject.indexCurrentRenderEffect].type;
			subject.RenderEffectIndexes.insert(subject.RenderEffectIndexes.begin() + subject.indexCurrentRenderEffect, subject.currentPos + 1);
			subject.renderEffects.insert(subject.renderEffects.begin() + subject.indexCurrentRenderEffect, RenderEffect(previousType, subject.currentState, font_px, subject.speed));
			subject.indexCurrentRenderEffect++;
		}
	}
	SDL_Surface* glyph = glyphs.get((unsigned)characterIndex);
	setColor(glyph, subject.currentColor);
	apply_surface(subject.xposOnSurface, subject.yposOnSurface, glyph, subject.currentState, nullptr);
	subject.xposOnSurface += characterWidths[characterIndex];
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

void TextPrinter::startNewText(std::string text, unsigned int boxW, unsigned int boxH, unsigned int effectSpeed)
{
	resetSubject();
	subject.text = text;
	subject.boxW = boxW;
	subject.boxH = boxH;
	subject.currentState = createTransparentSurface(boxW, boxH);
	subject.speed = effectSpeed >= 3 ? 3 : effectSpeed;
	checkText();
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

void TextPrinter::checkText()
{
	const char* textp = parse(subject.text).c_str();
	size_t textlen = SDL_strlen(textp);
	while ( textlen > 0 )
	{
        Uint16 c = UTF8_getch(&textp, &textlen);
        if ( c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED )
            continue;
		if (!characters.contains(c) && c != 10 && c != 13)
			std::cout << "Error: A certain character (" + FString::fromInt(c).toStdString() + ") in this text string: \"" + subject.text + "\" was not present in font " + font.getAbsolutePath() << " and will be omitted." << std::endl;
		else if (c != 13)
			subject.convertedText.push_back(c);
	}
}

std::string TextPrinter::parse(std::string text)
{
	FString textStr(text);
	std::vector<unsigned int> metaTextStartIndexes = textStr.findAll("/\\\\<");
	subject.RenderEffectIndexes = metaTextStartIndexes;
	std::vector<unsigned int> metaTextCloseIndexes = textStr.findAll("/\\\\>");
	if (metaTextCloseIndexes.size() != metaTextCloseIndexes.size())
		throw "Error: Cannot render following text " + text + " because its meta-format is invalid.";
	return extractMetaText(textStr, metaTextStartIndexes, metaTextCloseIndexes);
}

static bool unescapedBackslashCheck(FString text)
{
	while (text.contains("\\"))
	{
		std::vector<FString> split = text.split("\\");
		if (split.size() != 2 || !split[1].startsWith("\\"))
			return false;
		unsigned int startIndex = text.indexOf("\\", true, 0, 2);
		if (startIndex + 1 == text.length())
			return true;
		text = text.substring(startIndex + 1);
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
	}
}

std::string TextPrinter::extractMetaText(FString text, std::vector<unsigned int> metaTextStartIndexes, std::vector<unsigned int> metaTextCloseIndexes)
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
	for (unsigned int i = 0; i < metaText.size(); i++)
		text = text.replace(metaText[i], "", false, false);
	if (!unescapedBackslashCheck(text))
		throw "Error: Cannot render following text " + text.toStdString() + " because it contains unescaped backslashes (\\) in illegal places.";
	return text.toStdString();
}
