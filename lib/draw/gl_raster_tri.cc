// *************************************************************
// File:    gl_raster_tri.cc
// Descr:   rasterizes triangles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_raster_tri.h"

namespace anshub {

// Draws solid triangle. First, we should guarantee that y1 is most top, and
// y3 is most bottom points. Then we draw top triangle from top to middle,
// and then draw from bottom to the middle

void raster::SolidTriangle(
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

  int x1 = std::floor(px1);
  int x2 = std::floor(px2);
  int x3 = std::floor(px3);
  int y1 = std::floor(py1);
  int y2 = std::floor(py2);
  int y3 = std::floor(py3);
  
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
    int xlb = std::floor(x_lhs);
    int xrb = std::ceil(x_rhs);
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
    int xlb = std::floor(x_lhs);
    int xrb = std::ceil(x_rhs);
    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width()-1, xrb);
    raster::HorizontalLine(y, xlb, xrb, color, buf);
  }
}

// Draws gourang solid triangle. The proccess of drawing is similar
// to the draw::SolidTriangle, but here we interpolate vertexes colors
// (gradient)

void raster::GourangTriangle(
    float px1, float py1, float px2,
    float py2, float px3, float py3,
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

    int xlb = std::floor(x_lhs);
    int xrb = std::ceil(x_rhs);

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

    int xlb = std::floor(x_lhs);
    int xrb = std::ceil(x_rhs);

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

// Draws textured triangle without lighting

void raster::TexturedTriangle(
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    Bitmap* bmp, Buffer& buf)
{
    // Convert float to int for vertex positions

  int x1 = std::floor(p1.x);
  int x2 = std::floor(p2.x);
  int x3 = std::floor(p3.x);
  int y1 = std::floor(p1.y);
  int y2 = std::floor(p2.y);
  int y3 = std::floor(p3.y);
  
  // Aliases to texture coordinates

  float u1 = t1.x;
  float u2 = t2.x;
  float u3 = t3.x;
  float v1 = t1.y;
  float v2 = t2.y;
  float v3 = t3.y;

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

    int xlb = std::floor(x_lhs);          // xlb - x left border
    int xrb = std::ceil(x_rhs);
    
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
      bmp->get_pixel(std::ceil(curr_u), std::ceil(curr_v), r, g, b);
      
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

    int xlb = std::floor(x_lhs);          // xlb - x left border
    int xrb = std::ceil(x_rhs);

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
      bmp->get_pixel(std::ceil(curr_u), std::ceil(curr_v), r, g, b);
      
      raster::Point(x, y, color::MakeARGB(255, r, g, b), buf);
    }
  }
}

// Draws textured triangle with flat lighting

void raster::TexturedTriangleFL(
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    uint color, Bitmap* bmp, Buffer& buf)
{
    // Convert float to int for vertex positions

  int x1 = std::floor(p1.x);
  int x2 = std::floor(p2.x);
  int x3 = std::floor(p3.x);
  int y1 = std::floor(p1.y);
  int y2 = std::floor(p2.y);
  int y3 = std::floor(p3.y);
  
  // Aliases to texture coordinates

  float u1 = t1.x;
  float u2 = t2.x;
  float u3 = t3.x;
  float v1 = t1.y;
  float v2 = t2.y;
  float v3 = t3.y;

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

    int xlb = std::floor(x_lhs);          // xlb - x left border
    int xrb = std::ceil(x_rhs);
    
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
      
      // Get texel

      uchar r {};
      uchar g {};
      uchar b {};
      bmp->get_pixel(std::ceil(curr_u), std::ceil(curr_v), r, g, b);
      
      // Modulate light and color
      
      Color<> tex {r, g, b};
      Color<> total {color};
      total.Modulate(tex);

      // Draw texel
      
      raster::Point(x, y, total.GetARGB(), buf);
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

    int xlb = std::floor(x_lhs);          // xlb - x left border
    int xrb = std::ceil(x_rhs);

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
      
      // Get texel

      uchar r {};
      uchar g {};
      uchar b {};
      bmp->get_pixel(std::ceil(curr_u), std::ceil(curr_v), r, g, b);
      
      // Modulate light and color
      
      Color<> tex {r, g, b};
      Color<> total {color};
      total.Modulate(tex);
      
      // Draw texel

      raster::Point(x, y, total.GetARGB(), buf);
    }
  }
}


// Draws solid triangle using 1/z buffer

