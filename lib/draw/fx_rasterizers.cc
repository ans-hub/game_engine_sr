// *************************************************************
// File:    fx_rasterizers.cc
// Descr:   rasterizes points, lines and triangles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "fx_rasterizers.h"

namespace anshub {

// Draws the line, using Bresengham algorithm

void raster::LineBres(int x1, int y1, int x2, int y2, int color, Buffer& buf)
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

void raster::Line(int x1, int y1, int x2, int y2, int color, Buffer& buf)
{
  bool y_longer = false;
	int short_len = y2 - y1;
	int long_len = x2 - x1;
	if (abs(short_len) > abs(long_len)) { // todo: c-abs??? not std::abs? are you sure?
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
				raster::Point(j >> 16, y1, color, buf);	
				j += dec_inc;
			}
			return;
		}
		long_len += y1;
		for (int j = 0x8000 + (x1 << 16); y1 >= long_len; --y1) {
			raster::Point(j >> 16, y1, color, buf);	
			j -= dec_inc;
		}
		return;	
	}

	if (long_len > 0) {
		long_len += x1;
		for (int j = 0x8000 + (y1 << 16); x1 <= long_len; ++x1) {
			raster::Point(x1, j >> 16, color, buf);
			j += dec_inc;
		}
		return;
	}
	long_len += x1;
	for (int j = 0x8000 + (y1 << 16); x1 >= long_len; --x1) {
		raster::Point(x1, j >> 16, color, buf);
		j -= dec_inc;
	}
}

// Similar to above function, but with smooth colors
// where b_1 - brightness of first point, b_2 - of second point
// Supposed that b_1 > b_2

void raster::Line(int x1, int y1, int x2, int y2, int color, float b_1, float b_2, Buffer& buf)
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
        raster::Point(j >> 16, y1, curr_color, buf);
				j += dec_inc;
			}
			return;
		}
		long_len += y1;
		for (int j = 0x8000 + (x1 << 16); y1 >= long_len; --y1) {
      curr_color = color::IncreaseBrightness(color, b_1+step_total);
      step_total += bright_step;
      raster::Point(j >> 16, y1, curr_color, buf);	
			j -= dec_inc;
		}
		return;	
	}

	if (long_len > 0) {
		long_len += x1;
		for (int j = 0x8000 + (y1 << 16); x1<= long_len; ++x1) {
      curr_color = color::IncreaseBrightness(color, b_1+step_total);
      step_total += bright_step;
			raster::Point(x1, j >> 16, curr_color, buf);
			j += dec_inc;
		}
		return;
	}
	long_len += x1;
	for (int j = 0x8000 + (y1 << 16); x1 >= long_len; --x1) {
    curr_color = color::IncreaseBrightness(color, b_1 + step_total);
    step_total += bright_step;
		raster::Point(x1, j >> 16, curr_color, buf);
		j -= dec_inc;
	}
}

// Draws solid triangle. First, we should guarantee that y1 is most top, and
// y3 is most bottom points. Then we draw top triangle from top to middle,
// and then draw from bottom to the middle

void raster_tri::SolidFL(
    float px1, float py1, float px2,
    float py2, float px3, float py3,
    uint color, Buffer& buf)
{
  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (py2 < py3) {
    std::swap(px2, px3);
    std::swap(py2, py3);
  }
  if ((py1 < py2) && (py1 > py3)) {
    std::swap(px1, px2);
    std::swap(py1, py2);
  }
  else if ((py1 < py2) && (py1 < py3 || math::Feq(py1, py3))) {
    std::swap(px1, px2);
    std::swap(py1, py2);
    std::swap(px3, px2);
    std::swap(py3, py2);
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(py2, py3) && px2 > px3)
    std::swap(px2, px3);

  // If polygon is flat top, sort left to right

  if (math::Feq(py1, py2) && px1 > px2)
    std::swap(px1, px2);

  // Convert float to int

  int x1 = floor(px1);
  int x2 = floor(px2);
  int x3 = floor(px3);
  int y1 = floor(py1);
  int y2 = floor(py2);
  int y3 = floor(py3);
  
  // Special case - when object is less than 1 px, but visible

  if (y1 == y3) {
    y1 += 1;
    y3 = y1 - 1;
    y2 = y3;
  }

  // Part 1 : draw top part of triangle (from top to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dx_lhs {0.0f};
  float dx_rhs {0.0f};
  
  float dy2y1 = std::abs(y2-y1);
  float dy3y1 = std::abs(y3-y1);

  if (math::FNotZero(dy2y1))
    dx_lhs = (float)(x2-x1) / dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rhs = (float)(x3-x1) / dy3y1;

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
    int xlb = floor(x_lhs);
    int xrb = ceil(x_rhs);
    xlb = std::max(0, xlb);               // clip left and right lines
    xrb = std::min(buf.Width() - 1, xrb);
    raster::HorizontalLine(y, xlb, xrb, color, buf);
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
  }

  // Part 2 : draw bottom side of triangle (from bottom to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dy1y3 = std::abs(y1-y3);
  float dy2y3 = std::abs(y2-y3);

  if (math::FNotZero(dy1y3)) 
    dx_lhs = (float)(x1-x3) / dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rhs = (float)(x2-x3) / dy2y3;

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
    int xlb = floor(x_lhs);
    int xrb = ceil(x_rhs);
    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width()-1, xrb);
    raster::HorizontalLine(y, xlb, xrb, color, buf);
  }
}

// Draws gouraud solid triangle. The proccess of drawing is similar
// to the draw::SolidTriangle, but here we interpolate vertexes colors
// (gradient)

void raster_tri::SolidGR(
    float px1, float py1, float px2,
    float py2, float px3, float py3,
    uint col1, uint col2, uint col3, Buffer& buf)
{
  // Convert float to int

  int x1 = floor(px1);
  int x2 = floor(px2);
  int x3 = floor(px3);
  int y1 = floor(py1);
  int y2 = floor(py2);
  int y3 = floor(py3);

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

  float dy2y1 = std::abs(y2-y1);
  float dy3y1 = std::abs(y3-y1);

  if (math::FNotZero(dy2y1))
    dx_lhs = (float)(x2-x1) / dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rhs = (float)(x3-x1) / dy3y1;

  // Calc side colors differential (from top to left bottom and to right bottom)

  FColor dx_lc {c2 - c1};
  FColor dx_rc {c3 - c1};

  if (math::FNotZero(dy2y1))
    dx_lc /= dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rc /= dy3y1;
  
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
    
    // Compute differentials of colors on the edges

    FColor x_lc = c1 + (dx_lc * dy);
    FColor x_rc = c1 + (dx_rc * dy);

    // Compute x for left edge and right edges

    int xlb = floor(x_lhs);
    int xrb = ceil(x_rhs);

    // Compute color offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential of color between edges at the current y before
    // clipping left and right sides

    FColor dx_currx_c {};
    if ((xrb - xlb) != 0)
      dx_currx_c = (x_rc-x_lc)/(xrb-xlb);
    else
      dx_currx_c = c1;

    // Clip left and right lines of face
    
    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width() - 1, xrb);
    
    // Interpolate color for each pixel    

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;           // real, not clipped dx
      FColor curr_c {x_lc + (dx_currx_c * dx)};
      raster::Point(x, y, curr_c.GetARGB(), buf);
    }
    
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
  }

  // Part 2 : draw bottom side of triangle (from bottom to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dy1y3 = std::abs(y1-y3);
  float dy2y3 = std::abs(y2-y3);

  if (math::FNotZero(dy1y3)) 
    dx_lhs = (float)(x1-x3) / dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rhs = (float)(x2-x3) / dy2y3;

  // Calc side colors differential (from top to left bottom and to right bottom)

  dx_lc = c1 - c3;
  dx_rc = c2 - c3;

  if (math::FNotZero(dy1y3))
    dx_lc /= dy1y3 - 1;
  if (math::FNotZero(dy2y3))
    dx_rc /= dy2y3 - 1;
  
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

    // Compute differentials of colors on the edges

    FColor x_lc = c3 + (dx_lc * dy);
    FColor x_rc = c3 + (dx_rc * dy);

    // Compute x for left edge and right edges

    int xlb = floor(x_lhs);
    int xrb = ceil(x_rhs);

    // Compute color offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential of color between edges at the current y before
    // clipping left and right sides

    FColor dx_currx_c {};
    if ((xrb - xlb) != 0)
      dx_currx_c = (x_rc-x_lc)/(xrb-xlb);
    else
      dx_currx_c = c1;

    // Clip left and right lines of face
    
    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width() - 1, xrb);

    // Interpolate color for each pixel    

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;           // real, not clipped dx
      FColor curr_c {x_lc + (dx_currx_c * dx)};
      raster::Point(x, y, curr_c.GetARGB(), buf);
    }
  }
}

// Draws textured (affine) triangle without lighting

void raster_tri::TexturedAffine(
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    Bitmap* bmp, Buffer& buf)
{
  // Convert float to int for vertex positions

  int x1 = floor(p1.x);
  int x2 = floor(p2.x);
  int x3 = floor(p3.x);
  int y1 = floor(p1.y);
  int y2 = floor(p2.y);
  int y3 = floor(p3.y);
  
  // Unnormalize texture coords

  float u1 = t1.x * bmp->width();
  float u2 = t2.x * bmp->width();
  float u3 = t3.x * bmp->width();
  float v1 = t1.y * bmp->height();
  float v2 = t2.y * bmp->height();
  float v3 = t3.y * bmp->height();

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(u2, u3);
    std::swap(v2, v3);
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(u1, u2);
    std::swap(v1, v2);
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(u3, u2);
    std::swap(v3, v2);
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(u2, u3);
    std::swap(v2, v3);
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(u1, u2);
    std::swap(v1, v2);
  }

  // Part 1 : draw top part of triangle (from top to middle)
  // Note that 0;0 point is placed in left-bottom corner, but texture`s 0;0
  // is placed in left-top corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dx_lhs {0.0f};                          // dx - diff, lhs - left hand side
  float dx_rhs {0.0f};

  if (math::FNotZero(y2-y1))
    dx_lhs = (float)(x2-x1) / std::abs((float)(y2-y1));
  if (math::FNotZero(y3-y1))
    dx_rhs = (float)(x3-x1) / std::abs((float)(y3-y1));

  // Calc side textures differential (from top to left bottom and to right
  // bottom). Note that we need 4 differentials, for x and y from top to left
  // and for x and y from top to right

  float dx_lu = (u2 - u1) / std::abs((float)(y2-y1)); // dx - diff, l - left, u - x, v - y
  float dx_lv = (v2 - v1) / std::abs((float)(y2-y1));
  float dx_ru = (u3 - u1) / std::abs((float)(y3-y1));
  float dx_rv = (v3 - v1) / std::abs((float)(y3-y1));
  
  // Now choose, where really placed left and right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
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
    // Compute differentials of texture coords on the left and right edges

    int dy = y1 - y;                      // we need real dy, not clipped
    float x_lu = u1 + (dx_lu * dy);       // find tex_coords on the edges
    float x_lv = v1 + (dx_lv * dy);
    float x_ru = u1 + (dx_ru * dy);
    float x_rv = v1 + (dx_rv * dy);

    // Compute x for left edge and right edges

    int xlb = floor(x_lhs);          // xlb - x left border
    int xrb = ceil(x_rhs);
    
    // Compute texture offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential between edges of tex coords at the current y
    // before clipping left and right sides

    float dx_currx_u {};                   // find dx between left and right
    float dx_currx_v {};
    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
    }
    else
    {
      dx_currx_u = x_lu;
      dx_currx_v = x_lv;
    }

    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width() - 1, xrb);

    // Interpolate texture coordinate for each pixel

    float curr_u {};
    float curr_v {};
    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;
      curr_u = x_lu + (dx_currx_u * dx);
      curr_v = x_lv + (dx_currx_v * dx);
      byte r {0};
      byte g {0};
      byte b {0};
      bmp->get_pixel(ceil(curr_u), ceil(curr_v), r, g, b);
      
      raster::Point(x, y, color::MakeARGB(255, r, g, b), buf);
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

  // Calc side textures differential (from bottom to left top and to right
  // top). Note that we need 4 differentials, for x and y from top to left
  // and for x and y from top to right

  dx_lu = (u1 - u3) / std::abs(y1-y3);  // dx - diff, l - left, u - x, v - y
  dx_lv = (v1 - v3) / std::abs(y1-y3);  // -1 since we go -1 step less
  dx_ru = (u2 - u3) / std::abs(y2-y3);  // than diff in loop when we would
  dx_rv = (v2 - v3) / std::abs(y2-y3);  // draw triangle

  // Determine which is really left side step and really is right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
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

    // Compute differentials of texture coords on the left and right edges

    int dy = y - y3;                    // we need real dy, not clipped
    float x_lu = u3 + (dx_lu * dy);     // find tex_coords on the edges
    float x_lv = v3 + (dx_lv * dy);
    float x_ru = u3 + (dx_ru * dy);
    float x_rv = v3 + (dx_rv * dy);

    // Compute x for left edge and right edge

    int xlb = floor(x_lhs);          // xlb - x left border
    int xrb = ceil(x_rhs);

    // Compute texture offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential between edges of tex coords at the current y
    // before clipping left and right sides

    float dx_currx_u {};                   // find dx between left and right
    float dx_currx_v {};
    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
    }
    else
    {
      dx_currx_u = x_lu;
      dx_currx_v = x_lv;
    }

    // Clip left and right

    xlb = std::max(0, xlb);               // clip left and right lines
    xrb = std::min(buf.Width() - 1, xrb);
    
    // Interpolate texture coordinate for each pixel

    float curr_u {};
    float curr_v {};
    for (int x = xlb; x < xrb; ++x)        // for each pixel interpolate color
    {
      int dx = x - xlb + xl_dx;
      curr_u = x_lu + (dx_currx_u * dx);
      curr_v = x_lv + (dx_currx_v * dx);
      byte r {0};
      byte g {0};
      byte b {0};
      bmp->get_pixel(ceil(curr_u), ceil(curr_v), r, g, b);
      
      raster::Point(x, y, color::MakeARGB(255, r, g, b), buf);
    }
  }
}

