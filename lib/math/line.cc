// *************************************************************
// File:    line.h
// Descr:   represents 2d line entity on the plane
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#include "line.h"

namespace anshub {

// Return equation of line by given 2 points

Line line2d::EquationA(const Point& p1, const Point& p2)
{
  // todo: check if p1 == p2;
  Line res {};
  res.a = p1.y - p2.y;
  res.b = p2.x - p1.x;
  res.c = -res.a*p1.x - res.b*p1.y;
  return res;
}

// Returns equation of line by given point and perpendicular vector
// A(x-x0) + B(y-y0) + C = 0 for v(A;B) and p(x0,y0)

Line line2d::EquationB(const Point& p, const Vector& v)
{
  Line res {};
  res.a = v.x;
  res.b = v.y;
  res.c = v.x*(-p.x) + v.y*(-p.y);
  return res;
}

// Returns equation of line by give radius-vector

Line line2d::EquationC(const Vector& v)
{
  return line2d::EquationA(Point(0,0,0), Point(v.x, v.y, 0));
}

// Returns perpendicular line to given line in given point

Line line2d::Perpendicular(const Line& l, const Point& p)
{
  Point p1 {l.GetX(0.0), 0.0};
  Point p2 {0.0, l.GetY(0.0)};
  Vector dirv (p2-p1);
  return line2d::EquationB(p, dirv);
}

bool line2d::Intersects(const Segment& l, const Segment& r, Point& res)
{
  Vector v1 {l.b - l.a};
  Vector v2 {r.b - r.a};
  
  float vmul = v1.x*v2.y - v1.y*v2.x;  // vmul as matrix determ
  if (std::fabs(vmul) < math::kEpsilon)
    return false;
  
  float t1 = ( v2.x*(l.a.y - r.a.y) - v2.y*(l.a.x - r.a.x) ) / vmul;
  float t2 = ( v1.x*(l.a.y - r.a.y) - v1.y*(l.a.x - r.a.x) ) / vmul;

  if ((t1 >= 0) && (t1 <= 1) && (t2 >= 0) && (t2 <= 1)){
    res.x = l.a.x + (t1 * v1.x);
    res.y = l.a.y + (t1 * v1.y); 
    return true;
  } 
  else
    return false;
}

bool line2d::Intersects(const Line& l1, const Line& l2, Point& p)
{
  float det = l1.a*l2.b - l1.b*l2.a;
  if (std::fabs(det) < math::kEpsilon)
    return false;
  p.x = -(l1.c*l2.b - l2.c*l1.b) / det;
  p.y = -(l1.a*l2.c - l2.a*l1.c) / det;
  return true;
}

bool line2d::Intersects(const Line& l1, const Segment& s, Point& p)
{
  Line l2 = line2d::EquationA(s.a, s.b);
  if (line2d::Intersects(l1,l2,p))
  {
    float xmax = std::max(s.a.x, s.b.x);
    float xmin = std::min(s.a.x, s.b.x);
    float ymax = std::max(s.a.y, s.b.y);
    float ymin = std::min(s.a.y, s.b.y);
    
    if ( (p.x < xmax || math::Feq(p.x, xmax)) &&
         (p.x > xmin || math::Feq(p.x, xmin)) &&
         (p.y < ymax || math::Feq(p.y, ymax)) &&
         (p.y > ymin || math::Feq(p.y, ymin)) )
        return true;
  }
  return false;
}

} // namespace anshub