void raster::SolidTriangle(
    cVertex& v1, cVertex& v2, cVertex& v3,
    uint color, ZBuffer& zbuf, Buffer& buf)
{
  // Todo: function SortVertices(v1, v2, v3);
  
  // Convert float to int

  int x1 = std::floor(v1.pos_.x);
  int x2 = std::floor(v2.pos_.x);
  int x3 = std::floor(v3.pos_.x);
  int y1 = std::floor(v1.pos_.y);
  int y2 = std::floor(v2.pos_.y);
  int y3 = std::floor(v3.pos_.y);

  // Get 1/z coords

  float z1 = 1.0f / v1.pos_.z;
  float z2 = 1.0f / v2.pos_.z;
  float z3 = 1.0f / v3.pos_.z;

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(z2, z3);
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
  }
  else if ((y1 < y2) && (y1 < y3 || math::Feq(y1, y3))) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(z3, z2);
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(z2, z3);
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(z1, z2);
  }

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

  // Here we calc 1/z coordinate for left and right edges

  float dx_lz {z2 - z1};
  float dx_rz {z3 - z1};
  
  if (math::FNotZero(dy2y1))
    dx_lz /= dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rz /= dy3y1;

  // Now choose, where really placed left and right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lz, dx_rz);
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
    
    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = z1 + (dx_lz * dy);
    float x_rz = z1 + (dx_rz * dy);
    
    // Compute x border coords for left edge and right edges
    
    int xlb = std::floor(x_lhs);
    int xrb = std::ceil(x_rhs);

    // Compute x offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute 1/z differential between edges at the current y before clipping
    // left and right sides

    float  dx_currx_z {};

    if ((xrb - xlb) != 0)
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);

    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width() - 1, xrb);

    // Interpolate color and 1/z coordinate for each pixel    
    
    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb;
      float curr_z = x_lz + (dx_currx_z * dx);
      if (curr_z > zbuf(x,y))
      {
        raster::Point(x, y, color, buf);
        zbuf(x,y) = curr_z;
      }
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

  // Here we calc 1/z coordinate for left and right edges

  dx_lz = z1 - z3;
  dx_rz = z2 - z3;

  if (math::FNotZero(dy1y3))
    dx_lz /= dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rz /= dy2y3;
  
  // Determine which is really left step and really is right step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lz, dx_rz);
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
  y_top = std::min(y2, buf.Height()-1);

  // Draw bottom triangle

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;

    int dy = y - y3;                    // we need real dy, not clipped

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = z3 + (dx_lz * dy);
    float x_rz = z3 + (dx_rz * dy);

    // Compute x border coords for left edge and right edges

    int xlb = std::floor(x_lhs);
    int xrb = std::ceil(x_rhs);

    // Compute color offset from left screen border if face would be clipped
    
    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute 1/z differential between edges at the current y before clipping
    // left and right sides

    float  dx_currx_z {};
    if ((xrb - xlb) != 0)
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
   
    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width()-1, xrb);

    // Interpolate color and 1/z coordinate for each pixel    
    
    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb;
      float curr_z = x_lz + (dx_currx_z * dx);
      if (curr_z > zbuf(x,y))
      {
        raster::Point(x, y, color, buf);
        zbuf(x,y) = curr_z;
      }
    }
  }
}

// Draws gourang solid triangle. The proccess of drawing is similar
// to the draw::SolidTriangle, but here we interpolate vertexes colors
// (gradient).
// This is the version that uses ZBuffer and Vertexes as arguments 

