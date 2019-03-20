#include "textprinter.h"
#include "sdl_ttf_custom.h"
#include <ft2build.h>
#include FT_FREETYPE_H

TextPrinter::TextPrinter(File font, unsigned int font_px, bool highQuality) : font(font), font_px(font_px), fancy(highQuality), characters(true)
{
	if (!TTF_WasInit())
		throw "Can't create TextPrinter object. TTF library was not initialized. Call TTF_Init() first.";
	integrityCheck();
}

TextPrinter::~TextPrinter()
{
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

	charcode = FT_Get_First_Char(TTF_GetFontFace(fontstruct), &gindex );
	while ( gindex != 0 )
	{
		characters.push_back(charcode);
		charcode = FT_Get_Next_Char(TTF_GetFontFace(fontstruct), charcode, &gindex );
	}

	TTF_CloseFont(fontstruct);
}

void TextPrinter::startNewText(std::string text, unsigned int boxW)
{
	resetSubject();
	subject.text = text;
	subject.boxW = boxW;
	checkText();
}

void TextPrinter::setCurrentFPC(unsigned int fpc)
{
	subject.currentFPC = fpc;
}

SDL_Surface* TextPrinter::getPrinted()
{
	return nullptr;
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
		if (!characters.contains(c))
		{
			std::cout << "Error: A certain character in this text string: \"" + subject.text + "\" was not present in font " + font.getAbsolutePath() << " and will be omitted." << std::endl;
			subject.convertedText.push_back(c);
		}
	}
}

void TextPrinter::resetSubject()
{
	if (subject.currentState != nullptr)
		SDL_FreeSurface(subject.currentState);
	Subject clean;
	subject = clean;
	subject.currentColor.a = 255;
	subject.currentColor.b = 255;
	subject.currentColor.g = 255;
	subject.currentColor.r = 255;
}

std::string TextPrinter::parse(std::string text)
{
	FString textStr(text);
	std::vector<unsigned int> metaTextStartIndexes = textStr.findAll("/\\<");
	subject.RenderEffectIndexes = metaTextStartIndexes;
	std::vector<unsigned int> metaTextCloseIndexes = textStr.findAll("/\\>");
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

std::string TextPrinter::extractMetaText(FString text, std::vector<unsigned int> metaTextStartIndexes, std::vector<unsigned int> metaTextCloseIndexes)
{
	std::vector<std::string> metaText;
	for (unsigned int i = 0; i < metaTextStartIndexes.size(); i++)
	{
		FString sub = text.substring(metaTextStartIndexes[i] + 3, metaTextCloseIndexes[i]);
		try
		{
			subject.renderEffects.push_back(RenderEffect(sub));
			metaText.push_back(text.substring(metaTextStartIndexes[i], metaTextCloseIndexes[i] + 3).toStdString());
		}
		catch (RenderEffect::UnsupportedTypeError error)
		{
			throw "Error: Cannot render following text " + text.toStdString() + " because following meta text is of unknown type: " + sub.toStdString();
		}
	}
	for (unsigned int i = 0; i < metaText.size(); i++)
		text.replace(metaText[i], "", false, false);
	if (!unescapedBackslashCheck(text))
		throw "Error: Cannot render following text " + text.toStdString() + " because it contains unescaped backslashes (\\) in illegal places.";
	return text.toStdString();
}