// Draws textured triangle (affine) with flat shading 

void raster_tri::TexturedAffineFL(
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    uint color, Bitmap* bmp, Buffer& scr_buf)
{
  // Prepare fast screen buffer, texture and z-buffer access

  auto scr_ptr = scr_buf.GetPointer();
  int  scr_width = scr_buf.Width();
  int  scr_height = scr_buf.Height();
  auto tex_ptr = bmp->GetPointer();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();

  // Convert flat light color to Color<>

  Color<> light_color {color};

  // Convert float to int for vertex positions

  int x1 = math::Floor(p1.x);
  int x2 = math::Floor(p2.x);
  int x3 = math::Floor(p3.x);
  int y1 = math::Floor(p1.y);
  int y2 = math::Floor(p2.y);
  int y3 = math::Floor(p3.y);
  
  // Unnormalize texture coords

  float u1 = t1.x * bmp->width();
  float u2 = t2.x * bmp->width();
  float u3 = t3.x * bmp->width();
  float v1 = t1.y * bmp->height();
  float v2 = t2.y * bmp->height();
  float v3 = t3.y * bmp->height();

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(u2, u3);
    std::swap(v2, v3);
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(u1, u2);
    std::swap(v1, v2);
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(u3, u2);
    std::swap(v3, v2);
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(u2, u3);
    std::swap(v2, v3);
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(u1, u2);
    std::swap(v1, v2);
  }

  // Part 1 : draw top part of triangle (from top to middle)
  // Note that 0;0 point is placed in left-bottom corner, but texture`s 0;0
  // is placed in left-top corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dx_lhs {0.0f};                    // dx - diff, lhs - left hand side
  float dx_rhs {0.0f};

  float dy2y1 = std::abs(y2-y1);
  float dy3y1 = std::abs(y3-y1);

  if (math::FNotZero(dy2y1))
    dx_lhs = (float)(x2-x1) / dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rhs = (float)(x3-x1) / dy3y1;

  // Calc side textures differential (from top to left bottom and to right
  // bottom). Note that we need 4 differentials, for x and y from top to left
  // and for x and y from top to right.

  float dx_lu {u2 - u1};
  float dx_lv {v2 - v1};
  float dx_ru {u3 - u1};
  float dx_rv {v3 - v1};

  if (math::FNotZero(dy2y1)) {
    dx_lu /= dy2y1;
    dx_lv /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    dx_ru /= dy3y1;
    dx_rv /= dy3y1;
  }
  
  // Now choose, where really placed left and right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
  }

  // Now we should draw triangle from top to middle (y1-y2)

  float x_lhs {(float)x1};                // float curr x coord left
  float x_rhs {(float)x1};                // float curr x coord right

  // Clip top and bottom

  if (y1 < 0 || y3 >=scr_height)       // if triangle is full out of screen
    return;

  int y_top_clip = y1 - scr_height + 1; // how much pixels is clipped 
  y_top_clip = std::max(0, y_top_clip);   //  from the top of screen

  x_lhs += dx_lhs * y_top_clip;           // forward x left and x right curr
  x_rhs += dx_rhs * y_top_clip;           //  coords if y1 is out of screen

  int y_top = y1 - y_top_clip;            // define new drawable top
  int y_bot = std::max(0, y2);            //  and bottom

  // Draw top triangle

  Color<> tex_color {};                 // texture and its light color
  Color<> total_color {};               //  to interpolate it inside x row loop

  for (int y = y_top; y >= y_bot; --y)
  {
    // Compute differentials of texture coords on the left and right edges

    int dy = y1 - y;                      // we need real dy, not clipped
    float x_lu = u1 + (dx_lu * dy);       // find tex_coords on the edges
    float x_lv = v1 + (dx_lv * dy);
    float x_ru = u1 + (dx_ru * dy);
    float x_rv = v1 + (dx_rv * dy);

    // Compute x for left edge and right edges

    int xlb = math::Floor(x_lhs);          // xlb - x left border
    int xrb = math::Ceil(x_rhs);
    
    // Compute texture offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential between edges of tex coords at the current y
    // before clipping left and right sides

    float dx_currx_u {};              // find dx between left and right
    float dx_currx_v {};

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
    }

    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(scr_width - 1, xrb);

    // Interpolate texture coordinate for each pixel

    float curr_u {};
    float curr_v {};

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;
      curr_u = x_lu + (dx_currx_u * dx);
      curr_u = std::abs((int)(curr_u));
      curr_v = x_lv + (dx_currx_v * dx);
      curr_v = std::abs((int)(curr_v));

      // Get texture pixel. This is same as call to call
      // this function: bmp->get_pixel(u, v, r, g, b);

      int offset = (curr_v * tex_width) + (curr_u * tex_texel_width);
      tex_color.r_ = tex_ptr[offset + 2];
      tex_color.g_ = tex_ptr[offset + 1];
      tex_color.b_ = tex_ptr[offset + 0];

      // Modulate light and color
      
      Color<> total {light_color};
      total.Modulate(tex_color);

      // Draw texel

      scr_ptr[x + y * scr_width] = total.GetARGB();
    }
    
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
  }

  // Part 2 : draw bottom side of triangle (from bottom to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dy1y3 = std::abs(y1-y3);
  float dy2y3 = std::abs(y2-y3);

  if (math::FNotZero(dy1y3)) 
    dx_lhs = (float)(x1-x3) / dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rhs = (float)(x2-x3) / dy2y3;

  // Calc side textures differential (from bottom to left top and to right
  // top). Note that we need 4 differentials, for x and y from top to left
  // and for x and y from top to right

  dx_lu = u1 - u3;
  dx_lv = v1 - v3;
  dx_ru = u2 - u3;
  dx_rv = v2 - v3;

  if (math::FNotZero(dy1y3)) {
    dx_lu /= dy1y3;
    dx_lv /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    dx_ru /= dy2y3;
    dx_rv /= dy2y3;
  }

  // Determine which is really left side step and really is right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
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
  y_top = std::min(y2, scr_height-1);

  // Draw bottom triangle

  tex_color = Color<> {};               // texture and its light color
  total_color = Color<> {};             //  to interpolate it inside x row loop

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;

    // Compute differentials of texture coords on the left and right edges

    int dy = y - y3;                    // we need real dy, not clipped
    float x_lu = u3 + (dx_lu * dy);     // find tex_coords on the edges
    float x_lv = v3 + (dx_lv * dy);
    float x_ru = u3 + (dx_ru * dy);
    float x_rv = v3 + (dx_rv * dy);

    // Compute x for left edge and right edge

    int xlb = math::Floor(x_lhs);          // xlb - x left border
    int xrb = math::Ceil(x_rhs);

    // Compute texture offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential between edges of tex coords at the current y
    // before clipping left and right sides

    float dx_currx_u {};                // find dx between left and right
    float dx_currx_v {};

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
    }

    // Clip left and right

    xlb = std::max(0, xlb);               // clip left and right lines
    xrb = std::min(scr_width - 1, xrb);
    
    // Interpolate texture coordinate for each pixel

    float curr_u {};
    float curr_v {};

    for (int x = xlb; x < xrb; ++x)        // for each pixel interpolate color
    {
      int dx = x - xlb + xl_dx;
      curr_u = x_lu + (dx_currx_u * dx);
      curr_u = std::abs((int)(curr_u));   // int - fast floor
      curr_v = x_lv + (dx_currx_v * dx);
      curr_v = std::abs((int)(curr_v));

      // Get texture pixel. This is same as call to call
      // this function: bmp->get_pixel(u, v, r, g, b);

      int offset = (curr_v * tex_width) + (curr_u * tex_texel_width);
      tex_color.r_ = tex_ptr[offset + 2];
      tex_color.g_ = tex_ptr[offset + 1];
      tex_color.b_ = tex_ptr[offset + 0];

      // Modulate light and color
      
      Color<> total {light_color};
      total.Modulate(tex_color);

      // Draw texel

      scr_ptr[x + y * scr_width] = total.GetARGB();
    }
  }
}

// Draws textured (affine) triangle with gouraud shading