void raster::GourangTriangle(
    cVertex& v1, cVertex& v2, cVertex& v3,
    ZBuffer& zbuf, Buffer& buf)
{
  // Convert float to int

  int x1 = std::floor(v1.pos_.x);
  int x2 = std::floor(v2.pos_.x);
  int x3 = std::floor(v3.pos_.x);
  int y1 = std::floor(v1.pos_.y);
  int y2 = std::floor(v2.pos_.y);
  int y3 = std::floor(v3.pos_.y);
  
  // Get 1/z coords

  float z1 = 1.0f / v1.pos_.z;
  float z2 = 1.0f / v2.pos_.z;
  float z3 = 1.0f / v3.pos_.z;

  // Prepare colors

  FColor c1 {v1.color_};
  FColor c2 {v2.color_};
  FColor c3 {v3.color_};

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(z2, z3);
    std::swap(c2, c3);
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(c1, c2);
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(c1, c2);
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(z3, z2);
    std::swap(c3, c2);
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(z2, z3);
    std::swap(c2, c3);
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(z1, z2);
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

  // Here we calc 1/z coordinate for left and right edges

  float dx_lz {z2 - z1};
  float dx_rz {z3 - z1};
  
  if (math::FNotZero(dy2y1))
    dx_lz /= dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rz /= dy3y1;

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
    std::swap(dx_lc, dx_rc);
    std::swap(dx_lz, dx_rz);
  }

  // Now we should draw triangle from top to middle (from y1 to y2)

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

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = z1 + (dx_lz * dy);
    float x_rz = z1 + (dx_rz * dy);
    
    // Compute differentials of colors on the left and right edges
    
    FColor x_lc = c1 + (dx_lc * dy);
    FColor x_rc = c1 + (dx_rc * dy);

    // Compute x border coords for left edge and right edges

    int xlb = std::floor(x_lhs);
    int xrb = std::ceil(x_rhs);
        
    // Compute x offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute colors and 1/z differentials between edges at the current y
    // before clipping left and right sides

    FColor dx_currx_c {c1};
    float  dx_currx_z {};

    if ((xrb - xlb) != 0)
    {
      dx_currx_c = (x_rc - x_lc) / (xrb - xlb);
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
    }
   
    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width() - 1, xrb);
    
    // Interpolate color and 1/z coordinate for each pixel    

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb;
      
      FColor curr_c = x_lc + (dx_currx_c * dx);
      float  curr_z = x_lz + (dx_currx_z * dx);

      if (curr_z > zbuf(x,y))
      {
        raster::Point(x, y, curr_c.GetARGB(), buf);
        zbuf(x,y) = curr_z;
      }
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

  // Here we calc 1/z coordinate for left and right edges

  dx_lz = z1 - z3;
  dx_rz = z2 - z3;
  
  if (math::FNotZero(dy1y3))
    dx_lz /= dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rz /= dy2y3;

  // Calc side colors differential (from top to left bottom and to right bottom)

  dx_lc = c1 - c3;
  dx_rc = c2 - c3;
  if (math::FNotZero(dy1y3))
    dx_lc /= dy1y3 - 1;           // -1 since we go -1 step less than diff
  if (math::FNotZero(dy2y3))
    dx_rc /= dy2y3 - 1;           // in loop when we would draw triangle
  
  // Determine which is really left step and really is right step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lc, dx_rc);
    std::swap(dx_lz, dx_rz);
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
  y_top = std::min(y2, buf.Height()-1);

  // Draw bottom triangle

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;

    int dy = y - y3;                    // we need real dy, not clipped

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = z3 + (dx_lz * dy);
    float x_rz = z3 + (dx_rz * dy);

    // Compute differentials of colors on the left and right edges
    
    FColor x_lc = c3 + (dx_lc * dy);    // find colors on the edges
    FColor x_rc = c3 + (dx_rc * dy);

    // Compute x border coords for left edge and right edges

    int xlb = std::floor(x_lhs);
    int xrb = std::ceil(x_rhs);

    // Compute color offset from left screen border if face would be clipped
    
    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute colors and 1/z differentials between edges at the current y
    // before clipping left and right sides

    FColor dx_currx_c {c3};
    float  dx_currx_z {};

    if ((xrb - xlb) != 0)
    {
      dx_currx_c = (x_rc - x_lc) / (xrb - xlb);
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
    }
   
    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(buf.Width()-1, xrb);
    
    // Interpolate color and 1/z coordinate for each pixel    
    
    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb;

      FColor curr_c = x_lc + (dx_currx_c * dx);
      float  curr_z = x_lz + (dx_currx_z * dx);

      if (curr_z > zbuf(x,y))
      {
        raster::Point(x, y, curr_c.GetARGB(), buf);
        zbuf(x,y) = curr_z;        
      }
    }
  }
}

// Draws textured triangle without lighting, but with correct perspective
// using 1/z interpolating and returns total amount of drawn pixels

