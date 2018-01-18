// *************************************************************
// File:    fx_polygons.cc
// Descr:   polygon helpers functions 
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#include "fx_polygons.h"

namespace anshub {

void polygon::CheckInvariant(const Vertexes& p)
{
  if (p.size() < 2)
    throw MathExcept("X or Y coords less than 2");
}

double polygon::Square(const Vertexes& p)
{
  CheckInvariant(p);
  
  double summ {0};
  int sz = (int)p.size();
  for (int i = 0, j = sz-1; i < sz; j = i++) // i - next, j - prev
    summ += (p[j].x * p[i].y - p[i].x * p[j].y);
  return summ/2;
}

Point polygon::Barycenter(const Vertexes& p, double sq)
{
  CheckInvariant(p);
  Point res {};
  int sz = (int)p.size();
  for (int i = 0, j = sz-1; i < sz; j = i++)  // i - next, j - prev
  {
    double summ = (p[j].x * p[i].y - p[i].x * p[j].y);
    res.x += (p[j].x + p[i].x) * summ;
    res.y += (p[j].y + p[i].y) * summ;
  }
  res.x /= 6*sq;
  res.y /= 6*sq;
  return res;
}

// Based on:
// https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html

bool polygon::PointInside(const Vertexes& poly, const Point& p)
{
  int c = 0;
  int sz = (int)poly.size();
  for (int i=0, j=sz-1; i<sz; j=i++) // i - next, j - prev
  {
    Point prev = poly[j];
    Point curr = poly[i];
    double deltaX = prev.x - curr.x;
    double deltaY = prev.y - curr.y;
    double ray = ( deltaX * (p.y-curr.y) / deltaY ) + curr.x;
    bool pnt_beside = (curr.y > p.y) != (prev.y > p.y);

    if (pnt_beside && p.x < ray)
       c = !c;
  }
  return c == 0 ? false : true;
}

bool polygon::PointInside(double x0, double y0, double x1, double y1, double px, double py)
{
  if (px <= x0 || px >= x1 || py <= y0 || py >= y1)
    return false;
  return true;
}

bool polygon::PointInside(const Point& p1, const Point& p2, const Point& p)
{
  if (p.x <= p1.x || p.x >= p1.x || p.y <= p2.y || p.y >= p2.y)
    return false;
  return true;
}

bool polygon::PointsInside(double x0, double y0, double x1, double y1, std::vector<Point>& v)
{
  return polygon::PointsInside(x0, y0, x1, y1, v);
}

bool polygon::PointsInside(double x0, double y0, double x1, double y1, std::vector<Point>&& v)
{
  for (const auto& p : v)
  {
    if (p.x <= x0 || p.x >= x1 || p.y <= y0 || p.y >= y1)
      return false;
  }
  return true;
}

bool polygon::CutConvex(Vertexes& p1, Vertexes& p2, const Line& l)
{
  math_helpers::DummyPushBack(p1);
  Vertexes t1{};
  Vertexes t2{};
  bool in_two {false};
  
  for (auto it = p1.begin(); it != p1.end()-1; ++it)
  {  
    if (!in_two)
      t1.push_back(*it);
    else
      t2.push_back(*it);      
    
    Segment s(*it, *(it+1));
    Point p{};
    if (line::Intersects(l,s,p)) {
      t1.push_back(p);
      t2.push_back(p);
      in_two = !in_two;
    }
  }
  p1 = t1;
  p2 = t2;
  return true;
}

} // namespace anshub