void raster_tri::TexturedAffineGR(
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    uint col1, uint col2, uint col3, Bitmap* bmp, Buffer& scr_buf)
{
  // Prepare fast screen buffer, texture and z-buffer access

  auto scr_ptr = scr_buf.GetPointer();
  int  scr_width = scr_buf.Width();
  int  scr_height = scr_buf.Height();
  auto tex_ptr = bmp->GetPointer();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();

  // Convert float to int for vertex positions

  int x1 = math::Floor(p1.x);
  int x2 = math::Floor(p2.x);
  int x3 = math::Floor(p3.x);
  int y1 = math::Floor(p1.y);
  int y2 = math::Floor(p2.y);
  int y3 = math::Floor(p3.y);
  
  // Unnormalize texture coordinates

  float u1 = t1.x * bmp->width();
  float u2 = t2.x * bmp->width();
  float u3 = t3.x * bmp->width();
  float v1 = t1.y * bmp->height();
  float v2 = t2.y * bmp->height();
  float v3 = t3.y * bmp->height();

  // Prepare colors  

  FColor c1 {col1};
  FColor c2 {col2};
  FColor c3 {col3};

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(u2, u3);
    std::swap(v2, v3);
    std::swap(c2, c3);    
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(c1, c2);    
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(c1, c2);    
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(u3, u2);
    std::swap(v3, v2);
    std::swap(c3, c2);    
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(u2, u3);
    std::swap(v2, v3);
    std::swap(c2, c3);    
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(c1, c2);    
  }

  // Part 1 : draw top part of triangle (from top to middle)
  // Note that 0;0 point is placed in left-bottom corner, but texture`s 0;0
  // is placed in left-top corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dx_lhs {0.0f};                          // dx - diff, lhs - left hand side
  float dx_rhs {0.0f};
  
  float dy2y1 = std::abs(y2-y1);
  float dy3y1 = std::abs(y3-y1);

  if (math::FNotZero(dy2y1))
    dx_lhs = (float)(x2-x1) / dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rhs = (float)(x3-x1) / dy3y1;

  // Calc side textures differential (from top to left bottom and to right
  // bottom). Note that we need 4 differentials, for x and y from top to left
  // and for x and y from top to right.
  // Since we want perspective correct texture, we calc u/z and v/z coordinates

  float dx_lu {u2 - u1};
  float dx_lv {v2 - v1};
  float dx_ru {u3 - u1};
  float dx_rv {v3 - v1};

  if (math::FNotZero(dy2y1)) {
    dx_lu /= dy2y1;
    dx_lv /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    dx_ru /= dy3y1;
    dx_rv /= dy3y1;
  }
  
  // Calc side colors differential (from top to left bottom and to right bottom)

  FColor dx_lc {c2 - c1};
  FColor dx_rc {c3 - c1};
  if (math::FNotZero(dy2y1))
    dx_lc /= std::abs(y2-y1);
  if (math::FNotZero(dy3y1))
    dx_rc /= std::abs(y3-y1);

  // Now choose, where really placed left and right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
    std::swap(dx_lc, dx_rc);    
  }

  // Now we should draw triangle from top to middle (from y1 to y2)

  float x_lhs {(float)x1};                // float curr x coord left
  float x_rhs {(float)x1};                // float curr x coord right

  // Clip top and bottom

  if (y1 < 0 || y3 >= scr_height)       // if triangle is full out of screen
    return;

  int y_top_clip = y1 - scr_height + 1; // how much pixels is clipped 
  y_top_clip = std::max(0, y_top_clip);   //  from the top of screen

  x_lhs += dx_lhs * y_top_clip;           // forward x left and x right curr
  x_rhs += dx_rhs * y_top_clip;           //  coords if y1 is out of screen

  int y_top = y1 - y_top_clip;            // define new drawable top
  int y_bot = std::max(0, y2);            //  and bottom

  // Draw top triangle

  FColor tex_color {};                 // texture and its light color
  FColor total_color {};               //  to interpolate it inside x row loop

  for (int y = y_top; y >= y_bot; --y)
  {
    // Compute differentials of texture coords on the left and right edges

    int dy = y1 - y;                        // we need real dy, not clipped
    
    float x_lu = u1 + (dx_lu * dy);  // find tex_coords on the edges
    float x_lv = v1 + (dx_lv * dy);
    float x_ru = u1 + (dx_ru * dy);
    float x_rv = v1 + (dx_rv * dy);

    // Compute differentials of colors on the left and right edges
    
    FColor x_lc {c1 + (dx_lc * dy)};
    FColor x_rc {c1 + (dx_rc * dy)};

    // Compute x for left edge and right edges

    int xlb = math::Floor(x_lhs);          // xlb - x left border
    int xrb = math::Ceil(x_rhs);
    
    // Compute texture offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential between edges of tex coords at the current y
    // before clipping left and right sides

    float dx_currx_u {};                   // find dx between left and right
    float dx_currx_v {};
    FColor dx_currx_c {c1};    

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
      dx_currx_c = (x_rc - x_lc) / (xrb - xlb);      
    }

    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(scr_width - 1, xrb);

    // Interpolate texture coordinate for each pixel

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;           // we need real dx, not clipped
      FColor curr_c = x_lc + (dx_currx_c * dx);      
    
      // Get real texture coordinates

      float curr_u = x_lu + (dx_currx_u * dx);
      float curr_v = x_lv + (dx_currx_v * dx);
      int u = std::abs((int)(curr_u));
      int v = std::abs((int)(curr_v));
    
      // Get texture pixel. This is same as call to call
      // this function: bmp->get_pixel(u, v, r, g, b);

      int offset = (v * tex_width) + (u * tex_texel_width);
      tex_color.r_ = tex_ptr[offset + 2];
      tex_color.g_ = tex_ptr[offset + 1];
      tex_color.b_ = tex_ptr[offset + 0];
    
      // Modulate light and color

      FColor total {curr_c};
      total.Modulate(tex_color);

      // Draw point

      scr_ptr[x + y * scr_width] = total.GetARGB();
    }
    
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;
  }

  // Part 2 : draw bottom side of triangle (from bottom to middle)
  // Note that 0;0 point is placed in left-bottom corner

  // Define step of left and right side (if perpendicular, then step = 0)
  // Here we just suppose where left and right side

  float dy1y3 = std::abs(y1-y3);
  float dy2y3 = std::abs(y2-y3);

  if (math::FNotZero(dy1y3)) 
    dx_lhs = (float)(x1-x3) / dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rhs = (float)(x2-x3) / dy2y3;

  // Calc side textures differential (from bottom to left top and to right
  // top). Note that we need 4 differentials, for x and y from top to left
  // and for x and y from top to right
  // Since we want perspective correct texture, we calc u/z and v/z coordinates
  
  dx_lu = u1 - u3;
  dx_lv = v1 - v3;
  dx_ru = u2 - u3;
  dx_rv = v2 - v3;

  if (math::FNotZero(dy1y3)) {
    dx_lu /= dy1y3;
    dx_lv /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    dx_ru /= dy2y3;
    dx_rv /= dy2y3;
  }

  // Calc side colors differential (from top to left bottom and to right bottom)

  dx_lc = c1 - c3;
  dx_rc = c2 - c3;
  if (math::FNotZero(dy1y3))
    dx_lc /= dy1y3 - 1;           // -1 since we go -1 step less than diff
  if (math::FNotZero(dy2y3))
    dx_rc /= dy2y3 - 1;           // in loop when we would draw triangle

  // Determine which is really left side step and really is right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
    std::swap(dx_lc, dx_rc);    
  }
    
  // Now we should draw traingle from bottom to middle (from y3 to y2)

  x_lhs = (float)x3;
  x_rhs = (float)x3;
  
  // Clip top and bottom

  int y_bot_clip {0};                   // here we calc how mush pixels
  if (y3+1 < 0)                         //  is out of screen from bottom
    y_bot_clip = std::abs(y3+1);
  
  x_lhs += dx_lhs * y_bot_clip;         // expand left and right curr coords
  x_rhs += dx_rhs * y_bot_clip;

  y_bot = std::max(0, y3+1);            // new drawable top and bottom
  y_top = std::min(y2, scr_height-1);

  // Draw bottom triangle

  tex_color = FColor {};               // texture and its light color
  total_color = FColor {};             //  to interpolate it inside x row loop

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;

    // Compute differentials of texture coords on the left and right edges

    int dy = y - y3;                        // we need real dy, not clipped

    float x_lu = u3 + (dx_lu * dy);  // find tex_coords on the edges
    float x_lv = v3 + (dx_lv * dy);
    float x_ru = u3 + (dx_ru * dy);
    float x_rv = v3 + (dx_rv * dy);

    // Compute differentials of colors on the left and right edges
    
    FColor x_lc = c3 + (dx_lc * dy);    // find colors on the edges
    FColor x_rc = c3 + (dx_rc * dy);

    // Compute x for left edge and right edge

    int xlb = math::Floor(x_lhs);          // xlb - x left border
    int xrb = math::Ceil(x_rhs);

    // Compute texture offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential between edges of tex coords at the current y
    // before clipping left and right sides

    float dx_currx_u {};                   // find dx between left and right
    float dx_currx_v {};
    FColor dx_currx_c {c3};    

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
      dx_currx_c = (x_rc - x_lc) / (xrb - xlb);      
    }

    // Clip left and right

    xlb = std::max(0, xlb);               // clip left and right lines
    xrb = std::min(scr_width - 1, xrb);
    
    // Interpolate texture coordinate for each pixel
    
    for (int x = xlb; x < xrb; ++x)        // for each pixel interpolate color
    {
      int dx = x - xlb + xl_dx;
      FColor curr_c = x_lc + (dx_currx_c * dx);      

      // Get real texture coordinates
      
      float curr_u = x_lu + (dx_currx_u * dx);
      float curr_v = x_lv + (dx_currx_v * dx);
      int u = std::abs((int)(curr_u));   // (int) - fast floor
      int v = std::abs((int)(curr_v));

      // Get texture pixel. This is same as call to call
      // this function: bmp->get_pixel(u, v, r, g, b);

      int offset = (v * tex_width) + (u * tex_texel_width);
      tex_color.r_ = tex_ptr[offset + 2];
      tex_color.g_ = tex_ptr[offset + 1];
      tex_color.b_ = tex_ptr[offset + 0];
    
      // Modulate light and color

      FColor total {curr_c};
      total.Modulate(tex_color);
      
      // Draw point

      scr_ptr[x + y * scr_width] = total.GetARGB();
    }
  }
}

// Draws solid triangle and returns numbers of drawn pixels:
//  - flat shading
//  - 1/z buffer
//  - 50% fast alpha blending

int raster_tri::SolidFL(
    Vertex v1, Vertex v2, Vertex v3,
    cFColor& color, ZBuffer& zbuffer, Buffer& sbuffer)
{
  int total_drawn {};
  
  // Prepare fast buffers access

  int sbuf_w = sbuffer.Width();
  int sbuf_h = sbuffer.Height();
  auto* s_buf = sbuffer.GetPointer();
  auto* z_buf = zbuffer.GetPointer();

  // Prepare order of vertices from top to bottom 

  raster_helpers::SortVertices(v1, v2, v3);
  v1.pos_.z = 1.0f / v1.pos_.z;
  v2.pos_.z = 1.0f / v2.pos_.z;
  v3.pos_.z = 1.0f / v3.pos_.z;

  // Prepare alpha blending of current color

  uint curr_color {};
  if (color.a_ < 1.0f)
    curr_color = (color * color.a_).GetARGB();
  else
    curr_color = color.GetARGB();

  // Part 1 : draw top part of triangle (using top left filling convention)

  int iy1 = ceil(v1.pos_.y);
  int iy2 = ceil(v2.pos_.y) + 1;          // fill convention
  int iy3 = ceil(v3.pos_.y) + 1;          // fill convention

  if (iy1 < 0 || iy3 >= sbuf_h)                 // full out of screen
    return total_drawn;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy2y1 = iy1 - (iy2 - 1);                  // similar to iy1 - ceil(y2)
  int dy3y1 = iy1 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  float lx_step {v2.pos_.x - v1.pos_.x};
  float rx_step {v3.pos_.x - v1.pos_.x};
  float lz_step {v2.pos_.z - v1.pos_.z};
  float rz_step {v3.pos_.z - v1.pos_.z};

  if (math::FNotZero(dy2y1)) {
    lx_step /= dy2y1;
    lz_step /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    rx_step /= dy3y1;
    rz_step /= dy3y1;
  }

  // Swap interpolants if our suppose was wrong

  if (lx_step > rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
  }
  else if (lx_step == rx_step)
    return total_drawn;

  // Prepare vars for x and 1/z on the borders of triangle

  float x_lhs {v1.pos_.x};
  float z_lhs {v1.pos_.z};
  float x_rhs {x_lhs};
  float z_rhs {z_lhs};

  // Clip top triangle

  int y_top_clip = iy1 - (sbuf_h - 1); 
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;

  // Draw top triangle (note that 0-0 is in left bottom corner of screen)

  int y_top = iy1 - y_top_clip;
  int y_bot = std::max(0, iy2);

  int xlb {};
  int xrb {};
  int dx_curr {};

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    xlb = ceil(x_lhs);
    xrb = ceil(x_rhs);           // fill convention
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    if ((xrb - xlb) != 0)
      z_step = (z_rhs - z_lhs) / dx_curr;    
    float z_curr = z_lhs + (z_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (color.a_ < 1.0f)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};
          color::ShiftRight(buf_color, 1);
          s_buf[idx] = curr_color + buf_color.GetARGB();
          z_buf[idx] = z_curr;
        }
        ++idx;
        z_curr += z_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          s_buf[idx] = curr_color;
          z_buf[idx] = z_curr;
        }
        ++idx;
        z_curr += z_step;        
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
  }

  // Part 2 : draw bottom part of triangle (0-0 is the left bottom)

  // Down middle pixel as it was drawn

  iy2 = iy2 - 1;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy1y3 = iy1 - (iy3 - 1);
  int dy2y3 = iy2 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  lx_step = v3.pos_.x - v1.pos_.x;
  rx_step = v3.pos_.x - v2.pos_.x;
  lz_step = v3.pos_.z - v1.pos_.z;
  rz_step = v3.pos_.z - v2.pos_.z;

  if (math::FNotZero(dy1y3)) {
    lz_step /= dy1y3;
    lx_step /= dy1y3;
  }

  if (math::FNotZero(dy2y3)) {
    rx_step /= dy2y3;
    rz_step /= dy2y3;
  }

  // Now make new x2 point and new interpolants for triangle borders

  float new_x {};
  float new_z {};
  int dy_passed {iy1 - iy2};  // using new iy2 value

  if (lx_step > rx_step) {
    new_x = v1.pos_.x + (dy_passed * lx_step);
    new_z = v1.pos_.z + (dy_passed * lz_step);
    x_lhs = new_x;
    x_rhs = v2.pos_.x;
    z_lhs = new_z;
    z_rhs = v2.pos_.z;
  }
  else if (lx_step < rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    new_x = v1.pos_.x + (dy_passed * rx_step);
    new_z = v1.pos_.z + (dy_passed * rz_step);    
    x_lhs = v2.pos_.x;
    x_rhs = new_x;
    z_lhs = v2.pos_.z;
    z_rhs = new_z;
  }

  // Clip bottom triangle (4 cases)

  y_top_clip = iy2 - (sbuf_h - 1);
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;


  z_rhs += rz_step * y_top_clip;  // Draw bottom triangle

  y_top = iy2 - y_top_clip;
  y_bot = std::max(0, iy3);   // if iy3 out of screen - we are not start


  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    xlb = ceil(x_lhs);
    xrb = ceil(x_rhs);           // fill convention
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant from left side with clipping

    float z_step {};
    if ((xrb - xlb) != 0)
      z_step = (z_rhs - z_lhs) / dx_curr;
    float z_curr = z_lhs + (z_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (color.a_ < 1.0f)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};
          color::ShiftRight(buf_color, 1);
          s_buf[idx] = curr_color + buf_color.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          s_buf[idx] = curr_color;
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;        
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
  }
  return total_drawn;  
}

// Draws solid triangle and returns numbers of drawn pixels:
//  - gouraud shading
//  - 1/z buffer
//  - 50% fast alpha blending