int raster::TexturedTriangle(
    cVertex& p1, cVertex& p2, cVertex& p3,
    Bitmap* bmp, ZBuffer& zbuf, Buffer& scr_buf)
{
  // Debug variables
  
  int total_drawn {};

  // Prepare fast screen buffer, texture and z-buffer access

  auto scr_ptr = scr_buf.GetPointer();
  int  scr_width = scr_buf.Width();
  int  scr_height = scr_buf.Height();
  auto tex_ptr = bmp->GetPointer();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* z_ptr = zbuf.GetPointer();
  auto  z_width = zbuf.Width();

  // Convert float to int for vertex positions

  int x1 = math::Floor(p1.pos_.x);
  int x2 = math::Floor(p2.pos_.x);
  int x3 = math::Floor(p3.pos_.x);
  int y1 = math::Floor(p1.pos_.y);
  int y2 = math::Floor(p2.pos_.y);
  int y3 = math::Floor(p3.pos_.y);

  // Extract 1/z coordinates
  
  float z1 = p1.pos_.z;     // 1/z as we use 1/z buffer
  float z2 = p2.pos_.z;
  float z3 = p3.pos_.z;
  
  // Extract texture coordinates

  float u1 = p1.texture_.x;
  float u2 = p2.texture_.x;
  float u3 = p3.texture_.x;
  float v1 = p1.texture_.y;
  float v2 = p2.texture_.y;
  float v3 = p3.texture_.y;

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(z2, z3);
    std::swap(u2, u3);
    std::swap(v2, v3);
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(u1, u2);
    std::swap(v1, v2);
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(z3, z2);
    std::swap(u3, u2);
    std::swap(v3, v2);
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(z2, z3);
    std::swap(u2, u3);
    std::swap(v2, v3);
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(z1, z2);
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

  float dx_lu {u2/z2 - u1/z1};
  float dx_lv {v2/z2 - v1/z1};
  float dx_ru {u3/z3 - u1/z1};
  float dx_rv {v3/z3 - v1/z1};

  if (math::FNotZero(dy2y1)) {
    dx_lu /= dy2y1;
    dx_lv /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    dx_ru /= dy3y1;
    dx_rv /= dy3y1;
  }

  // Here we calc 1/z coordinate for left and right edges

  float dx_lz {1.0f/z2 - 1.0f/z1};
  float dx_rz {1.0f/z3 - 1.0f/z1};
  
  if (math::FNotZero(dy2y1))
    dx_lz /= dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rz /= dy3y1;
  
  // Now choose, where really placed left and right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
    std::swap(dx_lz, dx_rz);
  }

  // Now we should draw triangle from top to middle (from y1 to y2)

  float x_lhs {(float)x1};                // float curr x coord left
  float x_rhs {(float)x1};                // float curr x coord right

  // Clip top and bottom

  if (y1 < 0 || y3 >= scr_height)       // if triangle is full out of screen
    return total_drawn;

  int y_top_clip = y1 - scr_height + 1; // how much pixels is clipped 
  y_top_clip = std::max(0, y_top_clip);   //  from the top of screen

  x_lhs += dx_lhs * y_top_clip;           // forward x left and x right curr
  x_rhs += dx_rhs * y_top_clip;           //  coords if y1 is out of screen

  int y_top = y1 - y_top_clip;            // define new drawable top
  int y_bot = std::max(0, y2);            //  and bottom

  // Prepare start values of interpolants

  float x_u_start = u1/z1;
  float x_v_start = v1/z1;
  float x_z_start = 1.0f/z1;

  // Draw top triangle

  Color<> tex_color {};                 // texture color

  for (int y = y_top; y >= y_bot; --y)
  {
    // Compute differentials of texture coords on the left and right edges

    int dy = y1 - y;                        // we need real dy, not clipped
    
    float x_lu = x_u_start + (dx_lu * dy);  // find tex_coords on the edges
    float x_lv = x_v_start + (dx_lv * dy);
    float x_ru = x_u_start + (dx_ru * dy);
    float x_rv = x_v_start + (dx_rv * dy);

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = x_z_start + (dx_lz * dy);
    float x_rz = x_z_start + (dx_rz * dy);

    // Compute x border coords for left edge and right edges

    int xlb = math::Floor(x_lhs);          // xlb - x left border
    int xrb = math::Ceil(x_rhs);
    
    // Compute texture offset from left screen border if face would be clipped

    int xl_dx = 0 + xlb;
    xl_dx = xl_dx > 0 ? 0 : std::abs(xl_dx);

    // Compute differential between edges of tex coords at the current y
    // before clipping left and right sides

    float dx_currx_u {};                   // find dx between left and right
    float dx_currx_v {};
    float dx_currx_z {};

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
    }

    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(scr_width - 1, xrb);

    // Interpolate texture coordinate for each pixel

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;           // we need real dx, not clipped
      float curr_z = x_lz + (dx_currx_z * dx);
      
      if (curr_z > z_ptr[y * z_width + x])
      {
        // Get real texture coordinates

        float curr_u = x_lu + (dx_currx_u * dx);
        float curr_v = x_lv + (dx_currx_v * dx);
        int u = std::abs((int)(curr_u / curr_z));
        int v = std::abs((int)(curr_v / curr_z));

        // Get texture pixel. This is same as call to call
        // this function: bmp->get_pixel(u, v, r, g, b);

        int offset = (v * tex_width) + (u * tex_texel_width);
        tex_color.r_ = tex_ptr[offset + 2];
        tex_color.g_ = tex_ptr[offset + 1];
        tex_color.b_ = tex_ptr[offset + 0];
        
        // Draw point

        scr_ptr[x + y * scr_width] = tex_color.GetARGB();
        z_ptr[y * z_width + x] = curr_z;
        ++total_drawn;
      }
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
  
  dx_lu = u1/z1 - u3/z3;
  dx_lv = v1/z1 - v3/z3;
  dx_ru = u2/z2 - u3/z3;
  dx_rv = v2/z2 - v3/z3;

  if (math::FNotZero(dy1y3)) {
    dx_lu /= dy1y3;
    dx_lv /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    dx_ru /= dy2y3;
    dx_rv /= dy2y3;
  }

  // Here we calc 1/z coordinate for left and right edges

  dx_lz = 1.0f/z1 - 1.0f/z3;
  dx_rz = 1.0f/z2 - 1.0f/z3;
  
  if (math::FNotZero(dy1y3))
    dx_lz /= dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rz /= dy2y3;
  
  // Determine which is really left side step and really is right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
    std::swap(dx_lz, dx_rz);
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

  // Prepare start values of interpolants

  x_u_start = u3/z3;
  x_v_start = v3/z3;
  x_z_start = 1.0f/z3;

  // Draw bottom triangle

  tex_color = Color<> {};               // texture and its light color

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;

    // Compute differentials of texture coords on the left and right edges

    int dy = y - y3;                        // we need real dy, not clipped

    float x_lu = x_u_start + (dx_lu * dy);  // find tex_coords on the edges
    float x_lv = x_v_start + (dx_lv * dy);
    float x_ru = x_u_start + (dx_ru * dy);
    float x_rv = x_v_start + (dx_rv * dy);

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = x_z_start + (dx_lz * dy);
    float x_rz = x_z_start + (dx_rz * dy);

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
    float dx_currx_z {};

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
    }

    // Clip left and right

    xlb = std::max(0, xlb);               // clip left and right lines
    xrb = std::min(scr_width - 1, xrb);
    
    // Interpolate texture coordinate for each pixel
    
    for (int x = xlb; x < xrb; ++x)        // for each pixel interpolate color
    {
      int dx = x - xlb + xl_dx;
      float curr_z = x_lz + (dx_currx_z * dx);
      if (curr_z > zbuf(x,y))
      {
        // Get real texture coordinates
        
        float curr_u = x_lu + (dx_currx_u * dx);
        float curr_v = x_lv + (dx_currx_v * dx);
        int u = std::abs((int)(curr_u / curr_z));   // (int) - fast floor
        int v = std::abs((int)(curr_v / curr_z));

        // Get texture pixel. This is same as call to call
        // this function: bmp->get_pixel(u, v, r, g, b);

        int offset = (v * tex_width) + (u * tex_texel_width);
        tex_color.r_ = tex_ptr[offset + 2];
        tex_color.g_ = tex_ptr[offset + 1];
        tex_color.b_ = tex_ptr[offset + 0];

        // Draw point

        scr_ptr[x + y * scr_width] = tex_color.GetARGB();
        z_ptr[y * z_width + x] = curr_z;
        ++total_drawn;
      }
    }
  }
  return total_drawn;
}

