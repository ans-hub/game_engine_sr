// *************************************************************
// File:    gl_draw.cc
// Descr:   represents draw functions works with linear FB
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_draw.h"

namespace anshub {

// Draws the line, using Bresengham algorithm

void draw::LineBres(int x1, int y1, int x2, int y2, int color, Buffer& buf)
{
  int dx = x2 - x1;
  int dy = y2 - y1;
  int step_y = 0;
  int step_x = 0;

  // Eval steps based on direction of the line

  if (dy >= 0)                  // line is moves up
    step_y = 1;
  else                          // line is moves down
    step_y = -1;

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

void draw::Line(int x1, int y1, int x2, int y2, int color, Buffer& buf)
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
				draw::Point(j >> 16, y1, color, buf);	
				j += dec_inc;
			}
			return;
		}
		long_len += y1;
		for (int j = 0x8000 + (x1 << 16); y1 >= long_len; --y1) {
			draw::Point(j >> 16, y1, color, buf);	
			j -= dec_inc;
		}
		return;	
	}

	if (long_len > 0) {
		long_len += x1;
		for (int j = 0x8000 + (y1 << 16); x1 <= long_len; ++x1) {
			draw::Point(x1, j >> 16, color, buf);
			j += dec_inc;
		}
		return;
	}
	long_len += x1;
	for (int j = 0x8000 + (y1 << 16); x1 >= long_len; --x1) {
		draw::Point(x1, j >> 16, color, buf);
		j -= dec_inc;
	}
}

// Similar to above function, but with smooth colors
// where b_1 - brightness of first point, b_2 - of second point
// Supposed that b_1 > b_2

void draw::Line(int x1, int y1, int x2, int y2, int color, float b_1, float b_2, Buffer& buf)
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
        draw::Point(j >> 16, y1, curr_color, buf);
				j += dec_inc;
			}
			return;
		}
		long_len += y1;
		for (int j = 0x8000 + (x1 << 16); y1 >= long_len; --y1) {
      curr_color = color::IncreaseBrightness(color, b_1+step_total);
      step_total += bright_step;
      draw::Point(j >> 16, y1, curr_color, buf);	
			j -= dec_inc;
		}
		return;	
	}

	if (long_len > 0) {
		long_len += x1;
		for (int j = 0x8000 + (y1 << 16); x1<= long_len; ++x1) {
      curr_color = color::IncreaseBrightness(color, b_1+step_total);
      step_total += bright_step;
			draw::Point(x1, j >> 16, curr_color, buf);
			j += dec_inc;
		}
		return;
	}
	long_len += x1;
	for (int j = 0x8000 + (y1 << 16); x1 >= long_len; --x1) {
    curr_color = color::IncreaseBrightness(color, b_1 + step_total);
    step_total += bright_step;
		draw::Point(x1, j >> 16, curr_color, buf);
		j -= dec_inc;
	}
}

// Draws the smooth line using Xiaolin Wu algorighm
// Argument names - x1, y1, x2, y2, color, buf

void draw::LineWu(int, int, int, int, int, Buffer&)
{
  // will be implemented using bresenham algorithm as base 
}

// Draws solid triangle

void draw::SolidTriangle(
  float px1, float py1, float px2, float py2, float px3, float py3, uint color, Buffer& buf)
{
  int x1 = std::floor(px1);
  int x2 = std::floor(px2);
  int x3 = std::floor(px3);
  int y1 = std::floor(py1);
  int y2 = std::floor(py2);
  int y3 = std::floor(py3);

  // Make top y1 point and bottom y3 point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(x3, x2);
    std::swap(y3, y2);
  }

  // If polygon is flat top

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    // std::swap(y2, y3);
    std::cerr << "dsa\n";
    // draw::HorizontalLine(y3, std::ceil(px2), std::ceil(px3), color, buf);    
  }

  // If polygon is flat bottom

  if (math::Feq(y1, y2) && y1 > y2) {
    std::swap(x1, x2);
    // draw::HorizontalLine(y1, std::ceil(x1), std::ceil(x2), color, buf);        
    std::cerr << "dsb\n";
    // std::swap(y1, y2);
  } 

  // Part 1 : draw top part of triangle

  // Define step of right and left side (if perpendicular, then step = 0)

  float dx_lhs {0.0f};
  float dx_rhs {0.0f};
  
  if (math::FNotZero(y2-y1)) 
    dx_lhs = (float)(x2-x1) / std::abs((float)(y2-y1));
  if (math::FNotZero(y3-y1))
    dx_rhs = (float)(x3-x1) / std::abs((float)(y3-y1));

  // Check what of this is left and right step

  if (dx_lhs > dx_rhs)
    std::swap(dx_lhs, dx_rhs);
  
  // Draw triangle from top to middle

  float x_lhs {(float)x1};
  float x_rhs {(float)x1};

  for (int y = y1; y >= y2; --y) {
    draw::HorizontalLine(y, std::floor(x_lhs), std::ceil(x_rhs), color, buf);
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
  }

  // Part 2 : draw bottom side of triangle

  if (math::FNotZero(y1-y3)) 
    dx_lhs = (float)(x1-x3) / std::abs((float)(y1-y3));
  if (math::FNotZero(y2-y3))
    dx_rhs = (float)(x2-x3) / std::abs((float)(y2-y3));

  if (dx_lhs > dx_rhs)
    std::swap(dx_lhs, dx_rhs);
    
  x_lhs = (float)x3;
  x_rhs = (float)x3;
  
  for (int y = y3+1; y < y2; ++y) {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
    draw::HorizontalLine(y, std::floor(x_lhs), std::ceil(x_rhs), color, buf);
  }
}