int raster_tri::SolidGR(
    Vertex v1, Vertex v2, Vertex v3,
    ZBuffer& zbuffer, Buffer& sbuffer)
{
  int total_drawn {};

  // Prepare fast buffers access

  int sbuf_w = sbuffer.Width();
  int sbuf_h = sbuffer.Height();
  auto* s_buf = sbuffer.GetPointer();
  auto* z_buf = zbuffer.GetPointer();

  // Prepare order of vertices from top to bottom 

  raster_helpers::SortVertices(v1, v2, v3);
  v1.pos_.z = 1.0f / v1.pos_.z;
  v2.pos_.z = 1.0f / v2.pos_.z;
  v3.pos_.z = 1.0f / v3.pos_.z;

  // Prepare color and alpha blending of current color

  FColor c1 {v1.color_};    // todo: we can don`t copy this colors
  FColor c2 {v2.color_};
  FColor c3 {v3.color_};

  if (c1.a_ < 1.0f) {
    c1 *= c1.a_;                  // we don`t support gradient alpha
    c2 *= c1.a_;
    c3 *= c1.a_;
  }

  // Part 1 : draw top part of triangle (using top left filling convention)

  int iy1 = ceil(v1.pos_.y);
  int iy2 = ceil(v2.pos_.y) + 1;         // fill convention
  int iy3 = ceil(v3.pos_.y) + 1;         // fill convention

  if (iy1 < 0 || iy3 >= sbuf_h)               // full out of screen
    return total_drawn;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy2y1 = iy1 - (iy2 - 1);
  int dy3y1 = iy1 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  float lx_step {v2.pos_.x - v1.pos_.x};
  float rx_step {v3.pos_.x - v1.pos_.x};
  float lz_step {v2.pos_.z - v1.pos_.z};
  float rz_step {v3.pos_.z - v1.pos_.z};
  FColor lc_step {c2 - c1};
  FColor rc_step {c3 - c1};

  if (math::FNotZero(dy2y1)) {
    lx_step /= dy2y1;
    lz_step /= dy2y1;
    lc_step /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    rx_step /= dy3y1;
    rz_step /= dy3y1;
    rc_step /= dy3y1;
  }

  // Swap interpolants if our suppose was wrong

  if (lx_step > rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lc_step, rc_step);
  }
  else if (lx_step == rx_step)
    return total_drawn;

  // Prepare vars for x, 1/z and vxs colors on the borders of triangle

  float x_lhs {v1.pos_.x};
  float x_rhs {x_lhs};
  float z_lhs {v1.pos_.z};
  float z_rhs {z_lhs};
  FColor c_rhs {c1};
  FColor c_lhs {c1};

  // Clip top triangle

  int y_top_clip = iy1 - (sbuf_h - 1); 
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  c_lhs += lc_step * y_top_clip;
  c_rhs += rc_step * y_top_clip;

  // Draw top triangle (note that 0-0 is in left bottom corner of screen)

  int y_top = iy1 - y_top_clip;
  int y_bot = std::max(0, iy2);

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    int xlb = ceil(x_lhs);
    int xrb = ceil(x_rhs);           // fill convention
    int dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    FColor c_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      c_step = (c_rhs - c_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    FColor c_curr = c_lhs + (c_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (c1.a_ < 1.0f)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};
          color::ShiftRight(buf_color, 1);
          s_buf[idx] = c_curr.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;          
        }
        ++idx;
        z_curr += z_step;
        c_curr += c_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          s_buf[idx] = c_curr.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        c_curr += c_step;
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    c_lhs += lc_step;
    c_rhs += rc_step;
  }

  // Part 2 : draw bottom part of triangle (0-0 is the left bottom)

  // Down middle pixel as it was drawn

  iy2 = iy2 - 1;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy1y3 = iy1 - (iy3 - 1);
  int dy2y3 = iy2 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  lx_step = v3.pos_.x - v1.pos_.x;
  rx_step = v3.pos_.x - v2.pos_.x;
  lz_step = v3.pos_.z - v1.pos_.z;
  rz_step = v3.pos_.z - v2.pos_.z;
  lc_step = c3 - c1;
  rc_step = c3 - c2;

  if (math::FNotZero(dy1y3)) {
    lz_step /= dy1y3;
    lx_step /= dy1y3;
    lc_step /= dy1y3;
  }

  if (math::FNotZero(dy2y3)) {
    rx_step /= dy2y3;
    rz_step /= dy2y3;
    rc_step /= dy2y3;
  }

  // Now make new x2 point and new interpolants for triangle borders

  float new_x {};
  float new_z {};
  FColor new_c {};
  int dy_passed {iy1 - iy2};

  if (lx_step > rx_step) {
    new_x = v1.pos_.x + (dy_passed * lx_step);
    new_z = v1.pos_.z + (dy_passed * lz_step);
    new_c = c1 + (lc_step * dy_passed);
    x_lhs = new_x;
    x_rhs = v2.pos_.x;
    z_lhs = new_z;
    z_rhs = v2.pos_.z;
    c_lhs = new_c;
    c_rhs = c2;
  }
  else if (lx_step < rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lc_step, rc_step);
    new_x = v1.pos_.x + (dy_passed * rx_step);
    new_z = v1.pos_.z + (dy_passed * rz_step);
    new_c = c1 + (rc_step * dy_passed);    
    x_lhs = v2.pos_.x;
    x_rhs = new_x;
    z_lhs = v2.pos_.z;
    z_rhs = new_z;
    c_lhs = c2;
    c_rhs = new_c;
  }

  // Clip bottom triangle

  y_top_clip = iy2 - (sbuf_h - 1);
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  c_lhs += lc_step * y_top_clip;
  c_rhs += rc_step * y_top_clip;

  // Draw bottom triangle

  y_top = iy2 - y_top_clip;
  y_bot = std::max(0, iy3);

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    int xlb = ceil(x_lhs);
    int xrb = ceil(x_rhs);           // fill convention
    int dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant from left side with clipping

    float z_step {};
    FColor c_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;
      c_step = (c_rhs - c_lhs) / dx_curr;
    }    
    float z_curr = z_lhs + (z_step * xl_dx);
    FColor c_curr = c_lhs + (c_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (c1.a_ < 1.0f)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};
          color::ShiftRight(buf_color, 1);
          s_buf[idx] = c_curr.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        c_curr += c_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          s_buf[idx] = c_curr.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;          
        }
        ++idx;
        z_curr += z_step;
        c_curr += c_step;
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    c_lhs += lc_step;
    c_rhs += rc_step;
  }
  return total_drawn;
}

// Draws textured (perspective correct) triangle and returns numbers of drawn
// pixels:
//  - const shading (without lighting)
//  - 1/z buffer
//  - 50% fast alpha blending

int raster_tri::TexturedPerspective(
    Vertex v1, Vertex v2, Vertex v3,
    Bitmap* bmp, ZBuffer& zbuf, Buffer& sbuf)
{
  int total_drawn {};

  // Cull imposible triangles

  if (v1.pos_.x == v2.pos_.x && v1.pos_.y == v2.pos_.y)
    return total_drawn;
  if (v1.pos_.x == v3.pos_.x && v1.pos_.y == v3.pos_.y)
    return total_drawn;  
  if (v2.pos_.x == v3.pos_.x && v2.pos_.y == v3.pos_.y)
    return total_drawn;

  // Prepare fast buffers access

  int sbuf_w = sbuf.Width();
  int sbuf_h = sbuf.Height();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* s_buf = sbuf.GetPointer();
  auto* z_buf = zbuf.GetPointer();
  auto* tex_ptr = bmp->GetPointer();

  // Prepare order of vertices from top to bottom 

  raster_helpers::SortVertices(v1, v2, v3);
  raster_helpers::UnnormalizeTexture(v1, v2, v3, bmp->width(), bmp->height());

  // Prepare light color and alpha blending

  bool alpha {false};
  if (v1.color_.a_ < 1.0f)
    alpha = true;

  // Part 1 : draw top part of triangle (using top left filling convention)

  int iy1 = ceil(v1.pos_.y);
  int iy2 = ceil(v2.pos_.y) + 1;         // fill convention
  int iy3 = ceil(v3.pos_.y) + 1;         // fill convention

  if (iy1 < 0 || iy3 >= sbuf_h)               // full out of screen
    return total_drawn;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy2y1 = iy1 - (iy2 - 1);                // similar to iy1 - ceil(y2)
  int dy3y1 = iy1 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  float lx_step {v2.pos_.x - v1.pos_.x};
  float rx_step {v3.pos_.x - v1.pos_.x};
  float lz_step {1.0f / v2.pos_.z - 1.0f / v1.pos_.z};
  float rz_step {1.0f / v3.pos_.z - 1.0f / v1.pos_.z};
  float lu_step {v2.texture_.x / v2.pos_.z - v1.texture_.x / v1.pos_.z};
  float lv_step {v2.texture_.y / v2.pos_.z - v1.texture_.y / v1.pos_.z};
  float ru_step {v3.texture_.x / v3.pos_.z - v1.texture_.x / v1.pos_.z};
  float rv_step {v3.texture_.y / v3.pos_.z - v1.texture_.y / v1.pos_.z};

  if (math::FNotZero(dy2y1)) {
    lx_step /= dy2y1;
    lz_step /= dy2y1;
    lu_step /= dy2y1;
    lv_step /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    rx_step /= dy3y1;
    rz_step /= dy3y1;
    ru_step /= dy3y1;
    rv_step /= dy3y1;
  }

  // Swap interpolants if our suppose was wrong

  if (lx_step > rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
  }
  else if (lx_step == rx_step)
    return total_drawn;

  // Prepare vars for x, 1/z and tex coords on the borders of triangle

  float x_lhs {v1.pos_.x};
  float x_rhs {x_lhs};
  float z_lhs {1.0f / v1.pos_.z};
  float z_rhs {z_lhs};
  float u_lhs {v1.texture_.x / v1.pos_.z};
  float u_rhs {u_lhs};
  float v_lhs {v1.texture_.y / v1.pos_.z};
  float v_rhs {v_lhs};

  // Clip top triangle

  int y_top_clip = iy1 - (sbuf_h - 1); 
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;

  // Draw top triangle (note that 0-0 is in left bottom corner of screen)
  
  int y_top = iy1 - y_top_clip;
  int y_bot = std::max(0, iy2);

  int xlb {};
  int xrb {};
  int dx_curr {};

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);            // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);                // guarantee no gaps   
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};       // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};                 // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;  // real tex coords
          int v = v_curr / z_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);
          
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];

          color::ShiftRight(tex_color, 1);

          s_buf[idx] = tex_color.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> tex_color {};                 // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;  // real tex coords
          int v = v_curr / z_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];

          s_buf[idx] = tex_color.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
  }

  // Part 2 : draw bottom part of triangle (0-0 is the left bottom)

  // Down middle pixel as it was drawn

  iy2 = iy2 - 1;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy1y3 = iy1 - (iy3 - 1);
  int dy2y3 = iy2 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  lx_step = v3.pos_.x - v1.pos_.x;
  rx_step = v3.pos_.x - v2.pos_.x;
  lz_step = 1.0f / v3.pos_.z - 1.0f / v1.pos_.z;
  rz_step = 1.0f / v3.pos_.z - 1.0f / v2.pos_.z;
  lu_step = v3.texture_.x / v3.pos_.z - v1.texture_.x / v1.pos_.z;
  lv_step = v3.texture_.y / v3.pos_.z - v1.texture_.y / v1.pos_.z;
  ru_step = v3.texture_.x / v3.pos_.z - v2.texture_.x / v2.pos_.z;
  rv_step = v3.texture_.y / v3.pos_.z - v2.texture_.y / v2.pos_.z;

  if (math::FNotZero(dy1y3)) {
    lx_step /= dy1y3;
    lz_step /= dy1y3;
    lu_step /= dy1y3;
    lv_step /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    rx_step /= dy2y3;
    rz_step /= dy2y3;
    ru_step /= dy2y3;
    rv_step /= dy2y3;
  }

  // Now make new x2 point and new interpolants for triangle borders

  float new_x {};
  float new_z {};
  float new_u {};
  float new_v {};
  int dy_passed {iy1 - iy2};

  if (lx_step > rx_step) {
    new_x = v1.pos_.x + (dy_passed * lx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * lz_step);
    new_u = (v1.texture_.x / v1.pos_.z) + (lu_step * dy_passed);
    new_v = (v1.texture_.y / v1.pos_.z) + (lv_step * dy_passed);
    x_lhs = new_x;
    x_rhs = v2.pos_.x;
    z_lhs = new_z;
    z_rhs = 1.0f / v2.pos_.z;
    u_lhs = new_u;
    u_rhs = v2.texture_.x / v2.pos_.z;
    v_lhs = new_v;
    v_rhs = v2.texture_.y / v2.pos_.z;
  }
  else if (lx_step < rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    new_x = v1.pos_.x + (dy_passed * rx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * rz_step);
    new_u = (v1.texture_.x / v1.pos_.z) + (ru_step * dy_passed);
    new_v = (v1.texture_.y / v1.pos_.z) + (rv_step * dy_passed);
    x_lhs = v2.pos_.x;
    x_rhs = new_x;
    z_lhs = 1.0f / v2.pos_.z;
    z_rhs = new_z;
    u_lhs = v2.texture_.x / v2.pos_.z;
    u_rhs = new_u;
    v_lhs = v2.texture_.y / v2.pos_.z;
    v_rhs = new_v;
  }

  // Clip bottom triangle

  y_top_clip = iy2 - (sbuf_h - 1);
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;

  // Draw bottom triangle (note that 0-0 is in left bottom corner of screen)

  y_top = iy2 - y_top_clip;
  y_bot = std::max(0, iy3);
  
  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);                 // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);                 // guarantee no gaps
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};         // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};                   // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;    // real tex coords
          int v = v_curr / z_curr; 
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          color::ShiftRight(tex_color, 1);

          s_buf[idx] = tex_color.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> tex_color {};                   // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;    // real tex coords
          int v = v_curr / z_curr; 
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          s_buf[idx] = tex_color.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
  }

  return total_drawn;
}

// Draws textured (perspective correct) triangle and returns numbers of drawn
// pixels:
//  - flat shading
//  - 1/z buffer
//  - 50% fast alpha blending

