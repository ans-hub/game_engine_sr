// *************************************************************
// File:    polygon.cc
// Descr:   represents polygon 2d and helpers 
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "polygon.h"

namespace anshub {

void polygon2d::CheckInvariant(const Vertices& p)
{
  if (p.size() < 2)
    throw MathExcept("X or Y coords less than 2");
}

float polygon2d::Square(const Vertices& p)
{
  CheckInvariant(p);
  
  float summ {0};
  int sz = (int)p.size();
  for (int i = 0, j = sz-1; i < sz; j = i++) // i - next, j - prev
    summ += (p[j].x * p[i].y - p[i].x * p[j].y);
  return summ/2;
}

Point polygon2d::Barycenter(const Vertices& p, float sq)
{
  CheckInvariant(p);
  Point res {};
  int sz = (int)p.size();
  for (int i = 0, j = sz-1; i < sz; j = i++)  // i - next, j - prev
  {
    float summ = (p[j].x * p[i].y - p[i].x * p[j].y);
    res.x += (p[j].x + p[i].x) * summ;
    res.y += (p[j].y + p[i].y) * summ;
  }
  res.x /= 6*sq;
  res.y /= 6*sq;
  return res;
}

// Based on:
// https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html

bool polygon2d::PointInside(const Vertices& poly, const Point& p)
{
  int c = 0;
  int sz = (int)poly.size();
  for (int i=0, j=sz-1; i<sz; j=i++) // i - next, j - prev
  {
    Point prev = poly[j];
    Point curr = poly[i];
    float deltaX = prev.x - curr.x;
    float deltaY = prev.y - curr.y;
    float ray = ( deltaX * (p.y-curr.y) / deltaY ) + curr.x;
    bool pnt_beside = (curr.y > p.y) != (prev.y > p.y);

    if (pnt_beside && p.x < ray)
       c = !c;
  }
  return c == 0 ? false : true;
}

bool polygon2d::PointInside(
  float x0, float y0, float x1, float y1, float px, float py)
{
  if (px < x0 || px > x1 || py < y0 || py > y1)
    return false;
  return true;
}

bool polygon2d::PointInside(const Point& p1, const Point& p2, const Point& p)
{
  if (p.x < p1.x || p.x > p1.x || p.y < p2.y || p.y > p2.y)
    return false;
  return true;
}

bool polygon2d::PointsInside(
  float x0, float y0, float x1, float y1, std::vector<Point>& v)
{
  return polygon2d::PointsInside(x0, y0, x1, y1, v);
}

bool polygon2d::PointsInside(
  float x0, float y0, float x1, float y1, std::vector<Point>&& v)
{
  for (const auto& p : v)
  {
    if (p.x < x0 || p.x > x1 || p.y < y0 || p.y > y1)
      return false;
  }
  return true;
}

bool polygon2d::CutConvex(Vertices& p1, Vertices& p2, const Line& l)
{
  math_helpers::DummyPushBack(p1);
  Vertices t1{};
  Vertices t2{};
  bool in_two {false};
  
  for (auto it = p1.begin(); it != p1.end()-1; ++it)
  {  
    if (!in_two)
      t1.push_back(*it);
    else
      t2.push_back(*it);      
    
    Segment s(*it, *(it+1));
    Point p{};
    if (line2d::Intersects(l,s,p)) {
      t1.push_back(p);
      t2.push_back(p);
      in_two = !in_two;
    }
  }
  p1 = t1;
  p2 = t2;
  return true;
}

// Rotates point (by origin) (https://goo.gl/2y3sNZ)
//  Cx = Ax + (Bx−Ax)*cos⁡α − (By−Ay)*sin⁡α
//  Cy = Ay + (Bx−Ax)*sinα + (By−Ay)*cosα
// where A - origin, B - old point, C - new point

void polygon2d::RotatePoint(float& x, float& y, float th, const TrigTable& angles)
{
  float sin_theta = angles.Sin(th);;
  float cos_theta = angles.Cos(th);
  x = x * cos_theta - y * sin_theta;
  y = y * sin_theta + y * cos_theta;
}

} // namespace anshub