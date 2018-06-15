// ***************************************************************************
// File:    demo.cc
// Descr:   BHV-tree demo with text representation
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// ***************************************************************************

#include <vector>
#include <stdexcept>

#include "lib/draw/gl_bvh.h"
#include "lib/draw/gl_object.h"

#include "lib/system/rand_toolkit.h"

#include "lib/math/trig.h"

using namespace anshub;
using anshub::vector::operator<<;

// Prints usage

void PrintUsage(std::ostream& oss)
{
  oss << "Usage: ./demo arg1..arg5 \n"
      << "  [fname_path]    - string\n"
      << "  [world_rad]     - float\n"
      << "  [objs_in_world] - int\n"
      << "  [max_scale]     - float\n"
      << "  [tree_depth]    - int\n";
}

// Prints error message and usage

void PrintError(const char* msg, std::ostream&)
{
  std::cerr << msg << '\n';
  PrintUsage(std::cerr);
}

// Stores data inputed from cmd

struct InputData
{
  InputData(int argc, const char** argv);

  const char* fname_;
  float world_radius_;
  int objs_count_;
  float max_scale_;
  int tree_depth_;

}; // struct InputData

// Constructs InputData

InputData::InputData(int argc, const char** argv)
{
  if (argc != 6)
  {
    PrintUsage(std::cerr);
    throw std::runtime_error("Invalid arguments count");
  }
  fname_ = argv[1];
  world_radius_ = atof(argv[2]);
  objs_count_ = atoi(argv[3]);
  max_scale_ = atof(argv[4]);
  tree_depth_ = atoi(argv[5]);
}

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

  InputData args(argc, argv);

  // Make world objects and choose back as main object

  rand_toolkit::start_rand();
  auto other = MakeObjects(
    args.fname_, args.objs_count_, args.world_radius_, args.max_scale_);
  auto main  = other.back();
  other.pop_back();

  // Create bvh-tree and insert objects there

  Bvh tree (args.tree_depth_, args.world_radius_);
  for (auto& obj : other)
    tree.Insert(obj);

  // Detect collisions between main object and other objects

  auto potential = tree.FindPotential(main);
  auto collided  = tree.FindCollision(main);

  // Print results
  
  std::cout << "Main point:"
            << " pos: " << main.world_pos_ << ","
            << " rad: " << main.sphere_rad_ << '\n';
  std::cout << "Potential: " << potential.size() << ", "
            << "Collided:  " << collided.size() << '\n';

  if (!collided.empty())
    std::cout << "List of collided: \n";
  
  for (auto* obj : collided)
    std::cout << obj->world_pos_ << ' ' << obj->sphere_rad_ << '\n';

  return 0;
}