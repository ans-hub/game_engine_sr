// *************************************************************
// File:    gl_draw.cc
// Descr:   represents draw functions works with linear FB
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_draw.h"

namespace anshub {

// Draws the line, using Bresengham algorithm

void draw::DrawLineBres(int x1, int y1, int x2, int y2, int color, Buffer& buf)
{
  int dx = x2 - x1;
  int dy = y2 - y1;
  int step_y = 0;
  int step_x = 0;

  // Eval steps based on direction of the line

  if (dy >= 0)                  // line is moves up
    step_y = 1;//buf.Width();
  else                          // line is moves down
    step_y = -1;//-buf.Width();

  if (dx >= 0)                  // line is moves right
    step_x = 1;
  else                          // line is moves left
    step_x = -1;

  dx = std::abs(dx);
  dy = std::abs(dy);
  
  // Draws the line

  int d_err = 0;

  if (dx > dy)                  // line is horisontal oriented
  {
    d_err = dy;
    while (x1 != x2)
    {
      buf[x1 + y1 * buf.Width()] = color;
      if (d_err >= dx)
      {
        d_err -= dx;
        y1 += step_y;
      }
      x1 += step_x;
      d_err += dy;
    }
  }
  else {                        // line is vertical oriented
    d_err = dx;
    while (y1 != y2)
    {
      buf[x1 + y1 * buf.Width()] = color;
      if (d_err >= dy)
      {
        d_err -= dy;
        x1 += step_x;      
      }
      y1 += step_y;
      d_err += dx;
    }
  }
}

// Draws the line
// The extremely fast line algorithm var.E (additional fixed point precalc)
// Author: Po-Han Lin, http://www.edepot.com

void draw::DrawLine(int x1, int y1, int x2, int y2, int color, Buffer& buf)
{
  bool y_longer = false;
	int short_len = y2 - y1;
	int long_len = x2 - x1;
	if (abs(short_len) > abs(long_len)) {
		int swap = short_len;
		short_len = long_len;
		long_len = swap;				
		y_longer = true;
	}
	int dec_inc;
	if (long_len == 0) 
    dec_inc = 0;
	else 
    dec_inc = (short_len << 16) / long_len;

	if (y_longer) {
		if (long_len > 0) {
			long_len += y1;
			for (int j = 0x8000 + (x1 << 16); y1 <= long_len; ++y1) {
				DrawPoint(j >> 16, y1, color, buf);	
				j += dec_inc;
			}
			return;
		}
		long_len += y1;
		for (int j = 0x8000 + (x1 << 16); y1 >= long_len; --y1) {
			DrawPoint(j >> 16, y1, color, buf);	
			j -= dec_inc;
		}
		return;	
	}

	if (long_len > 0) {
		long_len += x1;
		for (int j = 0x8000 + (y1 << 16); x1 <= long_len; ++x1) {
			DrawPoint(x1, j >> 16, color, buf);
			j += dec_inc;
		}
		return;
	}
	long_len += x1;
	for (int j = 0x8000 + (y1 << 16); x1 >= long_len; --x1) {
		DrawPoint(x1, j >> 16, color, buf);
		j -= dec_inc;
	}
}

// Similar to above function, but with smooth colors
// where b_1 - brightness of first point, b_2 - of second point
// Supposed that b_1 > b_2

void draw::DrawLine(int x1, int y1, int x2, int y2, int color, float b_1, float b_2, Buffer& buf)
{
  int dx = std::abs(x2-x1);
  int dy = std::abs(y2-y1);
  int points_cnt = std::max(dx,dy);
  float bright_step = (b_2 - b_1) / points_cnt;

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
  
  int   curr_color;
  float step_total = 0;

	if (y_longer) {
		if (long_len > 0) {
			long_len += y1;
			for (int j = 0x8000 + (x1 << 16); y1 <= long_len; ++y1) {
				curr_color = color::IncreaseBrightness(color, b_1+step_total);
        step_total += bright_step;
        DrawPoint(j >> 16, y1, curr_color, buf);
				j += dec_inc;
			}
			return;
		}
		long_len += y1;
		for (int j = 0x8000 + (x1 << 16); y1 >= long_len; --y1) {
      curr_color = color::IncreaseBrightness(color, b_1+step_total);
      step_total += bright_step;
      DrawPoint(j >> 16, y1, curr_color, buf);	
			j -= dec_inc;
		}
		return;	
	}

	if (long_len > 0) {
		long_len += x1;
		for (int j = 0x8000 + (y1 << 16); x1<= long_len; ++x1) {
      curr_color = color::IncreaseBrightness(color, b_1+step_total);
      step_total += bright_step;
			DrawPoint(x1, j >> 16, curr_color, buf);
			j += dec_inc;
		}
		return;
	}
	long_len += x1;
	for (int j = 0x8000 + (y1 << 16); x1 >= long_len; --x1) {
    curr_color = color::IncreaseBrightness(color, b_1 + step_total);
    step_total += bright_step;
		DrawPoint(x1, j >> 16, curr_color, buf);
		j -= dec_inc;
	}
}

// Draws the smooth line using Xiaolin Wu algorighm
// Argument names - x1, y1, x2, y2, color, buf

void draw::DrawLineWu(int, int, int, int, int, Buffer&)
{
  // will be implemented using bresenham algorithm as base 
}

// Draws object

void draw::Object(const GlObject& obj, int w, int h, Buffer& buf)
{
  if (!obj.active_)
    return;
    
  for (const auto& t : obj.triangles_)
  {
    if ((t.attrs_ & Triangle::HIDDEN))
      continue;

    auto p1 = obj.vxs_trans_[t.indicies_[0]];
    auto p2 = obj.vxs_trans_[t.indicies_[1]];

    if (segment2d::Clip(0, 0, w-1, h-1, p1.x, p1.y, p2.x, p2.y))
      draw::DrawLine(p1.x, p1.y, p2.x, p2.y, t.color_, buf);
      
    auto p3 = obj.vxs_trans_[t.indicies_[1]];
    auto p4 = obj.vxs_trans_[t.indicies_[2]];

    if (segment2d::Clip(0, 0, w-1, h-1, p3.x, p3.y, p4.x, p4.y))
      draw::DrawLine(p3.x, p3.y, p4.x, p4.y, t.color_, buf);

    auto p5 = obj.vxs_trans_[t.indicies_[2]];
    auto p6 = obj.vxs_trans_[t.indicies_[0]];

    if (segment2d::Clip(0, 0, w-1, h-1, p5.x, p5.y, p6.x, p6.y))
      draw::DrawLine(p5.x, p5.y, p6.x, p6.y, t.color_, buf);
  }
}

// Draws objects

void draw::Objects(const std::vector<GlObject>& arr, int w, int h, Buffer& buf)
{
  for (auto& obj : arr)
    draw::Object(obj, w, h, buf);
}

// Draws triangles

void draw::TrianglesArray(const Triangles& arr, int w, int h, Buffer& buf)
{
  for (const auto& tri : arr)
  {
    if ((tri.attrs_ & Triangle::HIDDEN))
      continue;
  
    auto p1 = tri.vxs_[0];
    auto p2 = tri.vxs_[1];

    if (segment2d::Clip(0, 0, w-1, h-1, p1.x, p1.y, p2.x, p2.y))
      draw::DrawLine(p1.x, p1.y, p2.x, p2.y, tri.color_, buf);
      
    auto p3 = tri.vxs_[1];
    auto p4 = tri.vxs_[2];

    if (segment2d::Clip(0, 0, w-1, h-1, p3.x, p3.y, p4.x, p4.y))
      draw::DrawLine(p3.x, p3.y, p4.x, p4.y, tri.color_, buf);

    auto p5 = tri.vxs_[2];
    auto p6 = tri.vxs_[0];

    if (segment2d::Clip(0, 0, w-1, h-1, p5.x, p5.y, p6.x, p6.y))
      draw::DrawLine(p5.x, p5.y, p6.x, p6.y, tri.color_, buf);
  }
}

} // namespace anshub