// Draws textured triangle with flat lighting and with correct perspective
// using 1/z interpolating and returns number of drawn pixels

int raster::TexturedTriangleFL(
    cVertex& p1, cVertex& p2, cVertex& p3,
    uint color, Bitmap* bmp, ZBuffer& zbuf, Buffer& scr_buf)
{
  // Debug variables

  int total_drawn {};

  // Prepare fast screen buffer, texture and z-buffer access

  auto scr_ptr = scr_buf.GetPointer();
  int  scr_width = scr_buf.Width();
  int  scr_height = scr_buf.Height();
  auto tex_ptr = bmp->GetPointer();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* z_ptr = zbuf.GetPointer();
  auto  z_width = zbuf.Width();

  // Convert flat light color to Color<>

  Color<> light_color {color};

  // Convert float to int for vertex positions

  int x1 = math::Floor(p1.pos_.x);
  int x2 = math::Floor(p2.pos_.x);
  int x3 = math::Floor(p3.pos_.x);
  int y1 = math::Floor(p1.pos_.y);
  int y2 = math::Floor(p2.pos_.y);
  int y3 = math::Floor(p3.pos_.y);

  // Extract 1/z coordinates
  
  float z1 = p1.pos_.z;    // 1/z as we use 1/z buffer
  float z2 = p2.pos_.z;
  float z3 = p3.pos_.z;
  
  // Extract texture coordinates

  float u1 = p1.texture_.x;
  float u2 = p2.texture_.x;
  float u3 = p3.texture_.x;
  float v1 = p1.texture_.y;
  float v2 = p2.texture_.y;
  float v3 = p3.texture_.y;

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(z2, z3);
    std::swap(u2, u3);
    std::swap(v2, v3);
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(u1, u2);
    std::swap(v1, v2);
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(z3, z2);
    std::swap(u3, u2);
    std::swap(v3, v2);
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(z2, z3);
    std::swap(u2, u3);
    std::swap(v2, v3);
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(z1, z2);
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

  float dx_lu {u2/z2 - u1/z1};
  float dx_lv {v2/z2 - v1/z1};
  float dx_ru {u3/z3 - u1/z1};
  float dx_rv {v3/z3 - v1/z1};

  if (math::FNotZero(dy2y1)) {
    dx_lu /= dy2y1;
    dx_lv /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    dx_ru /= dy3y1;
    dx_rv /= dy3y1;
  }

  // Here we calc 1/z coordinate for left and right edges

  float dx_lz {1.0f/z2 - 1.0f/z1};
  float dx_rz {1.0f/z3 - 1.0f/z1};
  
  if (math::FNotZero(dy2y1))
    dx_lz /= dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rz /= dy3y1;
  
  // Now choose, where really placed left and right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
    std::swap(dx_lz, dx_rz);
  }

  // Now we should draw triangle from top to middle (from y1 to y2)

  float x_lhs {(float)x1};                // float curr x coord left
  float x_rhs {(float)x1};                // float curr x coord right

  // Clip top and bottom

  if (y1 < 0 || y3 >= scr_height)       // if triangle is full out of screen
    return 0;

  int y_top_clip = y1 - scr_height + 1; // how much pixels is clipped 
  y_top_clip = std::max(0, y_top_clip);   //  from the top of screen

  x_lhs += dx_lhs * y_top_clip;           // forward x left and x right curr
  x_rhs += dx_rhs * y_top_clip;           //  coords if y1 is out of screen

  int y_top = y1 - y_top_clip;            // define new drawable top
  int y_bot = std::max(0, y2);            //  and bottom

  // Prepare start values of interpolants

  float x_u_start = u1/z1;
  float x_v_start = v1/z1;
  float x_z_start = 1.0f/z1;

  // Draw top triangle

  Color<> tex_color {};                 // texture and its light color
  Color<> total_color {};               //  to interpolate it inside x row loop

  for (int y = y_top; y >= y_bot; --y)
  {
    // Compute differentials of texture coords on the left and right edges

    int dy = y1 - y;                        // we need real dy, not clipped
    
    float x_lu = x_u_start + (dx_lu * dy);  // find tex_coords on the edges
    float x_lv = x_v_start + (dx_lv * dy);
    float x_ru = x_u_start + (dx_ru * dy);
    float x_rv = x_v_start + (dx_rv * dy);

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = x_z_start + (dx_lz * dy);
    float x_rz = x_z_start + (dx_rz * dy);

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
    float dx_currx_z {};

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
    }

    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(scr_width - 1, xrb);

    // Interpolate texture coordinate for each pixel

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;           // we need real dx, not clipped
      float curr_z = x_lz + (dx_currx_z * dx);
      
      if (curr_z > z_ptr[y * z_width + x])
      {
        // Get real texture coordinates

        float curr_u = x_lu + (dx_currx_u * dx);
        float curr_v = x_lv + (dx_currx_v * dx);
        int u = std::abs((int)(curr_u / curr_z));
        int v = std::abs((int)(curr_v / curr_z));
      
        // Get texture pixel. This is same as call to call
        // this function: bmp->get_pixel(u, v, r, g, b);

        int offset = (v * tex_width) + (u * tex_texel_width);
        tex_color.r_ = tex_ptr[offset + 2];
        tex_color.g_ = tex_ptr[offset + 1];
        tex_color.b_ = tex_ptr[offset + 0];
      
        // Modulate light and color

        Color<> total {light_color};
        total.Modulate(tex_color);

        // Draw point

        scr_ptr[x + y * scr_width] = total.GetARGB();
        z_ptr[y * z_width + x] = curr_z;
        ++total_drawn;
      }
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
  
  dx_lu = u1/z1 - u3/z3;
  dx_lv = v1/z1 - v3/z3;
  dx_ru = u2/z2 - u3/z3;
  dx_rv = v2/z2 - v3/z3;

  if (math::FNotZero(dy1y3)) {
    dx_lu /= dy1y3;
    dx_lv /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    dx_ru /= dy2y3;
    dx_rv /= dy2y3;
  }

  // Here we calc 1/z coordinate for left and right edges

  dx_lz = 1.0f/z1 - 1.0f/z3;
  dx_rz = 1.0f/z2 - 1.0f/z3;
  
  if (math::FNotZero(dy1y3))
    dx_lz /= dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rz /= dy2y3;
  
  // Determine which is really left side step and really is right side step

  if (dx_lhs > dx_rhs)
  {
    std::swap(dx_lhs, dx_rhs);
    std::swap(dx_lu, dx_ru);
    std::swap(dx_lv, dx_rv);
    std::swap(dx_lz, dx_rz);
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

  // Prepare start values of interpolants

  x_u_start = u3/z3;
  x_v_start = v3/z3;
  x_z_start = 1.0f/z3;

  // Draw bottom triangle

  tex_color = Color<> {};               // texture and its light color
  total_color = Color<> {};             //  to interpolate it inside x row loop

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;

    // Compute differentials of texture coords on the left and right edges

    int dy = y - y3;                        // we need real dy, not clipped

    float x_lu = x_u_start + (dx_lu * dy);  // find tex_coords on the edges
    float x_lv = x_v_start + (dx_lv * dy);
    float x_ru = x_u_start + (dx_ru * dy);
    float x_rv = x_v_start + (dx_rv * dy);

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = x_z_start + (dx_lz * dy);
    float x_rz = x_z_start + (dx_rz * dy);

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
    float dx_currx_z {};

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
    }

    // Clip left and right

    xlb = std::max(0, xlb);               // clip left and right lines
    xrb = std::min(scr_width - 1, xrb);
    
    // Interpolate texture coordinate for each pixel
    
    for (int x = xlb; x < xrb; ++x)        // for each pixel interpolate color
    {
      int dx = x - xlb + xl_dx;
      float curr_z = x_lz + (dx_currx_z * dx);
      
      if (curr_z > z_ptr[y * z_width + x])
      {
        // Get real texture coordinates
        
        float curr_u = x_lu + (dx_currx_u * dx);
        float curr_v = x_lv + (dx_currx_v * dx);
        int u = std::abs((int)(curr_u / curr_z));   // (int) - fast floor
        int v = std::abs((int)(curr_v / curr_z));

        // Get texture pixel. This is same as call to call
        // this function: bmp->get_pixel(u, v, r, g, b);

        int offset = (v * tex_width) + (u * tex_texel_width);
        tex_color.r_ = tex_ptr[offset + 2];
        tex_color.g_ = tex_ptr[offset + 1];
        tex_color.b_ = tex_ptr[offset + 0];
      
        // Modulate light and color

        Color<> total {light_color};
        total.Modulate(tex_color);
        
        // Draw point

        scr_ptr[x + y * scr_width] = total.GetARGB();
        z_ptr[y * z_width + x] = curr_z;
        ++total_drawn;
      }
    }
  }
  return total_drawn;
}

