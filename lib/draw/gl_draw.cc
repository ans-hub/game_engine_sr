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

// Draws solid triangle. First, we should guarantee that y1 is most top, and
// y3 is most bottom points. Then we draw top triangle from top to middle,
// and then draw from bottom to the middle

void draw::SolidTriangle(
  float px1, float py1, float px2, float py2, float px3, float py3, uint color, Buffer& buf)
{
  // Convert float to int

  int x1 = std::floor(px1);
  int x2 = std::floor(px2);
  int x3 = std::floor(px3);
  int y1 = std::floor(py1);
  int y2 = std::floor(py2);
  int y3 = std::floor(py3);

  // Make y1 as top point and y3 as bottom point, y2 is middle

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

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3)
    std::swap(x2, x3);

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2)
    std::swap(x1, x2);

  // Part 1 : draw top part of triangle (from top to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dx_lhs {0.0f};
  float dx_rhs {0.0f};
  
  if (math::FNotZero(y2-y1)) 
    dx_lhs = (float)(x2-x1) / std::abs((float)(y2-y1));
  if (math::FNotZero(y3-y1))
    dx_rhs = (float)(x3-x1) / std::abs((float)(y3-y1));

  // Now choose, where really placed left and right side step

  if (dx_lhs > dx_rhs)
    std::swap(dx_lhs, dx_rhs);
  
  // Now we should draw triangle from top to middle (y1-y2)

  float x_lhs {(float)x1};                // float curr x coord left
  float x_rhs {(float)x1};                // float curr x coord right

  // Clip top and bottom

  if (y1 < 0 || y3 >= buf.Height())       // if triangle is full out of screen
    return;

  int y_top_clip = y1 - buf.Height() + 1; // how much pixels is clipped 
  y_top_clip = std::max(0, y_top_clip);   //  from the top of screen

  x_lhs += dx_lhs * y_top_clip;           // forward x left and x right curr
  x_rhs += dx_rhs * y_top_clip;           //  coords if y1 is out of screen

  int y_top = y1 - y_top_clip;            // define new drawable top
  int y_bot = std::max(0, y2);            //  and bottom

  // Draw top triangle
  
  for (int y = y_top; y >= y_bot; --y)
  {
    int xl = std::floor(x_lhs);
    int xr = std::ceil(x_rhs);
    xl = std::max(0, xl);                 // clip left and right lines
    xr = std::min(buf.Width() - 1, xr);
    draw::HorizontalLine(y, xl, xr, color, buf);
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
  }

  // Part 2 : draw bottom side of triangle (from bottom to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  if (math::FNotZero(y1-y3)) 
    dx_lhs = (float)(x1-x3) / std::abs((float)(y1-y3));
  if (math::FNotZero(y2-y3))
    dx_rhs = (float)(x2-x3) / std::abs((float)(y2-y3));

  // Determine which is really left step and really is right step

  if (dx_lhs > dx_rhs)
    std::swap(dx_lhs, dx_rhs);
    
  // Now we should draw traingle from bottom to middle (y3-y2)

  x_lhs = (float)x3;
  x_rhs = (float)x3;
  
  // Clip top and bottom

  int y_bot_clip {0};                   // here we calc how mush pixels
  if (y3+1 < 0)                         //  is out of screen from bottom
    y_bot_clip = std::abs(y3+1);
  
  x_lhs += dx_lhs * y_bot_clip;         // expand left and right curr coords
  x_rhs += dx_rhs * y_bot_clip;

  y_bot = std::max(0, y3+1);            // new drawable top and bottom
  y_top = std::min(y2, buf.Height()-1);

  // Draw bottom triangle

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
    int xl = std::floor(x_lhs);
    int xr = std::ceil(x_rhs);
    xl = std::max(0, xl);
    xr = std::min(buf.Width()-1, xr);
    draw::HorizontalLine(y, xl, xr, color, buf);
  }
}

// Draws gourang solid triangle. The proccess of drawing is similar
// to the draw::SolidTriangle, but here we interpolate vertexes colors
// (gradient)

