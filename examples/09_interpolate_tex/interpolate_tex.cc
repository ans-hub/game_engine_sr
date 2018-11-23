// *************************************************************
// File:    interpolate_tex.cc
// Descr:   interpolate tex coords
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include <iostream>
#include <cmath>
#include <cstdlib>

int main(int argc, const char** argv)
{ 
  if (argc != 7)
  {
    std::cerr << "Input format (float): y1 z1 u1 y2 z2 u2" << '\n';
    return 1;
  }
  
  float y1 = std::atof(argv[1]);
  float z1 = std::atof(argv[2]);
  float u1 = std::atof(argv[3]);
  float y2 = std::atof(argv[4]);
  float z2 = std::atof(argv[5]);
  float u2 = std::atof(argv[6]);

  if (y1 > y2)
  {
    std::swap(y1, y2);
    std::swap(z1, z2);
    std::swap(u1, u2);
  }

  float dy = y2 - y1;
  float du = u2/z2 - u1/z1;
  float dz = 1/z2 - 1/z1;

  float dudy = du / dy;   // step for u/z coord
  float dzdy = dz / dy;   // step for 1/z coord

  float curr_u = u1 / z1;
  float curr_z = 1 / z1;

  for (int i = (int)y1; i <= (int)y2; ++i)
  {
    std::cout << "y = " << i
      << ", u/z = " << curr_u
      << ", 1/z = " << curr_z
      << ", real_u = " << curr_u / curr_z << '\n';
    curr_u += dudy;
    curr_z += dzdy;
  }

  return 0;
}