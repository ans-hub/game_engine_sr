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
  bool y_longer = false;
	int short_len = y2 - y;
	int long_len = x2 - x;
	if (abs(short_len) > abs(long_len)) {
		int swap = short_len;
		short_len = long_len;
		long_len = swap;				
		y_longer = true;
	}
	int dec_inc;
	if (long_len == 0) 
    dec_inc=0;
	else 
    dec_inc = (short_len << 16) / long_len;

	if (y_longer) {
		if (long_len > 0) {
			long_len += y;
			for (int j = 0x8000 + (x << 16); y <= long_len; ++y) {
				DrawPoint(j >> 16, y, color, buf);	
				j += dec_inc;
			}
			return;
		}
		long_len += y;
		for (int j = 0x8000 + (x << 16); y >= long_len; --y) {
			DrawPoint(j >> 16,y, color, buf);	
			j -= dec_inc;
		}
		return;	
	}

	if (long_len > 0) {
		long_len += x;
		for (int j = 0x8000 + (y << 16); x <= long_len; ++x) {
			DrawPoint(x, j >> 16, color, buf);
			j += dec_inc;
		}
		return;
	}
	long_len += x;
	for (int j = 0x8000 + (y << 16); x >= long_len; --x) {
		DrawPoint(x, j >> 16, color, buf);
		j -= dec_inc;
	}
}

// Similar to above function, but with smooth colors
// where b_1 - brightness of first point, b_2 - of second point
// Supposed that b_1 > b_2

void DrawLine(int x1, int y1, int x2, int y2, int color, double b_1, double b_2, Buffer& buf)
{
  int dx = std::abs(x2-x1);
  int dy = std::abs(y2-y1);
  int points_cnt = std::max(dx,dy);
  double bright_step = (b_2 - b_1) / points_cnt;

  bool y_longer = false;
	int short_len = y2 - y1;
	int long_len = x2 - x1;
  
	if (std::abs(short_len) > std::abs(long_len)) {
		int swap = short_len;
		short_len = long_len;
		long_len = swap;				
		y_longer = true;
	}
	int dec_inc;
	if (long_len == 0) 
    dec_inc=0;
	else 
    dec_inc = (short_len << 16) / long_len;
  
  int curr_color;
  double step_total = 0;

	if (y_longer) {
		if (long_len > 0) {
			long_len += y1;
			for (int j = 0x8000 + (x1 << 16); y1 <= long_len; ++y1) {
				curr_color = color_helpers::IncreaseBrightness(color, b_1+step_total);
        step_total += bright_step;
        DrawPoint(j >> 16, y1, curr_color, buf);
				j += dec_inc;
			}
			return;
		}
		long_len += y1;
		for (int j = 0x8000 + (x1 << 16); y1 >= long_len; --y1) {
      curr_color = color_helpers::IncreaseBrightness(color, b_1+step_total);
      step_total += bright_step;
      DrawPoint(j >> 16, y1, curr_color, buf);	
			j -= dec_inc;
		}
		return;	
	}

	if (long_len > 0) {
		long_len += x1;
		for (int j = 0x8000 + (y1 << 16); x1<= long_len; ++x1) {
      curr_color = color_helpers::IncreaseBrightness(color, b_1+step_total);
      step_total += bright_step;
			DrawPoint(x1, j >> 16, curr_color, buf);
			j += dec_inc;
		}
		return;
	}
	long_len += x1;
	for (int j = 0x8000 + (y1 << 16); x1 >= long_len; --x1) {
    curr_color = color_helpers::IncreaseBrightness(color, b_1 + step_total);
    step_total += bright_step;
		DrawPoint(x1, j >> 16, curr_color, buf);
		j -= dec_inc;
	}
}

// Clips the segment using Liang-Barsky algorithm
// Returns false, if line is outside the clipping area

// We have P1(x1,y1) and P2(x2,y2)
// The parametrized equation is:
//  x = x1 + vx*t
//  y = y1 + vy*t
// where  {x1,y1} - any point on the line (in our case is start point of the segment)
//        {vx,vy} - directing vector of the line (in our case is P2-P1)
//        t - any rational value (if from 0 to 1, the point lies on segment P1-P2)

bool ClipSegment(
  int xmin, int ymin, int xmax, int ymax, int& x1, int& y1, int& x2, int& y2)
{
  enum Sides { LEFT, RIGHT, BOTTOM, TOP};

  int vx = x2 - x1; // x component of directing vector
  int vy = y2 - y1; // y component of directing vector
  double t0 = 0;    // with parametr t1 = 0 we have p1
  double t1 = 1;    // with parametr t2 = 1 we have p2
  
  double p;         // inequalities variables
  double q; 
  double t;

  // Traverse through sides of clippint rect

  for (Sides s = LEFT; s <= TOP; s = (Sides)(s+1))
  {
    switch (s)
    {
      case LEFT   : p = -vx; q = x1 - xmin; break;
      case RIGHT  : p = vx;  q = xmax - x1; break;
      case BOTTOM : p = -vy; q = y1 - ymin; break;
      case TOP    : p = vy;  q = ymax - y1; break;
    }
    t = q/p;

    // p == 0 - line parallel boundary line
    // q < 0  - line outside boundary line
    // p < 0  - line proceeds outside to inside (and vice verca)
    // p !=0  - then r is intersection point

    if (p < 0)
      t0 = std::max(t0, t);
    else if (p > 0)
      t1 = std::min(t1, t);
    else if (q < 0)           // parallel and outside
      return false;
    
    if (t0 > t1) {
      return false;
    }
  }

  x2 = x1 + (t1 * vx),
  y2 = y1 + (t1 * vy);
  x1 += (t0 * vx),
  y1 += (t0 * vy);

  return true;
}

} // namespace draw_helpers

} // namespace anshub