int raster_tri::TexturedPerspectiveFL(
    Vertex v1, Vertex v2, Vertex v3,
    cFColor& fcolor, Bitmap* bmp, ZBuffer& zbuf, Buffer& sbuf)
{
  int total_drawn {};

  // Cull impossible triangles

  if (v1.pos_.x == v2.pos_.x && v1.pos_.y == v2.pos_.y)
    return total_drawn;
  if (v1.pos_.x == v3.pos_.x && v1.pos_.y == v3.pos_.y)
    return total_drawn;
  if (v2.pos_.x == v3.pos_.x && v2.pos_.y == v3.pos_.y)
    return total_drawn;

  // Prepare fast buffers access

  int sbuf_w = sbuf.Width();
  int sbuf_h = sbuf.Height();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* s_buf = sbuf.GetPointer();
  auto* z_buf = zbuf.GetPointer();
  auto* tex_ptr = bmp->GetPointer();

  // Prepare order of vertices from top to bottom 

  raster_helpers::SortVertices(v1, v2, v3);
  raster_helpers::UnnormalizeTexture(v1, v2, v3, bmp->width(), bmp->height());

  // Prepare light color and alpha blending

  bool alpha {false};
  if (v1.color_.a_ < 1.0f)
    alpha = true;
  uint light_color {fcolor.GetARGB()};

  // Part 1 : draw top part of triangle (using top left filling convention)

  int iy1 = ceil(v1.pos_.y);
  int iy2 = ceil(v2.pos_.y) + 1;         // fill convention
  int iy3 = ceil(v3.pos_.y) + 1;         // fill convention

  if (iy1 < 0 || iy3 >= sbuf_h)               // full out of screen
    return 0;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy2y1 = iy1 - (iy2 - 1);
  int dy3y1 = iy1 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  float lx_step {v2.pos_.x - v1.pos_.x};
  float rx_step {v3.pos_.x - v1.pos_.x};
  float lz_step {1.0f / v2.pos_.z - 1.0f / v1.pos_.z};
  float rz_step {1.0f / v3.pos_.z - 1.0f / v1.pos_.z};
  float lu_step {v2.texture_.x / v2.pos_.z - v1.texture_.x / v1.pos_.z};
  float lv_step {v2.texture_.y / v2.pos_.z - v1.texture_.y / v1.pos_.z};
  float ru_step {v3.texture_.x / v3.pos_.z - v1.texture_.x / v1.pos_.z};
  float rv_step {v3.texture_.y / v3.pos_.z - v1.texture_.y / v1.pos_.z};

  if (math::FNotZero(dy2y1)) {
    lx_step /= dy2y1;
    lz_step /= dy2y1;
    lu_step /= dy2y1;
    lv_step /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    rx_step /= dy3y1;
    rz_step /= dy3y1;
    ru_step /= dy3y1;
    rv_step /= dy3y1;
  }

  // Swap interpolants if our suppose was wrong

  if (lx_step > rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
  }
  else if (lx_step == rx_step)
    return 0;

  // Prepare vars for x, 1/z and tex coords on the borders of triangle

  float x_lhs {v1.pos_.x};
  float x_rhs {x_lhs};
  float z_lhs {1.0f / v1.pos_.z};
  float z_rhs {z_lhs};
  float u_lhs {v1.texture_.x / v1.pos_.z};
  float u_rhs {u_lhs};
  float v_lhs {v1.texture_.y / v1.pos_.z};
  float v_rhs {v_lhs};

  // Clip top triangle

  int y_top_clip = iy1 - (sbuf_h - 1); 
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;

  // Draw top triangle (note that 0-0 is in left bottom corner of screen)

  int y_top = iy1 - y_top_clip;
  int y_bot = std::max(0, iy2);

  int xlb {};
  int xrb {};
  int dx_curr {};  

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);            // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);                // guarantee no gaps   
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};     // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;            // real tex coords
          int v = v_curr / z_curr; 
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {light_color};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;

        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;            // real tex coords
          int v = v_curr / z_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {light_color};
          total.Modulate(tex_color);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;

        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
  }

  // Part 2 : draw bottom part of triangle (0-0 is the left bottom)

  // Down middle pixel as it was drawn

  iy2 = iy2 - 1;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy1y3 = iy1 - (iy3 - 1);
  int dy2y3 = iy2 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  lx_step = v3.pos_.x - v1.pos_.x;
  rx_step = v3.pos_.x - v2.pos_.x;
  lz_step = 1.0f / v3.pos_.z - 1.0f / v1.pos_.z;
  rz_step = 1.0f / v3.pos_.z - 1.0f / v2.pos_.z;
  lu_step = v3.texture_.x / v3.pos_.z - v1.texture_.x / v1.pos_.z;
  lv_step = v3.texture_.y / v3.pos_.z - v1.texture_.y / v1.pos_.z;
  ru_step = v3.texture_.x / v3.pos_.z - v2.texture_.x / v2.pos_.z;
  rv_step = v3.texture_.y / v3.pos_.z - v2.texture_.y / v2.pos_.z;

  if (math::FNotZero(dy1y3)) {
    lx_step /= dy1y3;
    lz_step /= dy1y3;
    lu_step /= dy1y3;
    lv_step /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    rx_step /= dy2y3;
    rz_step /= dy2y3;
    ru_step /= dy2y3;
    rv_step /= dy2y3;
  }

  // Now make new x2 point and new interpolants for triangle borders

  float new_x {};
  float new_z {};
  float new_u {};
  float new_v {};
  int dy_passed {iy1 - iy2};

  if (lx_step > rx_step) {
    new_x = v1.pos_.x + (dy_passed * lx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * lz_step);
    new_u = (v1.texture_.x / v1.pos_.z) + (lu_step * dy_passed);
    new_v = (v1.texture_.y / v1.pos_.z) + (lv_step * dy_passed);
    x_lhs = new_x;
    x_rhs = v2.pos_.x;
    z_lhs = new_z;
    z_rhs = 1.0f / v2.pos_.z;
    u_lhs = new_u;
    u_rhs = v2.texture_.x / v2.pos_.z;
    v_lhs = new_v;
    v_rhs = v2.texture_.y / v2.pos_.z;
  }
  else if (lx_step < rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    new_x = v1.pos_.x + (dy_passed * rx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * rz_step);
    new_u = (v1.texture_.x / v1.pos_.z) + (ru_step * dy_passed);
    new_v = (v1.texture_.y / v1.pos_.z) + (rv_step * dy_passed);
    x_lhs = v2.pos_.x;
    x_rhs = new_x;
    z_lhs = 1.0f / v2.pos_.z;
    z_rhs = new_z;
    u_lhs = v2.texture_.x / v2.pos_.z;
    u_rhs = new_u;
    v_lhs = v2.texture_.y / v2.pos_.z;
    v_rhs = new_v;
  }

  // Clip bottom triangle

  y_top_clip = iy2 - (sbuf_h - 1);
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;

  // Draw bottom triangle (note that 0-0 is in left bottom corner of screen)

  y_top = std::min(iy2, sbuf_h-1);
  y_bot = std::max(0, iy3);
  
  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);            // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);                // guarantee no gaps
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / ((x_rhs + 1) - xlb);    
      u_step = (u_rhs - u_lhs) / ((x_rhs + 1) - xlb);
      v_step = (v_rhs - v_lhs) / ((x_rhs + 1) - xlb);
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};         // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};                   // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;    // real tex coords
          int v = v_curr / z_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {light_color};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;

        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> tex_color {};       // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;    // real tex coords
          int v = v_curr / z_curr; 
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {light_color};
          total.Modulate(tex_color);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;

        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
  }
  return total_drawn;
}

// Draws textured (perspective correct) triangle and returns numbers of drawn
// pixels:
//  - flat shading
//  - 1/z buffer
//  - 50% fast alpha blending
//  - billinear texture filtering

