#include "rendereffect.h"
#include <fstring.h>
#include "dialoguesdl_functionality.h"

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

RenderEffect::RenderEffect(FString metaText, SDL_Surface* target, int pointsize) : pointsize(pointsize), target(target)
{
	isValidMetaText(metaText);
	area.x = 0;
	area.y = 0;
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
	switch(type)
	{
	case Type::BOUNCE:
		if (bounce.size() > 0)
			back = bounce[bounce.size() - 1].clip.x + bounce[bounce.size() - 1].clip.w;
		bounce.push_back(Bounce(pointsize, back, expansion - back, area.y, area.h));
		break;
	default:
		return;
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
		break;
	case Type::WAVE:
		break;
	default:
		return;
	}
}

void RenderEffect::applyBounceEffect()
{
	for(unsigned int i = 0; i < bounce.size(); i ++)
	{
		bounce[i].next();
		if (bounce[i].ready)
			DialogueSDL_Functionality::bounce(target, bounce[i].clip, bounce[i].up());
	}
}
