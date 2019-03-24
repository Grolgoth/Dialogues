#include "rendereffect.h"
#include <fstring.h>
#include <custom_sdl_functions.h>
#include "bounce.h"
#include "spin.h"

static bool isColor(FString color)
{
	if (!color.replace(",", "").allDigits())
		return false;
	std::vector<FString> split = color.split(",", true, true);
	if (split.size() != 3)
		return false;
	for (FString index : split)
	{
		if (index.isEmpty() || index.toInt() > 255)
			return false;
	}
	return true;
}

RenderEffect::RenderEffect(FString metaText, SDL_Surface* target, int pointsize, int speed) : pointsize(pointsize), speed(speed), target(target)
{
	isValidMetaText(metaText);
	area.x = 0;
	area.y = 0;
}

RenderEffect::RenderEffect(const RenderEffect& other) : type(other.type), state(other.state), color(other.color), fpcValue(other.fpcValue), pointsize(other.pointsize),
	speed(other.speed), target(other.target), area(other.area), effects(other.effects)
{
	for (RenderEffectCore* index : effects)
		index->copies ++;
}

RenderEffect::~RenderEffect()
{
	for (RenderEffectCore* index : effects)
	{
		if (index->copies == 0)
			delete index;
		else
			index->copies --;
	}
}

bool RenderEffect::isValidMetaText(FString metaText)
{
	if (metaText.toStdString() == "NONE")
		type = Type::NONE;
	else if (metaText.toStdString() == "WAVE")
		type = Type::WAVE;
	else if (metaText.toStdString() == "BOUNCE")
		type = Type::BOUNCE;
	else if (metaText.toStdString() == "SHAKE")
		type = Type::SHAKE;
	else if (metaText.toStdString() == "SPIN")
		type = Type::SPIN;
	else if (metaText.startsWith("FPC", false) && metaText.toStdString().length() > 4 && metaText.charAt(3) == '=' && metaText.substring(4).toStdString().length() < 7)
	{
		if(metaText.substring(4).allDigits())
		{
			type = Type::FPC;
			fpcValue = (unsigned)metaText.substring(4).toInt();
		}
		else
		{
			UnsupportedTypeError error;
			throw error;
		}
	}
	else if (metaText.startsWith("COLOR") && metaText.toStdString().length() > 5 && metaText.charAt(5) == '=')
	{
		if (isColor(metaText.substring(6)))
		{
			type = Type::COLOR;
			color.a = 255;

			std::vector<FString> split = metaText.substring(6).split(",", true, true);
			color.r = split[0].toInt();
			color.g = split[1].toInt();
			color.b = split[2].toInt();
		}
		else
		{
			UnsupportedTypeError error;
			throw error;
		}
	}
	else
	{
		UnsupportedTypeError error;
		throw error;
	}
	return true;
}

void RenderEffect::expandArea(int expansion)
{
	if (area.x + area.w == expansion)
		return;
	area.w = expansion - area.x;
	int back = area.x;
	if (effects.size() > 0)
		back = effects[effects.size() - 1]->clip.x + effects[effects.size() - 1]->clip.w;
	if (type == BOUNCE)
		effects.push_back(new Bounce(pointsize, back, expansion - back, area.y, area.h, 16 - 5 * speed));
	else if (type == SPIN)
	{
		SDL_Rect clip; clip.x = back; clip.w = expansion - back; clip.y = area.y; clip.h = area.h;
		SDL_Surface* glyph = copy_surface(target, &clip);
		effects.push_back(new Spin(pointsize, back, expansion - back, area.y, area.h, 4 - speed, false, glyph));
	}
}

void RenderEffect::apply()
{
	switch(type)
	{
	case Type::BOUNCE:
		applyBounceEffect();
		break;
	case Type::SHAKE:
		break;
	case Type::SPIN:
		applySpinEffect();
		break;
	case Type::WAVE:
		break;
	default:
		return;
	}
}

void RenderEffect::applyBounceEffect()
{
	for(unsigned int i = 0; i < effects.size(); i ++)
	{
		effects[i]->next();
		if (effects[i]->ready)
			shift_pixels_vertical(target, static_cast<Bounce*>(effects[i])->up(), &effects[i]->clip);
	}
}

void RenderEffect::applySpinEffect()
{
	for(unsigned int i = 0; i < effects.size(); i ++)
	{
		effects[i]->next();
		if (effects[i]->ready && effects[i]->currentStep != 0)
		{
			if (static_cast<Spin*>(effects[i])->hasGlyph)
			{
				erase_surface(target, &effects[i]->clip);
				SDL_Surface* modifiedGlyph = copy_surface(static_cast<Spin*>(effects[i])->glyph, nullptr);
				spin_surface(modifiedGlyph, effects[i]->currentStep);
				apply_surface(effects[i]->clip.x, effects[i]->clip.y, modifiedGlyph, target);
				SDL_FreeSurface(modifiedGlyph);
			}
		}
	}
}
