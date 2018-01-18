// *************************************************************
// File:    gl_draw.cc
// Descr:   represents draw functions works with linear FB
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_draw.h"

namespace anshub {

namespace draw_helpers {

// Draws point using Buffer object

void DrawPoint(int x, int y, int color, Buffer& buf)
{
  buf[x + y * buf.Width()] = color;
}

// Draws point using buffer pointer

void DrawPoint(int x, int y, int color, uint* buf, int lpitch)
{
  buf[x + y * lpitch] = color;
}

// The extremely fast line algorithm var.E (additional fixed point precalc)
// Author: Po-Han Lin, http://www.edepot.com

void DrawLine(int x, int y, int x2, int y2, int color, Buffer& buf)
{
  bool yLonger = false;
	int shortLen = y2 - y;
	int longLen = x2 - x;
	if (abs(shortLen) > abs(longLen)) {
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;				
		yLonger = true;
	}
	int decInc;
	if (longLen == 0) 
    decInc=0;
	else 
    decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen > 0) {
			longLen += y;
			for (int j = 0x8000 + (x << 16); y <= longLen; ++y) {
				DrawPoint(j >> 16, y, color, buf);	
				j += decInc;
			}
			return;
		}
		longLen += y;
		for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
			DrawPoint(j >> 16,y, color, buf);	
			j -= decInc;
		}
		return;	
	}

	if (longLen > 0) {
		longLen += x;
		for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
			DrawPoint(x, j >> 16, color, buf);
			j += decInc;
		}
		return;
	}
	longLen += x;
	for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
		DrawPoint(x, j >> 16, color, buf);
		j -= decInc;
	}
}

} // namespace draw_helpers

} // namespace anshub