void draw::GourangTriangle(
  float px1, float py1, float px2, float py2, float px3, float py3,
  uint col1, uint col2, uint col3, Buffer& buf)
{
  // Convert float to int

  int x1 = std::floor(px1);
  int x2 = std::floor(px2);
  int x3 = std::floor(px3);
  int y1 = std::floor(py1);
  int y2 = std::floor(py2);
  int y3 = std::floor(py3);

  // Prepare colors

  FColor c1 {col1};
  FColor c2 {col2};
  FColor c3 {col3};

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(c2, c3);
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(c1, c2);
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(c1, c2);
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(c3, c2);
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(c2, c3);
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(c1, c2);
  }

  // Part 1 : draw top part of triangle (from top to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dx_lhs {0.0f};
  float dx_rhs {0.0f};

  if (math::FNotZero(y2-y1))
    dx_lhs = (float)(x2-x1) / std::abs((float)(y2-y1));
  if (math::FNotZero(y3-y1))
    dx_rhs = (float)(x3-x1) / std::abs((float)(y3-y1));

  // Calc side colors differential (from top to left bottom and to right bottom)

  FColor dx_lc {c2 - c1};
  FColor dx_rc {c3 - c1};
  dx_lc /= std::abs(y2-y1);
  dx_rc /= std::abs(y3-y1);
  
  // Now choose, where really placed left and right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lc, dx_rc);
  }

  // Now we should draw triangle from top to middle (y1-y2)

  float x_lhs {(float)x1};                // float curr x coord left
  float x_rhs {(float)x1};                // float curr x coord right

  // Clip top and bottom

  if (y1 < 0 || y3 >= buf.Height())       // if triangle is full out of screen
    return;

  int y_top_clip = y1 - buf.Height() + 1; // how much pixels is clipped 
  y_top_clip = std::max(0, y_top_clip);   //  from the top of screen

  x_lhs += dx_lhs * y_top_clip;           // forward x left and x right curr
  x_rhs += dx_rhs * y_top_clip;           //  coords if y1 is out of screen

  int y_top = y1 - y_top_clip;            // define new drawable top
  int y_bot = std::max(0, y2);            //  and bottom

  // Draw top triangle

  for (int y = y_top; y >= y_bot; --y)
  {
    int dy = y1 - y;                      // we need real dy, not clipped
    FColor x_lc = c1 + (dx_lc * dy);      // find colors on the edges
    FColor x_rc = c1 + (dx_rc * dy);

    int xl = std::floor(x_lhs);
    int xr = std::ceil(x_rhs);
    xl = std::max(0, xl);                 // clip left and right lines
    xr = std::min(buf.Width() - 1, xr);
    
    FColor dx_currx_c {};                 // find dx between left and right   
    if ((xr-xl) != 0)
      dx_currx_c = (x_rc-x_lc)/(xr-xl);
    else
      dx_currx_c = c1;

    FColor curr_c {};
    for (int x = xl; x < xr; ++x)        // for each pixel interpolate color
    {
      int dx = x-xl;
      curr_c = x_lc + (dx_currx_c * dx);
      draw::Point(x, y, curr_c.GetARGB(), buf);
    }
    
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
  }

  // Part 2 : draw bottom side of triangle (from bottom to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  if (math::FNotZero(y1-y3)) 
    dx_lhs = (float)(x1-x3) / std::abs((float)(y1-y3));
  if (math::FNotZero(y2-y3))
    dx_rhs = (float)(x2-x3) / std::abs((float)(y2-y3));

  // Calc side colors differential (from bottom to left top and to right top)

  dx_lc = c1 - c3;
  dx_rc = c2 - c3;
  dx_lc /= std::abs(y1-y3-1);   // -1 since we go -1 step less than diff
  dx_rc /= std::abs(y2-y3-1);   // in loop when we would draw triangle

  // Determine which is really left step and really is right step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lc, dx_rc);
  }
    
  // Now we should draw traingle from bottom to middle (y3-y2)

  x_lhs = (float)x3;
  x_rhs = (float)x3;
  
  // Clip top and bottom

  int y_bot_clip {0};                   // here we calc how mush pixels
  if (y3+1 < 0)                         //  is out of screen from bottom
    y_bot_clip = std::abs(y3+1);
  
  x_lhs += dx_lhs * y_bot_clip;         // expand left and right curr coords
  x_rhs += dx_rhs * y_bot_clip;

  y_bot = std::max(0, y3+1);            // new drawable top and bottom
  y_top = std::min(y2, buf.Height()-1);

  // Draw bottom triangle

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;

    int dy = y - y3;                    // we need real dy, not clipped
    FColor x_lc = c3 + (dx_lc * dy);    // find colors on the edges
    FColor x_rc = c3 + (dx_rc * dy);

    int xl = std::floor(x_lhs);
    int xr = std::ceil(x_rhs);
    xl = std::max(0, xl);               // clip left and right lines
    xr = std::min(buf.Width()-1, xr);
    
    FColor dx_currx_c {};               // find dx between left and right
    if ((xr-xl) != 0)
      dx_currx_c = (x_rc-x_lc)/(xr-xl);
    else
      dx_currx_c = c3;
    
    FColor curr_c {};
    for (int x = xl; x < xr; ++x)        // for each pixel interpolate color
    {
      int dx = x-xl;
      curr_c = x_lc + (dx_currx_c * dx);
      draw::Point(x, y, curr_c.GetARGB(), buf);
    }
  }
}

// Draws wired object