// Draws wired object

void draw::WiredObject(const GlObject& obj, int w, int h, Buffer& buf)
{
  if (!obj.active_)
    return;
    
  for (const auto& t : obj.triangles_)
  {
    if ((t.attrs_ & Triangle::HIDDEN))
      continue;

    // As we haven`t filling we get first vertexes color

    auto color = obj.colors_trans_[t.indicies_[0]].GetARGB();

    // Now get pairs of vectors on the triangle face and draw lines

    auto p1 = obj.vxs_trans_[t.indicies_[0]];
    auto p2 = obj.vxs_trans_[t.indicies_[1]];

    if (segment2d::Clip(0, 0, w-1, h-1, p1.x, p1.y, p2.x, p2.y))
      draw::Line(p1.x, p1.y, p2.x, p2.y, color, buf);
      
    auto p3 = obj.vxs_trans_[t.indicies_[1]];
    auto p4 = obj.vxs_trans_[t.indicies_[2]];

    if (segment2d::Clip(0, 0, w-1, h-1, p3.x, p3.y, p4.x, p4.y))
      draw::Line(p3.x, p3.y, p4.x, p4.y, color, buf);

    auto p5 = obj.vxs_trans_[t.indicies_[2]];
    auto p6 = obj.vxs_trans_[t.indicies_[0]];

    if (segment2d::Clip(0, 0, w-1, h-1, p5.x, p5.y, p6.x, p6.y))
      draw::Line(p5.x, p5.y, p6.x, p6.y, color, buf);
  }
}

// Draws solid object

void draw::SolidObject(const GlObject& obj, int w, int h, Buffer& buf)
{
  if (!obj.active_)
    return;
    
  for (const auto& t : obj.triangles_)
  {
    if ((t.attrs_ & Triangle::HIDDEN))
      continue;

    auto p1 = obj.vxs_trans_[t.indicies_[0]];
    auto p2 = obj.vxs_trans_[t.indicies_[1]];
    auto p3 = obj.vxs_trans_[t.indicies_[2]];
    auto color = obj.colors_trans_[t.indicies_[0]].GetARGB();
    draw::SolidTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, buf);
    // draw::Line(p1.x, p1.y, p2.x, p2.y, color, buf);
    // draw::Line(p2.x, p2.y, p3.x, p3.y, color, buf);
    // draw::Line(p3.x, p3.y, p1.x, p1.y, color, buf);
  }
}

// Draws wired objects

void draw::WiredObjects(const std::vector<GlObject>& arr, int w, int h, Buffer& buf)
{
  for (auto& obj : arr)
    draw::WiredObject(obj, w, h, buf);
}

// Draws triangles

void draw::WiredTriangles(const Triangles& arr, int w, int h, Buffer& buf)
{
  for (const auto& tri : arr)
  {
    if ((tri.attrs_ & Triangle::HIDDEN))
      continue;

    // As we haven`t filling we get first vertexes color

    auto color = tri.colors_[0].GetARGB();
  
    // Now get pairs of vectors on the triangle face and draw lines
  
    auto p1 = tri.vxs_[0];
    auto p2 = tri.vxs_[1];

    if (segment2d::Clip(0, 0, w-1, h-1, p1.x, p1.y, p2.x, p2.y))
      draw::Line(p1.x, p1.y, p2.x, p2.y, color, buf);
      
    auto p3 = tri.vxs_[1];
    auto p4 = tri.vxs_[2];

    if (segment2d::Clip(0, 0, w-1, h-1, p3.x, p3.y, p4.x, p4.y))
      draw::Line(p3.x, p3.y, p4.x, p4.y, color, buf);

    auto p5 = tri.vxs_[2];
    auto p6 = tri.vxs_[0];

    if (segment2d::Clip(0, 0, w-1, h-1, p5.x, p5.y, p6.x, p6.y))
      draw::Line(p5.x, p5.y, p6.x, p6.y, color, buf);
  }
}

} // namespace anshub