int raster_tri::TexturedPerspectiveFLBF(
    Vertex v1, Vertex v2, Vertex v3,
    cFColor& fcolor, Bitmap* bmp, ZBuffer& zbuf, Buffer& sbuf)
{
  int total_drawn {};

  // Cull impossible triangles

  if (v1.pos_.x == v2.pos_.x && v1.pos_.y == v2.pos_.y)
    return total_drawn;
  if (v1.pos_.x == v3.pos_.x && v1.pos_.y == v3.pos_.y)
    return total_drawn;
  if (v2.pos_.x == v3.pos_.x && v2.pos_.y == v3.pos_.y)
    return total_drawn;

  // Prepare fast buffers access

  int sbuf_w = sbuf.Width();
  int sbuf_h = sbuf.Height();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* s_buf = sbuf.GetPointer();
  auto* z_buf = zbuf.GetPointer();
  auto* tex_ptr = bmp->GetPointer();
  int tex_w = bmp->width();
  int tex_h = bmp->height();

  // Prepare order of vertices from top to bottom 

  raster_helpers::SortVertices(v1, v2, v3);
  raster_helpers::UnnormalizeTexture(v1, v2, v3, bmp->width(), bmp->height());

  // Prepare light color and alpha blending

  bool alpha {false};
  if (v1.color_.a_ < 1.0f)
    alpha = true;
  uint light_color {fcolor.GetARGB()};

  // Part 1 : draw top part of triangle (using top left filling convention)

  int iy1 = ceil(v1.pos_.y);
  int iy2 = ceil(v2.pos_.y) + 1;         // fill convention
  int iy3 = ceil(v3.pos_.y) + 1;         // fill convention

  if (iy1 < 0 || iy3 >= sbuf_h)               // full out of screen
    return 0;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy2y1 = iy1 - (iy2 - 1);
  int dy3y1 = iy1 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  float lx_step {v2.pos_.x - v1.pos_.x};
  float rx_step {v3.pos_.x - v1.pos_.x};
  float lz_step {1.0f / v2.pos_.z - 1.0f / v1.pos_.z};
  float rz_step {1.0f / v3.pos_.z - 1.0f / v1.pos_.z};
  float lu_step {v2.texture_.x / v2.pos_.z - v1.texture_.x / v1.pos_.z};
  float lv_step {v2.texture_.y / v2.pos_.z - v1.texture_.y / v1.pos_.z};
  float ru_step {v3.texture_.x / v3.pos_.z - v1.texture_.x / v1.pos_.z};
  float rv_step {v3.texture_.y / v3.pos_.z - v1.texture_.y / v1.pos_.z};

  if (math::FNotZero(dy2y1)) {
    lx_step /= dy2y1;
    lz_step /= dy2y1;
    lu_step /= dy2y1;
    lv_step /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    rx_step /= dy3y1;
    rz_step /= dy3y1;
    ru_step /= dy3y1;
    rv_step /= dy3y1;
  }

  // Swap interpolants if our suppose was wrong

  if (lx_step > rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
  }
  else if (lx_step == rx_step)
    return 0;

  // Prepare vars for x, 1/z and tex coords on the borders of triangle

  float x_lhs {v1.pos_.x};
  float x_rhs {x_lhs};
  float z_lhs {1.0f / v1.pos_.z};
  float z_rhs {z_lhs};
  float u_lhs {v1.texture_.x / v1.pos_.z};
  float u_rhs {u_lhs};
  float v_lhs {v1.texture_.y / v1.pos_.z};
  float v_rhs {v_lhs};

  // Clip top triangle

  int y_top_clip = iy1 - (sbuf_h - 1); 
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;

  // Draw top triangle (note that 0-0 is in left bottom corner of screen)

  int y_top = iy1 - y_top_clip;
  int y_bot = std::max(0, iy2);

  int xlb {};
  int xrb {};
  int dx_curr {};  

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);            // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);                // guarantee no gaps   
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};     // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;            // real tex coords
          int v = v_curr / z_curr; 
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {light_color};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;

        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          // Get real u and v tex coords and diff from rounding for bifiltering

          float free_u = u_curr / z_curr;
          float free_v = v_curr / z_curr;
          int u = free_u;
          int v = free_v;
          float du = free_u - u;
          float dv = free_v - v;

          // Offsets to left-top, left-right, left-bottom and right-bottom texels

          int offset_lt = (v * tex_width) + (u * tex_texel_width);
          int offset_rt = offset_lt + tex_texel_width;
          int offset_lb = offset_lt + tex_width;
          int offset_rb = offset_lb + tex_texel_width;

          FColor tex_lt {};
          FColor tex_rt {};
          FColor tex_lb {};
          FColor tex_rb {};

          tex_lt.r_ = tex_ptr[offset_lt + 2];       // get origin texel
          tex_lt.g_ = tex_ptr[offset_lt + 1];
          tex_lt.b_ = tex_ptr[offset_lt + 0];

          if (u < tex_w-1)                          // get right top texel
          {
            tex_rt.r_ = tex_ptr[offset_rt + 2];
            tex_rt.g_ = tex_ptr[offset_rt + 1];
            tex_rt.b_ = tex_ptr[offset_rt + 0];
          }
          if (v < tex_h-1)                          // get left bottom texel
          {
            tex_lb.r_ = tex_ptr[offset_lb + 2];
            tex_lb.g_ = tex_ptr[offset_lb + 1];
            tex_lb.b_ = tex_ptr[offset_lb + 0];
          }
          if (v < tex_h-1 && u < tex_w-1)           // get left bottom texel
          {
            tex_rb.r_ = tex_ptr[offset_rb + 2];
            tex_rb.g_ = tex_ptr[offset_rb + 1];
            tex_rb.b_ = tex_ptr[offset_rb + 0];
          }
        
          // Mix neighboring texels to get average texel

          FColor tex_total {};
          tex_total += tex_lt*(1.0f-du)*(1.0f-dv);
          tex_total += tex_rt*(du)*(1.0f-dv);
          tex_total += tex_lb*(1.0f-du)*(dv);
          tex_total += tex_rb*(du)*(dv);
        
          if (v >= tex_h-1 || u >= tex_w-1)         // special case rb texel
            tex_total = tex_lt;

          FColor total {light_color};
          total.Modulate(tex_total);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
  }

  // Part 2 : draw bottom part of triangle (0-0 is the left bottom)

  // Down middle pixel as it was drawn

  iy2 = iy2 - 1;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy1y3 = iy1 - (iy3 - 1);
  int dy2y3 = iy2 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  lx_step = v3.pos_.x - v1.pos_.x;
  rx_step = v3.pos_.x - v2.pos_.x;
  lz_step = 1.0f / v3.pos_.z - 1.0f / v1.pos_.z;
  rz_step = 1.0f / v3.pos_.z - 1.0f / v2.pos_.z;
  lu_step = v3.texture_.x / v3.pos_.z - v1.texture_.x / v1.pos_.z;
  lv_step = v3.texture_.y / v3.pos_.z - v1.texture_.y / v1.pos_.z;
  ru_step = v3.texture_.x / v3.pos_.z - v2.texture_.x / v2.pos_.z;
  rv_step = v3.texture_.y / v3.pos_.z - v2.texture_.y / v2.pos_.z;

  if (math::FNotZero(dy1y3)) {
    lx_step /= dy1y3;
    lz_step /= dy1y3;
    lu_step /= dy1y3;
    lv_step /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    rx_step /= dy2y3;
    rz_step /= dy2y3;
    ru_step /= dy2y3;
    rv_step /= dy2y3;
  }

  // Now make new x2 point and new interpolants for triangle borders

  float new_x {};
  float new_z {};
  float new_u {};
  float new_v {};
  int dy_passed {iy1 - iy2};

  if (lx_step > rx_step) {
    new_x = v1.pos_.x + (dy_passed * lx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * lz_step);
    new_u = (v1.texture_.x / v1.pos_.z) + (lu_step * dy_passed);
    new_v = (v1.texture_.y / v1.pos_.z) + (lv_step * dy_passed);
    x_lhs = new_x;
    x_rhs = v2.pos_.x;
    z_lhs = new_z;
    z_rhs = 1.0f / v2.pos_.z;
    u_lhs = new_u;
    u_rhs = v2.texture_.x / v2.pos_.z;
    v_lhs = new_v;
    v_rhs = v2.texture_.y / v2.pos_.z;
  }
  else if (lx_step < rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    new_x = v1.pos_.x + (dy_passed * rx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * rz_step);
    new_u = (v1.texture_.x / v1.pos_.z) + (ru_step * dy_passed);
    new_v = (v1.texture_.y / v1.pos_.z) + (rv_step * dy_passed);
    x_lhs = v2.pos_.x;
    x_rhs = new_x;
    z_lhs = 1.0f / v2.pos_.z;
    z_rhs = new_z;
    u_lhs = v2.texture_.x / v2.pos_.z;
    u_rhs = new_u;
    v_lhs = v2.texture_.y / v2.pos_.z;
    v_rhs = new_v;
  }

  // Clip bottom triangle

  y_top_clip = iy2 - (sbuf_h - 1);
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;

  // Draw bottom triangle (note that 0-0 is in left bottom corner of screen)

  y_top = std::min(iy2, sbuf_h-1);
  y_bot = std::max(0, iy3);
  
  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);            // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);                // guarantee no gaps
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / ((x_rhs + 1) - xlb);    
      u_step = (u_rhs - u_lhs) / ((x_rhs + 1) - xlb);
      v_step = (v_rhs - v_lhs) / ((x_rhs + 1) - xlb);
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};         // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};                   // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;    // real tex coords
          int v = v_curr / z_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {light_color};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;

        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          // Get real u and v tex coords and diff from rounding for bifiltering

          float free_u = u_curr / z_curr;
          float free_v = v_curr / z_curr;
          int u = free_u;
          int v = free_v;
          float du = free_u - u;
          float dv = free_v - v;

          // Offsets to left-top, left-right, left-bottom and right-bottom texels

          int offset_lt = (v * tex_width) + (u * tex_texel_width);
          int offset_rt = offset_lt + tex_texel_width;
          int offset_lb = offset_lt + tex_width;
          int offset_rb = offset_lb + tex_texel_width;

          FColor tex_lt {};
          FColor tex_rt {};
          FColor tex_lb {};
          FColor tex_rb {};

          tex_lt.r_ = tex_ptr[offset_lt + 2];       // get origin texel
          tex_lt.g_ = tex_ptr[offset_lt + 1];
          tex_lt.b_ = tex_ptr[offset_lt + 0];

          if (u < tex_w-1)                          // get right top texel
          {
            tex_rt.r_ = tex_ptr[offset_rt + 2];
            tex_rt.g_ = tex_ptr[offset_rt + 1];
            tex_rt.b_ = tex_ptr[offset_rt + 0];
          }
          if (v < tex_h-1)                          // get left bottom texel
          {
            tex_lb.r_ = tex_ptr[offset_lb + 2];
            tex_lb.g_ = tex_ptr[offset_lb + 1];
            tex_lb.b_ = tex_ptr[offset_lb + 0];
          }
          if (v < tex_h-1 && u < tex_w-1)           // get left bottom texel
          {
            tex_rb.r_ = tex_ptr[offset_rb + 2];
            tex_rb.g_ = tex_ptr[offset_rb + 1];
            tex_rb.b_ = tex_ptr[offset_rb + 0];
          }
        
          // Mix neighboring texels to get average texel

          FColor tex_total {};
          tex_total += tex_lt*(1.0f-du)*(1.0f-dv);
          tex_total += tex_rt*(du)*(1.0f-dv);
          tex_total += tex_lb*(1.0f-du)*(dv);
          tex_total += tex_rb*(du)*(dv);
        
          if (v >= tex_h-1 || u >= tex_w-1)         // special case rb texel
            tex_total = tex_lt;

          FColor total {light_color};
          total.Modulate(tex_total);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
  }
  return total_drawn;
}

// Draws textured (perspective correct) triangle and returns numbers of drawn
// pixels:
//  - gouraud shading
//  - 1/z buffer
//  - 50% fast alpha blending

int raster_tri::TexturedPerspectiveGR(
    Vertex v1, Vertex v2, Vertex v3,
    Bitmap* bmp, ZBuffer& zbuf, Buffer& sbuf)
{
  int total_drawn {};
  
  // Cull impossible triangles

  if (v1.pos_.x == v2.pos_.x && v1.pos_.y == v2.pos_.y)
    return total_drawn;
  if (v1.pos_.x == v3.pos_.x && v1.pos_.y == v3.pos_.y)
    return total_drawn;
  if (v2.pos_.x == v3.pos_.x && v2.pos_.y == v3.pos_.y)
    return total_drawn;

  // Prepare fast buffers access

  int sbuf_w = sbuf.Width();
  int sbuf_h = sbuf.Height();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* s_buf = sbuf.GetPointer();
  auto* z_buf = zbuf.GetPointer();
  auto* tex_ptr = bmp->GetPointer();

 // Prepare order of vertices from top to bottom 

  raster_helpers::SortVertices(v1, v2, v3);
  raster_helpers::UnnormalizeTexture(v1, v2, v3, bmp->width(), bmp->height());

  // Prepare color and alpha blending of current color

  FColor c1 {v1.color_};          // todo: we can don`t copy this colors
  FColor c2 {v2.color_};
  FColor c3 {v3.color_};

  bool alpha {false};
  if (c1.a_ < 1.0f) {
    c1 *= c1.a_;                  // we don`t support gradient alpha
    c2 *= c1.a_;
    c3 *= c1.a_;
    alpha = true;
  }

  // Part 1 : draw top part of triangle (using top left filling convention)

  int iy1 = ceil(v1.pos_.y);
  int iy2 = ceil(v2.pos_.y) + 1;         // fill convention
  int iy3 = ceil(v3.pos_.y) + 1;         // fill convention

  if (iy1 < 0 || iy3 >= sbuf_h)               // full out of screen
    return total_drawn;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy2y1 = iy1 - (iy2 - 1);
  int dy3y1 = iy1 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  float lx_step {v2.pos_.x - v1.pos_.x};
  float rx_step {v3.pos_.x - v1.pos_.x};
  float lz_step {1.0f / v2.pos_.z - 1.0f / v1.pos_.z};
  float rz_step {1.0f / v3.pos_.z - 1.0f / v1.pos_.z};
  float lu_step {v2.texture_.x / v2.pos_.z - v1.texture_.x / v1.pos_.z};
  float lv_step {v2.texture_.y / v2.pos_.z - v1.texture_.y / v1.pos_.z};
  float ru_step {v3.texture_.x / v3.pos_.z - v1.texture_.x / v1.pos_.z};
  float rv_step {v3.texture_.y / v3.pos_.z - v1.texture_.y / v1.pos_.z};
  FColor lc_step {c2 - c1};
  FColor rc_step {c3 - c1};

  if (math::FNotZero(dy2y1)) {
    lx_step /= dy2y1;
    lz_step /= dy2y1;
    lu_step /= dy2y1;
    lv_step /= dy2y1;
    lc_step /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    rx_step /= dy3y1;
    rz_step /= dy3y1;
    ru_step /= dy3y1;
    rv_step /= dy3y1;
    rc_step /= dy3y1;
  }

  // Swap interpolants if our suppose was wrong

  if (lx_step > rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    std::swap(lc_step, rc_step);
  }
  else if (lx_step == rx_step)
    return total_drawn;

  // Prepare vars for x, 1/z, tex coords and color on the borders of triangle

  float x_lhs {v1.pos_.x};
  float x_rhs {x_lhs};
  float z_lhs {1.0f / v1.pos_.z};
  float z_rhs {z_lhs};
  float u_lhs {v1.texture_.x / v1.pos_.z};
  float u_rhs {u_lhs};
  float v_lhs {v1.texture_.y / v1.pos_.z};
  float v_rhs {v_lhs};
  FColor c_rhs {c1};
  FColor c_lhs {c1};

  // Clip top triangle

  int y_top_clip = iy1 - (sbuf_h - 1); 
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;
  c_lhs += lc_step * y_top_clip;
  c_rhs += rc_step * y_top_clip;

  // Draw top triangle (note that 0-0 is in left bottom corner of screen)

  int y_top = iy1 - y_top_clip;
  int y_bot = std::max(0, iy2);

  int xlb {};
  int xrb {};
  int dx_curr {};  

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);            // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);            // guarantee no gaps   
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    FColor c_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
      c_step = (c_rhs - c_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
    FColor c_curr = c_lhs + (c_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};       // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};                 // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;  // get real tex coords
          int v = v_curr / z_curr; 
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> tex_color {};                 // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;  // real tex coords
          int v = v_curr / z_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
    c_lhs += lc_step;
    c_rhs += rc_step;
  }

  // Part 2 : draw bottom part of triangle (0-0 is the left bottom)

  // Down middle pixel as it was drawn

  iy2 = iy2 - 1;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy1y3 = iy1 - (iy3 - 1);
  int dy2y3 = iy2 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  lx_step = v3.pos_.x - v1.pos_.x;
  rx_step = v3.pos_.x - v2.pos_.x;
  lz_step = 1.0f / v3.pos_.z - 1.0f / v1.pos_.z;
  rz_step = 1.0f / v3.pos_.z - 1.0f / v2.pos_.z;
  lu_step = v3.texture_.x / v3.pos_.z - v1.texture_.x / v1.pos_.z;
  lv_step = v3.texture_.y / v3.pos_.z - v1.texture_.y / v1.pos_.z;
  ru_step = v3.texture_.x / v3.pos_.z - v2.texture_.x / v2.pos_.z;
  rv_step = v3.texture_.y / v3.pos_.z - v2.texture_.y / v2.pos_.z;
  lc_step = c3 - c1;
  rc_step = c3 - c2;

  if (math::FNotZero(dy1y3)) {
    lx_step /= dy1y3;
    lz_step /= dy1y3;
    lu_step /= dy1y3;
    lv_step /= dy1y3;
    lc_step /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    rx_step /= dy2y3;
    rz_step /= dy2y3;
    ru_step /= dy2y3;
    rv_step /= dy2y3;
    rc_step /= dy2y3;
  }

  // Now make new x2 point and new interpolants for triangle borders

  float new_x {};
  float new_z {};
  float new_u {};
  float new_v {};
  FColor new_c {};
  int dy_passed {iy1 - iy2};

  if (lx_step > rx_step) {
    new_x = v1.pos_.x + (dy_passed * lx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * lz_step);
    new_u = (v1.texture_.x / v1.pos_.z) + (lu_step * dy_passed);
    new_v = (v1.texture_.y / v1.pos_.z) + (lv_step * dy_passed);
    new_c = c1 + (lc_step * dy_passed);
    x_lhs = new_x;
    x_rhs = v2.pos_.x;
    z_lhs = new_z;
    z_rhs = 1.0f / v2.pos_.z;
    u_lhs = new_u;
    u_rhs = v2.texture_.x / v2.pos_.z;
    v_lhs = new_v;
    v_rhs = v2.texture_.y / v2.pos_.z;
    c_lhs = new_c;
    c_rhs = c2;
  }
  else if (lx_step < rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    std::swap(lc_step, rc_step);
    new_x = v1.pos_.x + (dy_passed * rx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * rz_step);
    new_u = (v1.texture_.x / v1.pos_.z) + (ru_step * dy_passed);
    new_v = (v1.texture_.y / v1.pos_.z) + (rv_step * dy_passed);
    new_c = c1 + (rc_step * dy_passed);    
    x_lhs = v2.pos_.x;
    x_rhs = new_x;
    z_lhs = 1.0f / v2.pos_.z;
    z_rhs = new_z;
    u_lhs = v2.texture_.x / v2.pos_.z;
    u_rhs = new_u;
    v_lhs = v2.texture_.y / v2.pos_.z;
    v_rhs = new_v;
    c_lhs = c2;
    c_rhs = new_c;
  }

  // Clip bottom triangle

  y_top_clip = iy2 - (sbuf_h - 1);
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;
  c_lhs += lc_step * y_top_clip;
  c_rhs += rc_step * y_top_clip;

  // Draw bottom triangle (note that 0-0 is in left bottom corner of screen)

  y_top = iy2 - y_top_clip;
  y_bot = std::max(0, iy3);
  
  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);            // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);                // guarantee no gaps   
    }
    dx_curr = xrb - xlb;
    
    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    FColor c_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
      c_step = (c_rhs - c_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
    FColor c_curr = c_lhs + (c_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};       // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};                 // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;
          int v = v_curr / z_curr; 
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr / z_curr;            // real tex coords
          int v = v_curr / z_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
    c_lhs += lc_step;
    c_rhs += rc_step;
  }

  return total_drawn;
}


