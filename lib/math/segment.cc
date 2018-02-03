// *************************************************************
// File:    segment.cc
// Descr:   represents 2d segment entity
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#include "segment.h"

namespace anshub {

// Returns divide point using ratio

Point segment2d::Divpoint(const Segment& s, float ratio)
{
  return Point(
    s.b.x - (s.b.x - s.a.x) * ratio,
    s.b.y - (s.b.y - s.a.y) * ratio,
    0
  );
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

bool segment2d::Clip(
  int xmin, int ymin, int xmax, int ymax, int& x1, int& y1, int& x2, int& y2)
{
  enum Sides { LEFT, RIGHT, BOTTOM, TOP};

  int vx = x2 - x1; // x component of directing vector
  int vy = y2 - y1; // y component of directing vector
  float t0 = 0;    // with parametr t1 = 0 we have p1
  float t1 = 1;    // with parametr t2 = 1 we have p2
  
  float p;         // inequalities variables
  float q; 
  float t;

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

// Overloaded version of clip function above with float reference arguments

bool segment2d::Clip(
  int xmin, int ymin, int xmax, int ymax, float& x1, float& y1, float& x2, float& y2)
{
  int x1_ = x1;
  int y1_ = y1;
  int x2_ = x2;
  int y2_ = y2;
  bool result = segment2d::Clip(xmin, ymin, xmax, ymax, x1_, y1_, x2_, y2_);
  x1 = x1_;
  y1 = y1_;
  x2 = x2_;
  y2 = y2_;
  return result;
}

}  // namespace anshub