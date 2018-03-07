// *************************************************************
// File:    gl_rasterize_tri_z.cc
// Descr:   rasterizes triangles with 1/z buffer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_rasterize_tri_z.h"

namespace anshub {

// Draws textured triangle without lighting, but with correct perspective
// using 1/z interpolating

void draw::TexturedTriangle(
  cVertex& p1, cVertex& p2, cVertex& p3, Bitmap* bmp, ZBuffer& zbuf, Buffer& buf)
{
  // Convert float to int for vertex positions

  int x1 = std::floor(p1.pos_.x);
  int x2 = std::floor(p2.pos_.x);
  int x3 = std::floor(p3.pos_.x);
  int y1 = std::floor(p1.pos_.y);
  int y2 = std::floor(p2.pos_.y);
  int y3 = std::floor(p3.pos_.y);

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

  if (y1 < 0 || y3 >= buf.Height())       // if triangle is full out of screen
    return;

  int y_top_clip = y1 - buf.Height() + 1; // how much pixels is clipped 
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

    int xlb = std::floor(x_lhs);          // xlb - x left border
    int xrb = std::ceil(x_rhs);
    
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
    xrb = std::min(buf.Width() - 1, xrb);

    // Interpolate texture coordinate for each pixel

    for (int x = xlb; x < xrb; ++x)
    {
      int dx = x - xlb + xl_dx;           // we need real dx, not clipped
      float curr_z = x_lz + (dx_currx_z * dx);
      
      if (curr_z > zbuf(x,y))
      {
        float curr_u = x_lu + (dx_currx_u * dx);
        float curr_v = x_lv + (dx_currx_v * dx);
        byte r {0};
        byte g {0};
        byte b {0};
        bmp->get_pixel(
          std::floor(std::abs(curr_u/curr_z)),
          std::floor(std::abs(curr_v/curr_z)),
          r, g, b);
        draw::Point(x, y, color::MakeARGB(255, r, g, b), buf);
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
  y_top = std::min(y2, buf.Height()-1);

  // Prepare start values of interpolants

  x_u_start = u3/z3;
  x_v_start = v3/z3;
  x_z_start = 1.0f/z3;

  // Draw bottom triangle

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

    int xlb = std::floor(x_lhs);          // xlb - x left border
    int xrb = std::ceil(x_rhs);

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
    xrb = std::min(buf.Width() - 1, xrb);
    
    // Interpolate texture coordinate for each pixel
    
    for (int x = xlb; x < xrb; ++x)        // for each pixel interpolate color
    {
      int dx = x - xlb + xl_dx;
      float curr_z = x_lz + (dx_currx_z * dx);
      if (curr_z > zbuf(x,y))
      {
        float curr_u = x_lu + (dx_currx_u * dx);
        float curr_v = x_lv + (dx_currx_v * dx);
        byte r {0};
        byte g {0};
        byte b {0};
        bmp->get_pixel(
          std::floor(std::abs(curr_u/curr_z)),
          std::floor(std::abs(curr_v/curr_z)),
          r, g, b);
        draw::Point(x, y, color::MakeARGB(255, r, g, b), buf);
        zbuf(x,y) = curr_z;
      }
    }
  }
}

} // namespace anshub