// Draws textured triangle with gourang lighting and with correct perspective
// using 1/z interpolating and returns number of drawn pixels

int raster::TexturedTriangleGR(
    cVertex& p1, cVertex& p2, cVertex& p3,
    Bitmap* bmp, ZBuffer& zbuf, Buffer& scr_buf)
{
  // Debug variables
  
  int total_drawn {};

  // Prepare fast screen buffer, texture and z-buffer access

  auto scr_ptr = scr_buf.GetPointer();
  int  scr_width = scr_buf.Width();
  int  scr_height = scr_buf.Height();
  auto tex_ptr = bmp->GetPointer();
  auto tex_width = bmp->GetRowIncrement();
  auto tex_texel_width = bmp->GetBytesPerPixel();
  auto* z_ptr = zbuf.GetPointer();
  auto  z_width = zbuf.Width();

  // Convert float to int for vertex positions

  int x1 = math::Floor(p1.pos_.x);
  int x2 = math::Floor(p2.pos_.x);
  int x3 = math::Floor(p3.pos_.x);
  int y1 = math::Floor(p1.pos_.y);
  int y2 = math::Floor(p2.pos_.y);
  int y3 = math::Floor(p3.pos_.y);

  // Extract 1/z coordinates
  
  float z1 = p1.pos_.z;    // 1/z as we use 1/z buffer
  float z2 = p2.pos_.z;
  float z3 = p3.pos_.z;
  
  // Extract texture coordinates

  float u1 = p1.texture_.x;
  float u2 = p2.texture_.x;
  float u3 = p3.texture_.x;
  float v1 = p1.texture_.y;
  float v2 = p2.texture_.y;
  float v3 = p3.texture_.y;

  // Prepare colors  

  FColor c1 {p1.color_};
  FColor c2 {p2.color_};
  FColor c3 {p3.color_};

  // Make y1 as top point and y3 as bottom point, y2 is middle

  if (y2 < y3) {
    std::swap(x2, x3);
    std::swap(y2, y3);
    std::swap(z2, z3);
    std::swap(u2, u3);
    std::swap(v2, v3);
    std::swap(c2, c3);    
  }
  if ((y1 < y2) && (y1 > y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(c1, c2);    
  }
  else if ((y1 < y2) && (y1 <= y3)) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(u1, u2);
    std::swap(v1, v2);
    std::swap(c1, c2);    
    std::swap(x3, x2);
    std::swap(y3, y2);
    std::swap(z3, z2);
    std::swap(u3, u2);
    std::swap(v3, v2);
    std::swap(c3, c2);    
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(y2, y3) && x2 > x3) {
    std::swap(x2, x3);
    std::swap(z2, z3);
    std::swap(u2, u3);
    std::swap(v2, v3);
    std::swap(c2, c3);    
  }

  // If polygon is flat top, sort left to right

  if (math::Feq(y1, y2) && x1 > x2) {
    std::swap(x1, x2);
    std::swap(z1, z2);
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

  float dx_lu {u2/z2 - u1/z1};
  float dx_lv {v2/z2 - v1/z1};
  float dx_ru {u3/z3 - u1/z1};
  float dx_rv {v3/z3 - v1/z1};

  if (math::FNotZero(dy2y1)) {
    dx_lu /= dy2y1;
    dx_lv /= dy2y1;
  }
  if (math::FNotZero(dy3y1)) {
    dx_ru /= dy3y1;
    dx_rv /= dy3y1;
  }

  // Here we calc 1/z coordinate for left and right edges

  float dx_lz {1.0f/z2 - 1.0f/z1};
  float dx_rz {1.0f/z3 - 1.0f/z1};
  
  if (math::FNotZero(dy2y1))
    dx_lz /= dy2y1;
  if (math::FNotZero(dy3y1))
    dx_rz /= dy3y1;
  
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
    std::swap(dx_lz, dx_rz);
    std::swap(dx_lc, dx_rc);    
  }

  // Now we should draw triangle from top to middle (from y1 to y2)

  float x_lhs {(float)x1};                // float curr x coord left
  float x_rhs {(float)x1};                // float curr x coord right

  // Clip top and bottom

  if (y1 < 0 || y3 >= scr_height)       // if triangle is full out of screen
    return total_drawn;

  int y_top_clip = y1 - scr_height + 1; // how much pixels is clipped 
  y_top_clip = std::max(0, y_top_clip);   //  from the top of screen

  x_lhs += dx_lhs * y_top_clip;           // forward x left and x right curr
  x_rhs += dx_rhs * y_top_clip;           //  coords if y1 is out of screen

  int y_top = y1 - y_top_clip;            // define new drawable top
  int y_bot = std::max(0, y2);            //  and bottom

  // Prepare start values of interpolants

  float x_u_start = u1/z1;
  float x_v_start = v1/z1;
  float x_z_start = 1.0f/z1;

  // Draw top triangle

  FColor tex_color {};                 // texture and its light color
  FColor total_color {};               //  to interpolate it inside x row loop

  for (int y = y_top; y >= y_bot; --y)
  {
    // Compute differentials of texture coords on the left and right edges

    int dy = y1 - y;                        // we need real dy, not clipped
    
    float x_lu = x_u_start + (dx_lu * dy);  // find tex_coords on the edges
    float x_lv = x_v_start + (dx_lv * dy);
    float x_ru = x_u_start + (dx_ru * dy);
    float x_rv = x_v_start + (dx_rv * dy);

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = x_z_start + (dx_lz * dy);
    float x_rz = x_z_start + (dx_rz * dy);

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
    float dx_currx_z {};
    FColor dx_currx_c {c1};    

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
      dx_currx_c = (x_rc - x_lc) / (xrb - xlb);      
    }

    // Clip left and right lines of face

    xlb = std::max(0, xlb);
    xrb = std::min(scr_width - 1, xrb);

    // Interpolate texture coordinate for each pixel

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;           // we need real dx, not clipped
      float curr_z = x_lz + (dx_currx_z * dx);
      FColor curr_c = x_lc + (dx_currx_c * dx);      
      
      if (curr_z > z_ptr[y * z_width + x])
      {
        // Get real texture coordinates

        float curr_u = x_lu + (dx_currx_u * dx);
        float curr_v = x_lv + (dx_currx_v * dx);
        int u = std::abs((int)(curr_u / curr_z));
        int v = std::abs((int)(curr_v / curr_z));
      
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
        z_ptr[y * z_width + x] = curr_z;
        ++total_drawn;
      }
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
  
  dx_lu = u1/z1 - u3/z3;
  dx_lv = v1/z1 - v3/z3;
  dx_ru = u2/z2 - u3/z3;
  dx_rv = v2/z2 - v3/z3;

  if (math::FNotZero(dy1y3)) {
    dx_lu /= dy1y3;
    dx_lv /= dy1y3;
  }
  if (math::FNotZero(dy2y3)) {
    dx_ru /= dy2y3;
    dx_rv /= dy2y3;
  }

  // Here we calc 1/z coordinate for left and right edges

  dx_lz = 1.0f/z1 - 1.0f/z3;
  dx_rz = 1.0f/z2 - 1.0f/z3;
  
  if (math::FNotZero(dy1y3))
    dx_lz /= dy1y3;
  if (math::FNotZero(dy2y3))
    dx_rz /= dy2y3;
  
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
    std::swap(dx_lz, dx_rz);
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

  // Prepare start values of interpolants

  x_u_start = u3/z3;
  x_v_start = v3/z3;
  x_z_start = 1.0f/z3;

  // Draw bottom triangle

  tex_color = FColor {};               // texture and its light color
  total_color = FColor {};             //  to interpolate it inside x row loop

  for (int y = y_bot; y < y_top; ++y)
  {
    x_lhs += dx_lhs;
    x_rhs += dx_rhs;

    // Compute differentials of texture coords on the left and right edges

    int dy = y - y3;                        // we need real dy, not clipped

    float x_lu = x_u_start + (dx_lu * dy);  // find tex_coords on the edges
    float x_lv = x_v_start + (dx_lv * dy);
    float x_ru = x_u_start + (dx_ru * dy);
    float x_rv = x_v_start + (dx_rv * dy);

    // Compute differentials of 1/z coords on the left and right edges

    float x_lz = x_z_start + (dx_lz * dy);
    float x_rz = x_z_start + (dx_rz * dy);

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
    float dx_currx_z {};
    FColor dx_currx_c {c3};    

    if ((xrb - xlb) != 0)
    {
      dx_currx_u = (x_ru - x_lu) / (xrb - xlb);
      dx_currx_v = (x_rv - x_lv) / (xrb - xlb);
      dx_currx_z = (x_rz - x_lz) / (xrb - xlb);
      dx_currx_c = (x_rc - x_lc) / (xrb - xlb);      
    }

    // Clip left and right

    xlb = std::max(0, xlb);               // clip left and right lines
    xrb = std::min(scr_width - 1, xrb);
    
    // Interpolate texture coordinate for each pixel
    
    for (int x = xlb; x < xrb; ++x)        // for each pixel interpolate color
    {
      int dx = x - xlb + xl_dx;
      float curr_z = x_lz + (dx_currx_z * dx);
      FColor curr_c = x_lc + (dx_currx_c * dx);      
      
      if (curr_z > z_ptr[y * z_width + x])
      {
        // Get real texture coordinates
        
        float curr_u = x_lu + (dx_currx_u * dx);
        float curr_v = x_lv + (dx_currx_v * dx);
        int u = std::abs((int)(curr_u / curr_z));   // (int) - fast floor
        int v = std::abs((int)(curr_v / curr_z));

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
        z_ptr[y * z_width + x] = curr_z;
        ++total_drawn;
      }
    }
  }
  return total_drawn;
}

} // namespace anshub