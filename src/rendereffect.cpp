#include "rendereffect.h"
#include <fstring.h>

static bool isColor(FString color)
{
	if (!color.replace(",", "").allDigits())
		return false;
	std::vector<FString> split = color.split(",", true, true);
	if (split.size() != 3)
		return false;
	for (FString index : split)
	{
		if (index.length() > 3 || index.isEmpty())
			return false;
	}
	return true;
}

RenderEffect::RenderEffect(FString metaText)
{
	isValidMetaText(metaText);
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
			fpcValue = (unsigned)metaText.substring(4).toInt();
		}
		else
		{
			UnsupportedTypeError error;
			throw error;
		}
	}
	else if ((metaText.startsWith("COLOUR") || metaText.startsWith("COLOR")) && metaText.toStdString().length() > 4 && metaText.charAt(3) == '=')
	{
		if (isColor(metaText.substring(4)))
		{
			type = Type::COLOR;
			color.a = 255;
			color.r = metaText.substring(4, 7).toInt();
			color.g = metaText.substring(8, 11).toInt();
			color.b = metaText.substring(12).toInt();
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