// Draws textured (affine) triangle and returns numbers of drawn pixels:
//  - gouraud shading
//  - 1/z buffer
//  - 50% fast alpha blending

int raster_tri::TexturedAffineGR(
    Vertex v1, Vertex v2, Vertex v3,
    Bitmap* bmp, ZBuffer& zbuf, Buffer& sbuf)
{
  int total_drawn {};

  // Cull imposible triangles

  if (v1.pos_.x == v2.pos_.x && v1.pos_.y == v2.pos_.y)
    return total_drawn;
  if (v1.pos_.x == v3.pos_.x && v1.pos_.y == v3.pos_.y)
    return total_drawn;  
  if (v2.pos_.x == v3.pos_.x && v2.pos_.y == v3.pos_.y)
    return total_drawn;

  // Prepare fast buffers access

  int sbuf_w = sbuf.Width();
  int sbuf_h = sbuf.Height();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* s_buf = sbuf.GetPointer();
  auto* z_buf = zbuf.GetPointer();
  auto* tex_ptr = bmp->GetPointer();

  // Prepare order of vertices from top to bottom 

  raster_helpers::SortVertices(v1, v2, v3);
  raster_helpers::UnnormalizeTexture(v1, v2, v3, bmp->width(), bmp->height());

  // Prepare color and alpha blending of current color

  FColor c1 {v1.color_};    // todo: we can don`t copy this colors
  FColor c2 {v2.color_};
  FColor c3 {v3.color_};

  bool alpha {false};
  if (c1.a_ < 1.0f) {
    c1 *= c1.a_;                  // we don`t support gradient alpha
    c2 *= c1.a_;
    c3 *= c1.a_;
    alpha = true;
  }

  // Part 1 : draw top part of triangle (using top left filling convention)

  int iy1 = ceil(v1.pos_.y);
  int iy2 = ceil(v2.pos_.y) + 1;        // fill convention
  int iy3 = ceil(v3.pos_.y) + 1;        // fill convention

  if (iy1 < 0 || iy3 >= sbuf_h)               // full out of screen
    return total_drawn;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy2y1 = iy1 - (iy2 - 1);                // similar to iy1 - ceil(y2)
  int dy3y1 = iy1 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  float lx_step {v2.pos_.x - v1.pos_.x};
  float rx_step {v3.pos_.x - v1.pos_.x};
  float lz_step {1.0f / v2.pos_.z - 1.0f / v1.pos_.z};
  float rz_step {1.0f / v3.pos_.z - 1.0f / v1.pos_.z};
  float lu_step {v2.texture_.x - v1.texture_.x};
  float lv_step {v2.texture_.y - v1.texture_.y};
  float ru_step {v3.texture_.x - v1.texture_.x};
  float rv_step {v3.texture_.y - v1.texture_.y};
  FColor lc_step {c2 - c1};
  FColor rc_step {c3 - c1};

  if (math::FNotZero(dy2y1)) {
    lx_step /= dy2y1;
    lz_step /= dy2y1;
    lu_step /= dy2y1;
    lv_step /= dy2y1;
    lc_step /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    rx_step /= dy3y1;
    rz_step /= dy3y1;
    ru_step /= dy3y1;
    rv_step /= dy3y1;
    rc_step /= dy3y1;
  }

  // Swap interpolants if our suppose was wrong

  if (lx_step > rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    std::swap(lc_step, rc_step);
  }
  else if (lx_step == rx_step)
    return total_drawn;

  // Prepare vars for x, 1/z, tex coords and color on the borders of triangle

  float x_lhs {v1.pos_.x};
  float x_rhs {x_lhs};
  float z_lhs {1.0f / v1.pos_.z};
  float z_rhs {z_lhs};
  float u_lhs {v1.texture_.x};
  float u_rhs {u_lhs};
  float v_lhs {v1.texture_.y};
  float v_rhs {v_lhs};
  FColor c_rhs {c1};
  FColor c_lhs {c1};

  // Clip top triangle

  int y_top_clip = iy1 - (sbuf_h - 1); 
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;
  c_lhs += lc_step * y_top_clip;
  c_rhs += rc_step * y_top_clip;

  // Draw top triangle (note that 0-0 is in left bottom corner of screen)

  int y_top = iy1 - y_top_clip;
  int y_bot = std::max(0, iy2);

  int xlb {};
  int xrb {};
  int dx_curr {};

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);               // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);              // guarantee no gaps 
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    FColor c_step {};
    if ((xrb - xlb) != 0)
    {
      z_step = (z_rhs - z_lhs) / dx_curr;
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
      c_step = (c_rhs - c_lhs) / dx_curr;
    }
      
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
    FColor c_curr = c_lhs + (c_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};     // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr;
          int v = v_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr;
          int v = v_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
    c_lhs += lc_step;
    c_rhs += rc_step;
  }

  // Part 2 : draw bottom part of triangle (0-0 is the left bottom)

  // Down middle pixel as it was drawn

  iy2 = iy2 - 1;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy1y3 = iy1 - (iy3 - 1);
  int dy2y3 = iy2 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  lx_step = v3.pos_.x - v1.pos_.x;
  rx_step = v3.pos_.x - v2.pos_.x;
  lz_step = 1.0f / v3.pos_.z - 1.0f / v1.pos_.z;
  rz_step = 1.0f / v3.pos_.z - 1.0f / v2.pos_.z;
  lu_step = v3.texture_.x - v1.texture_.x;
  lv_step = v3.texture_.y - v1.texture_.y;
  ru_step = v3.texture_.x - v2.texture_.x;
  rv_step = v3.texture_.y - v2.texture_.y;
  lc_step = c3 - c1;
  rc_step = c3 - c2;

  if (math::FNotZero(dy1y3)) {
    lx_step /= dy1y3;
    lz_step /= dy1y3;
    lu_step /= dy1y3;
    lv_step /= dy1y3;
    lc_step /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    rx_step /= dy2y3;
    rz_step /= dy2y3;
    ru_step /= dy2y3;
    rv_step /= dy2y3;
    rc_step /= dy2y3;
  }

  // Now make new x2 point and new interpolants for triangle borders

  float new_x {};
  float new_z {};
  float new_u {};
  float new_v {};
  FColor new_c {};
  int dy_passed {iy1 - iy2};

  if (lx_step > rx_step) {
    new_x = v1.pos_.x + (dy_passed * lx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * lz_step);
    new_u = (v1.texture_.x) + (lu_step * dy_passed);
    new_v = (v1.texture_.y) + (lv_step * dy_passed);
    new_c = c1 + (lc_step * dy_passed);
    x_lhs = new_x;
    x_rhs = v2.pos_.x;
    z_lhs = new_z;
    z_rhs = 1.0f / v2.pos_.z;
    u_lhs = new_u;
    u_rhs = v2.texture_.x;
    v_lhs = new_v;
    v_rhs = v2.texture_.y;
    c_lhs = new_c;
    c_rhs = c2;
  }
  else if (lx_step < rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    std::swap(lc_step, rc_step);
    new_x = v1.pos_.x + (dy_passed * rx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * rz_step);
    new_u = (v1.texture_.x) + (ru_step * dy_passed);
    new_v = (v1.texture_.y) + (rv_step * dy_passed);
    new_c = c1 + (rc_step * dy_passed);    
    x_lhs = v2.pos_.x;
    x_rhs = new_x;
    z_lhs = 1.0f / v2.pos_.z;
    z_rhs = new_z;
    u_lhs = v2.texture_.x;
    u_rhs = new_u;
    v_lhs = v2.texture_.y;
    v_rhs = new_v;
    c_lhs = c2;
    c_rhs = new_c;
  }

  // Clip bottom triangle

  y_top_clip = iy2 - (sbuf_h - 1);
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;
  c_lhs += lc_step * y_top_clip;
  c_rhs += rc_step * y_top_clip;

  // Draw bottom triangle (note that 0-0 is in left bottom corner of screen)

  y_top = iy2 - y_top_clip;
  y_bot = std::max(0, iy3);
  
  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);          // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);              // guarantee no gaps
    }
    dx_curr = xrb - xlb;
  
    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    FColor c_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
      c_step = (c_rhs - c_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
    FColor c_curr = c_lhs + (c_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};     // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr;
          int v = v_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr;
          int v = v_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step; 
    c_lhs += lc_step;
    c_rhs += rc_step;
  }

  return total_drawn;
}

// Draws textured (affine) triangle and returns numbers of drawn pixels:
//  - gouraud shading
//  - 1/z buffer
//  - 50% fast alpha blending
//  - billinear texture filtering

