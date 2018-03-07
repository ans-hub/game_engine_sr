// *************************************************************
// File:    gl_rasterize_tri.cc
// Descr:   rasterizes triangles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_rasterize_tri.h"

namespace anshub {

// Draws solid triangle. First, we should guarantee that y1 is most top, and
// y3 is most bottom points. Then we draw top triangle from top to middle,
// and then draw from bottom to the middle

void draw::SolidTriangle(
  float px1, float py1, float px2, float py2, float px3, float py3, uint color, Buffer& buf)
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

// Draws textured triangle without lighting

void draw::TexturedTriangle(
  Bitmap* bmp, cVector& p1, cVector& p2, cVector& p3,
  cVector& t1, cVector& t2, cVector& t3, Buffer& buf)
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
      
      draw::Point(x, y, color::MakeARGB(255, r, g, b), buf);
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
      
      draw::Point(x, y, color::MakeARGB(255, r, g, b), buf);
    }
  }
}

// Draws textured triangle with flat lighting

void draw::TexturedTriangleFlatLight(
  Bitmap* bmp, cVector& p1, cVector& p2, cVector& p3,
  cVector& t1, cVector& t2, cVector& t3, uint color, Buffer& buf)
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
      
      // Modulate light and color

      FColor total {color};
      FColor light {r, g, b};
      total *= light;
      total /= 256.0f;
      
      draw::Point(x, y, total.GetARGB(), buf);
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
      
      // Modulate light and color
      
      FColor total {color};
      FColor light {r, g, b};
      total *= light;
      total /= 256.0f;
      
      draw::Point(x, y, total.GetARGB(), buf);
    }
  }
}

} // namespace anshub