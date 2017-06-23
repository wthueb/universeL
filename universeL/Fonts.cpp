#include "Fonts.h"
#include "Draw.h"

HFont espfont = 0;

void Fonts::Init()
{
	espfont = Draw::CreateFont("Arial", 12, static_cast<int>(FontFlags::FONTFLAG_DROPSHADOW));
}