void draw::WiredObject(const GlObject& obj, Buffer& buf)
{
  if (!obj.active_)
    return;

  int w = buf.Width();
  int h = buf.Height();
    
  for (const auto& t : obj.triangles_)
  {
    if ((t.attrs_ & Triangle::HIDDEN))
      continue;

    // As we haven`t filling we get first vertexes color

    auto color = obj.colors_trans_[t.f1_].GetARGB();

    // Now get pairs of vectors on the triangle face and draw lines

    auto p1 = obj.vxs_trans_[t.f1_];
    auto p2 = obj.vxs_trans_[t.f2_];

    if (segment2d::Clip(0, 0, w-1, h-1, p1.x, p1.y, p2.x, p2.y))
      draw::Line(p1.x, p1.y, p2.x, p2.y, color, buf);
      
    auto p3 = obj.vxs_trans_[t.f2_];
    auto p4 = obj.vxs_trans_[t.f3_];

    if (segment2d::Clip(0, 0, w-1, h-1, p3.x, p3.y, p4.x, p4.y))
      draw::Line(p3.x, p3.y, p4.x, p4.y, color, buf);

    auto p5 = obj.vxs_trans_[t.f3_];
    auto p6 = obj.vxs_trans_[t.f1_];

    if (segment2d::Clip(0, 0, w-1, h-1, p5.x, p5.y, p6.x, p6.y))
      draw::Line(p5.x, p5.y, p6.x, p6.y, color, buf);
  }
}

// Draws solid object

int draw::SolidObject(const GlObject& obj, Buffer& buf)
{
  int total {0};

  if (!obj.active_)
    return total;
    
  for (const auto& face : obj.triangles_)
  {
    if ((face.attrs_ & Triangle::HIDDEN))
      continue;

    auto p1 = obj.vxs_trans_[face.f1_];
    auto p2 = obj.vxs_trans_[face.f2_];
    auto p3 = obj.vxs_trans_[face.f3_];
    
    if (face.attrs_ & Triangle::GOURANG_SHADING)
    {
      auto c1 = obj.colors_trans_[face.f1_].GetARGB();
      auto c2 = obj.colors_trans_[face.f2_].GetARGB();
      auto c3 = obj.colors_trans_[face.f3_].GetARGB();
      draw::GourangTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, buf);
    }
    else {
      auto color = face.face_color_.GetARGB();
      draw::SolidTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, buf);
    }
    ++total;
  }
  return total;
}

// Draws triangles

void draw::WiredTriangles(const TrianglesRef& arr, Buffer& buf)
{
  // int w = buf.Width();
  // int h = buf.Height();

  // for (const auto& tri : arr)
  // {
  //   if ((tri.attrs_ & Triangle::HIDDEN))
  //     continue;

  //   // As we haven`t filling we get first vertexes color

  //   auto color = tri.colors_[0].GetARGB();
  
  //   // Now get pairs of vectors on the triangle face and draw lines
  
  //   auto p1 = tri.vxs_[0];
  //   auto p2 = tri.vxs_[1];

  //   if (segment2d::Clip(0, 0, w-1, h-1, p1.x, p1.y, p2.x, p2.y))
  //     draw::Line(p1.x, p1.y, p2.x, p2.y, color, buf);
      
  //   auto p3 = tri.vxs_[1];
  //   auto p4 = tri.vxs_[2];

  //   if (segment2d::Clip(0, 0, w-1, h-1, p3.x, p3.y, p4.x, p4.y))
  //     draw::Line(p3.x, p3.y, p4.x, p4.y, color, buf);

  //   auto p5 = tri.vxs_[2];
  //   auto p6 = tri.vxs_[0];

  //   if (segment2d::Clip(0, 0, w-1, h-1, p5.x, p5.y, p6.x, p6.y))
  //     draw::Line(p5.x, p5.y, p6.x, p6.y, color, buf);
  // }
}

// Draws solid triangles

int draw::SolidTriangles(const TrianglesRef& arr, Buffer& buf)
{
  int total {0};
  for (const auto& tri : arr)
  {
    auto& t = tri.get();

    if ((t.attrs_ & Triangle::HIDDEN))
      continue;

    auto p1 = t.v1_;
    auto p2 = t.v2_;
    auto p3 = t.v3_;
    auto c1 = t.c1_.GetARGB();
    auto c2 = t.c2_.GetARGB();
    auto c3 = t.c3_.GetARGB();
    if (t.attrs_ & Triangle::GOURANG_SHADING)
      draw::GourangTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, buf);
    else 
      draw::SolidTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, buf);
    ++total;
  }
  return total;
}

// Here we build segments of normals like: obj.vx[i] is start, ends.vx[i] is end

void draw::ObjectNormals(
  const GlObject& obj, const Vertexes& ends, uint color, Buffer& buf)
{
  auto& vxs = obj.GetCoords();
  for (std::size_t i = 0; i < vxs.size(); ++i)
  {
    auto start = vxs[i];
    auto end = ends[i];
    if (segment2d::Clip(
      0, 0, buf.Width()-1, buf.Height()-1, start.x, start.y, end.x, end.y))
        draw::Line(start.x, start.y, end.x, end.y, color, buf);
  }
}


} // namespace anshub