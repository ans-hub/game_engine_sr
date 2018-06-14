// ***************************************************************************
// File:    bvh_test.cc
// Descr:   BHV-tree test
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// ***************************************************************************

#include <vector>

#include "lib/draw/gl_bvh.h"
#include "lib/draw/gl_object.h"

#include "lib/system/rand_toolkit.h"

#include "lib/math/trig.h"

using namespace anshub;
using anshub::vector::operator<<;

const char* HandleInput(int argc, const char** argv)
{
  if (argc != 2)
    return nullptr;
  else
    return argv[1];
}

// Makes one object with 

// Makes cnt objects randomly using given width, depth and height

auto MakeObjects(const char* fname, int cnt, float world_rad, float max_scale)
{
  TrigTable trig {};                  // creates object and make cnt of objects
  auto obj = object::Make(
    fname, trig, 
    {1.0f, 1.0f, 1.0f},   // initial scale
    {0.0f, 0.0f, 0.0f},   // world pos
    {0.0f, 0.0f, 0.0f}    // initial rotate
  );
  V_GlObject objs (cnt, obj);

  for (auto& obj : objs)              // randomly place and scale all objects 
  {
    // Make random world pos

    float rad = world_rad / 2;
    obj.world_pos_.x = rand_toolkit::get_rand(-rad, rad);
    obj.world_pos_.y = rand_toolkit::get_rand(-rad, rad);
    obj.world_pos_.z = rand_toolkit::get_rand(-rad, rad);
    
    // Make random scaling

    float sfactor  = rand_toolkit::get_rand(1.0f, max_scale);
    object::Scale(obj, {sfactor, sfactor, sfactor});
    obj.sphere_rad_ = object::FindFarthestCoordinate(obj);
  }
  return objs;
}

int main(int argc, const char** argv)
{
  // Process file name

  auto fname = HandleInput(argc, argv);
  if (!fname) {
    std::cerr << "Incorrect file name\n";
    return 1;
  }

  // World constants

  constexpr int kTreeDepth {5};
  constexpr float kWorldRadius {100.0f};
  constexpr int kObjsCount {1000};
  constexpr float kMaxScale {3.0f};

  // Make world objects and choose back as main object

  rand_toolkit::start_rand();
  auto other = MakeObjects(fname, kObjsCount, kWorldRadius, kMaxScale);
  auto main  = other.back();
  other.pop_back();

  // Create bvh-tree and insert objects there

  Bvh tree (kTreeDepth, kWorldRadius);
  for (auto& obj : other)
    tree.Insert(obj);

  // Detect collisions between main object and other objects

  auto potential = tree.FindPotential(main);
  auto collided  = tree.FindCollision(main);

  std::cout << "Main: " << main.world_pos_ << ' ' << main.sphere_rad_ << '\n';
  std::cout << "P: " << potential.size() << ", C: " << collided.size() << '\n';

  for (auto* obj : collided)
    std::cout << obj->world_pos_ << ' ' << obj->sphere_rad_ << '\n';

  return 0;
}