int raster_tri::TexturedAffineGRBF(
    Vertex v1, Vertex v2, Vertex v3,
    Bitmap* bmp, ZBuffer& zbuf, Buffer& sbuf)
{
  int total_drawn {};

  // Cull imposible triangles

  if (v1.pos_.x == v2.pos_.x && v1.pos_.y == v2.pos_.y)
    return total_drawn;
  if (v1.pos_.x == v3.pos_.x && v1.pos_.y == v3.pos_.y)
    return total_drawn;  
  if (v2.pos_.x == v3.pos_.x && v2.pos_.y == v3.pos_.y)
    return total_drawn;

  // Prepare fast buffers access

  int sbuf_w = sbuf.Width();
  int sbuf_h = sbuf.Height();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* s_buf = sbuf.GetPointer();
  auto* z_buf = zbuf.GetPointer();
  auto* tex_ptr = bmp->GetPointer();
  int tex_w = bmp->width();
  int tex_h = bmp->height();

  // Prepare order of vertices from top to bottom 

  raster_helpers::SortVertices(v1, v2, v3);
  raster_helpers::UnnormalizeTexture(v1, v2, v3, bmp->width(), bmp->height());

  // Prepare color and alpha blending of current color

  FColor c1 {v1.color_};    // todo: we can don`t copy this colors
  FColor c2 {v2.color_};
  FColor c3 {v3.color_};

  bool alpha {false};
  if (c1.a_ < 1.0f) {
    c1 *= c1.a_;                  // we don`t support gradient alpha
    c2 *= c1.a_;
    c3 *= c1.a_;
    alpha = true;
  }

  // Part 1 : draw top part of triangle (using top left filling convention)

  int iy1 = ceil(v1.pos_.y);
  int iy2 = ceil(v2.pos_.y) + 1;        // fill convention
  int iy3 = ceil(v3.pos_.y) + 1;        // fill convention

  if (iy1 < 0 || iy3 >= sbuf_h)               // full out of screen
    return total_drawn;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy2y1 = iy1 - (iy2 - 1);                // similar to iy1 - ceil(y2)
  int dy3y1 = iy1 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  float lx_step {v2.pos_.x - v1.pos_.x};
  float rx_step {v3.pos_.x - v1.pos_.x};
  float lz_step {1.0f / v2.pos_.z - 1.0f / v1.pos_.z};
  float rz_step {1.0f / v3.pos_.z - 1.0f / v1.pos_.z};
  float lu_step {v2.texture_.x - v1.texture_.x};
  float lv_step {v2.texture_.y - v1.texture_.y};
  float ru_step {v3.texture_.x - v1.texture_.x};
  float rv_step {v3.texture_.y - v1.texture_.y};
  FColor lc_step {c2 - c1};
  FColor rc_step {c3 - c1};

  if (math::FNotZero(dy2y1)) {
    lx_step /= dy2y1;
    lz_step /= dy2y1;
    lu_step /= dy2y1;
    lv_step /= dy2y1;
    lc_step /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    rx_step /= dy3y1;
    rz_step /= dy3y1;
    ru_step /= dy3y1;
    rv_step /= dy3y1;
    rc_step /= dy3y1;
  }

  // Swap interpolants if our suppose was wrong

  if (lx_step > rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    std::swap(lc_step, rc_step);
  }
  else if (lx_step == rx_step)
    return total_drawn;

  // Prepare vars for x, 1/z, tex coords and color on the borders of triangle

  float x_lhs {v1.pos_.x};
  float x_rhs {x_lhs};
  float z_lhs {1.0f / v1.pos_.z};
  float z_rhs {z_lhs};
  float u_lhs {v1.texture_.x};
  float u_rhs {u_lhs};
  float v_lhs {v1.texture_.y};
  float v_rhs {v_lhs};
  FColor c_rhs {c1};
  FColor c_lhs {c1};

  // Clip top triangle

  int y_top_clip = iy1 - (sbuf_h - 1); 
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;
  c_lhs += lc_step * y_top_clip;
  c_rhs += rc_step * y_top_clip;

  // Draw top triangle (note that 0-0 is in left bottom corner of screen)

  int y_top = iy1 - y_top_clip;
  int y_bot = std::max(0, iy2);

  int xlb {};
  int xrb {};
  int dx_curr {};

  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);               // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);              // guarantee no gaps 
    }
    dx_curr = xrb - xlb;

    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z, u && v interpolants and clip them from left side

    float z_step {};
    float u_step {};
    float v_step {};
    FColor c_step {};
    if ((xrb - xlb) != 0)
    {
      z_step = (z_rhs - z_lhs) / dx_curr;
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
      c_step = (c_rhs - c_lhs) / dx_curr;
    }
      
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
    FColor c_curr = c_lhs + (c_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};     // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr;
          int v = v_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);

          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          // Get real u and v tex coords and diff from rounding for bifiltering

          int u = u_curr;
          int v = v_curr;
          float du = u_curr - u;
          float dv = v_curr - v;

          // Offsets to left-top, left-right, left-bottom and right-bottom texels

          int offset_lt = (v * tex_width) + (u * tex_texel_width);
          int offset_rt = offset_lt + tex_texel_width;
          int offset_lb = offset_lt + tex_width;
          int offset_rb = offset_lb + tex_texel_width;

          FColor tex_lt {};
          FColor tex_rt {};
          FColor tex_lb {};
          FColor tex_rb {};

          tex_lt.r_ = tex_ptr[offset_lt + 2];       // get origin texel
          tex_lt.g_ = tex_ptr[offset_lt + 1];
          tex_lt.b_ = tex_ptr[offset_lt + 0];

          if (u < tex_w-1)                          // get right top texel
          {
            tex_rt.r_ = tex_ptr[offset_rt + 2];
            tex_rt.g_ = tex_ptr[offset_rt + 1];
            tex_rt.b_ = tex_ptr[offset_rt + 0];
          }
          if (v < tex_h-1)                          // get left bottom texel
          {
            tex_lb.r_ = tex_ptr[offset_lb + 2];
            tex_lb.g_ = tex_ptr[offset_lb + 1];
            tex_lb.b_ = tex_ptr[offset_lb + 0];
          }
          if (v < tex_h-1 && u < tex_w-1)           // get left bottom texel
          {
            tex_rb.r_ = tex_ptr[offset_rb + 2];
            tex_rb.g_ = tex_ptr[offset_rb + 1];
            tex_rb.b_ = tex_ptr[offset_rb + 0];
          }
        
          // Mix neighboring texels to get average texel

          FColor tex_total {};
          tex_total += tex_lt*(1.0f-du)*(1.0f-dv);
          tex_total += tex_rt*(du)*(1.0f-dv);
          tex_total += tex_lb*(1.0f-du)*(dv);
          tex_total += tex_rb*(du)*(dv);
        
          if (v >= tex_h-1 || u >= tex_w-1)         // special case rb texel
            tex_total = tex_lt;

          FColor total {c_curr.GetARGB()};
          total.Modulate(tex_total);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step;
    c_lhs += lc_step;
    c_rhs += rc_step;
  }

  // Part 2 : draw bottom part of triangle (0-0 is the left bottom)

  // Down middle pixel as it was drawn

  iy2 = iy2 - 1;

  // Precompute dy for sides (we need full differential, not cutted)

  int dy1y3 = iy1 - (iy3 - 1);
  int dy2y3 = iy2 - (iy3 - 1);

  // Compute x interpolants for left and right sides (we just suppose that
  // this side left and right)

  lx_step = v3.pos_.x - v1.pos_.x;
  rx_step = v3.pos_.x - v2.pos_.x;
  lz_step = 1.0f / v3.pos_.z - 1.0f / v1.pos_.z;
  rz_step = 1.0f / v3.pos_.z - 1.0f / v2.pos_.z;
  lu_step = v3.texture_.x - v1.texture_.x;
  lv_step = v3.texture_.y - v1.texture_.y;
  ru_step = v3.texture_.x - v2.texture_.x;
  rv_step = v3.texture_.y - v2.texture_.y;
  lc_step = c3 - c1;
  rc_step = c3 - c2;

  if (math::FNotZero(dy1y3)) {
    lx_step /= dy1y3;
    lz_step /= dy1y3;
    lu_step /= dy1y3;
    lv_step /= dy1y3;
    lc_step /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    rx_step /= dy2y3;
    rz_step /= dy2y3;
    ru_step /= dy2y3;
    rv_step /= dy2y3;
    rc_step /= dy2y3;
  }

  // Now make new x2 point and new interpolants for triangle borders

  float new_x {};
  float new_z {};
  float new_u {};
  float new_v {};
  FColor new_c {};
  int dy_passed {iy1 - iy2};

  if (lx_step > rx_step) {
    new_x = v1.pos_.x + (dy_passed * lx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * lz_step);
    new_u = (v1.texture_.x) + (lu_step * dy_passed);
    new_v = (v1.texture_.y) + (lv_step * dy_passed);
    new_c = c1 + (lc_step * dy_passed);
    x_lhs = new_x;
    x_rhs = v2.pos_.x;
    z_lhs = new_z;
    z_rhs = 1.0f / v2.pos_.z;
    u_lhs = new_u;
    u_rhs = v2.texture_.x;
    v_lhs = new_v;
    v_rhs = v2.texture_.y;
    c_lhs = new_c;
    c_rhs = c2;
  }
  else if (lx_step < rx_step) {
    std::swap(lx_step, rx_step);
    std::swap(lz_step, rz_step);
    std::swap(lu_step, ru_step);
    std::swap(lv_step, rv_step);
    std::swap(lc_step, rc_step);
    new_x = v1.pos_.x + (dy_passed * rx_step);
    new_z = (1.0f / v1.pos_.z) + (dy_passed * rz_step);
    new_u = (v1.texture_.x) + (ru_step * dy_passed);
    new_v = (v1.texture_.y) + (rv_step * dy_passed);
    new_c = c1 + (rc_step * dy_passed);    
    x_lhs = v2.pos_.x;
    x_rhs = new_x;
    z_lhs = 1.0f / v2.pos_.z;
    z_rhs = new_z;
    u_lhs = v2.texture_.x;
    u_rhs = new_u;
    v_lhs = v2.texture_.y;
    v_rhs = new_v;
    c_lhs = c2;
    c_rhs = new_c;
  }

  // Clip bottom triangle

  y_top_clip = iy2 - (sbuf_h - 1);
  y_top_clip = std::max(0, y_top_clip);

  // Forward interpolants if triangle is clipped

  x_lhs += lx_step * y_top_clip;
  x_rhs += rx_step * y_top_clip;
  z_lhs += lz_step * y_top_clip;
  z_rhs += rz_step * y_top_clip;
  u_lhs += lu_step * y_top_clip;
  u_rhs += ru_step * y_top_clip;
  v_lhs += lv_step * y_top_clip;
  v_rhs += rv_step * y_top_clip;
  c_lhs += lc_step * y_top_clip;
  c_rhs += rc_step * y_top_clip;

  // Draw bottom triangle (note that 0-0 is in left bottom corner of screen)

  y_top = iy2 - y_top_clip;
  y_bot = std::max(0, iy3);
  
  for (int y = y_top; y >= y_bot; --y)
  {
    // Convert most left and most right x pixels

    if (alpha) {
      xlb = ceil(x_lhs);
      xrb = ceil(x_rhs);          // fill convention
    }
    else {
      xlb = floor(x_lhs);
      xrb = ceil(x_rhs);              // guarantee no gaps
    }
    dx_curr = xrb - xlb;
  
    // Compute how much pixels we should clip from left

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Make 1/z interpolant and clip it from left side

    float z_step {};
    float u_step {};
    float v_step {};
    FColor c_step {};
    if ((xrb - xlb) != 0) {
      z_step = (z_rhs - z_lhs) / dx_curr;    
      u_step = (u_rhs - u_lhs) / dx_curr;
      v_step = (v_rhs - v_lhs) / dx_curr;
      c_step = (c_rhs - c_lhs) / dx_curr;
    }
    float z_curr = z_lhs + (z_step * xl_dx);
    float u_curr = u_lhs + (u_step * xl_dx);
    float v_curr = v_lhs + (v_step * xl_dx);
    FColor c_curr = c_lhs + (c_step * xl_dx);
   
    // Clip x interpolant

    xlb = std::max(0, xlb);
    xrb = std::min(sbuf_w - 1, xrb);

    // Iterate over x line and draw pixels (alpha or not cases)

    if (alpha)
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          Color<> buf_color {s_buf[idx]};     // blend background
          color::ShiftRight(buf_color, 1);

          Color<> tex_color {};               // bmp->get_pixel(u, v, r, g, b)
          int u = u_curr;
          int v = v_curr;
          int offset = (v * tex_width) + (u * tex_texel_width);
          tex_color.r_ = tex_ptr[offset + 2];
          tex_color.g_ = tex_ptr[offset + 1];
          tex_color.b_ = tex_ptr[offset + 0];
        
          Color<> total {c_curr.GetARGB()};
          total.Modulate(tex_color);

          color::ShiftRight(total, 1);

          s_buf[idx] = total.GetARGB() + buf_color.GetARGB();
          z_buf[idx] = z_curr;

          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    else
    {
      for (int x = xlb; x < xrb; ++x)
      {
        int idx = y * sbuf_w + x;
        if (z_curr > z_buf[idx])
        {
          // Get real u and v tex coords and diff from rounding for bifiltering
          
          int u = u_curr;
          int v = v_curr;
          float du = u_curr - u;
          float dv = v_curr - v;

          // Offsets to left-top, left-right, left-bottom and right-bottom texels

          int offset_lt = (v * tex_width) + (u * tex_texel_width);
          int offset_rt = offset_lt + tex_texel_width;
          int offset_lb = offset_lt + tex_width;
          int offset_rb = offset_lb + tex_texel_width;

          FColor tex_lt {};
          FColor tex_rt {};
          FColor tex_lb {};
          FColor tex_rb {};

          tex_lt.r_ = tex_ptr[offset_lt + 2];       // get origin texel
          tex_lt.g_ = tex_ptr[offset_lt + 1];
          tex_lt.b_ = tex_ptr[offset_lt + 0];

          if (u < tex_w-1)                          // get right top texel
          {
            tex_rt.r_ = tex_ptr[offset_rt + 2];
            tex_rt.g_ = tex_ptr[offset_rt + 1];
            tex_rt.b_ = tex_ptr[offset_rt + 0];
          }
          if (v < tex_h-1)                          // get left bottom texel
          {
            tex_lb.r_ = tex_ptr[offset_lb + 2];
            tex_lb.g_ = tex_ptr[offset_lb + 1];
            tex_lb.b_ = tex_ptr[offset_lb + 0];
          }
          if (v < tex_h-1 && u < tex_w-1)           // get left bottom texel
          {
            tex_rb.r_ = tex_ptr[offset_rb + 2];
            tex_rb.g_ = tex_ptr[offset_rb + 1];
            tex_rb.b_ = tex_ptr[offset_rb + 0];
          }

          // Mix neighboring texels to get average texel
          
          FColor tex_total {};
          tex_total += tex_lt*(1.0f-du)*(1.0f-dv);
          tex_total += tex_rt*(du)*(1.0f-dv);
          tex_total += tex_lb*(1.0f-du)*(dv);
          tex_total += tex_rb*(du)*(dv);

          if (v >= tex_h-1 || u >= tex_w-1)         // special case rb texel
            tex_total = tex_lt;
        
          FColor total {c_curr.GetARGB()};
          total.Modulate(tex_total);

          s_buf[idx] = total.GetARGB();
          z_buf[idx] = z_curr;
          ++total_drawn;
        }
        ++idx;
        z_curr += z_step;
        u_curr += u_step;
        v_curr += v_step;
        c_curr += c_step;        
      }
    }
    
    x_lhs += lx_step;
    x_rhs += rx_step;
    z_lhs += lz_step;
    z_rhs += rz_step;
    u_lhs += lu_step;
    u_rhs += ru_step;
    v_lhs += lv_step;
    v_rhs += rv_step; 
    c_lhs += lc_step;
    c_rhs += rc_step;
  }

  return total_drawn;
}

} // namespace anshub