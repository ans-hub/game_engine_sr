// ***************************************************************************
// File:    test_inserting.cc
// Descr:   BHV-tree behavior test using Cathc2 test framework
// Author:  Novoselov Anton @ 2017
// ***************************************************************************

#include <sstream>

#include "3rdparty/catch.hpp"

#include "lib/render/gl_bvh.h"
#include "lib/render/gl_object.h"

#include "lib/math/trig.h"

using namespace anshub;

// Makes instance of object with width == 1.0f

auto MakeObject(const Vector& pos, float fscale, const TrigTable& trig)
{
  auto obj = object::Make(
    "../cube_flat.ply",
    trig, 
    {fscale, fscale, fscale},   // initial scale
    pos,                        // world pos
    {0.0f, 0.0f, 0.0f}          // initial rotate
  );
  return obj;
}

// Creates 1 object in the middle of the world

auto MakeObjectsPack_1_middle()
{
  TrigTable  trig {};
  V_GlObject objs {};

  objs.emplace_back(MakeObject({0.0f, 0.0f, 0.0f},    1.0f,   trig));
  return objs;
}

// Creates 13 big objects with particular position and radius

auto MakeObjectsPack_13_big()
{
  TrigTable  trig {};
  V_GlObject objs {};

  objs.emplace_back(MakeObject({0.0f, 0.0f, 0.0f},    1.0f,   trig));
  objs.emplace_back(MakeObject({-3.5f, 0.0f, 3.5f},   0.5f,   trig));
  objs.emplace_back(MakeObject({-2.5f, 0.0f, 2.5f},   2.0f,   trig));
  objs.emplace_back(MakeObject({2.5f, 0.0f, 3.5f},    1.8f,   trig));
  objs.emplace_back(MakeObject({-3.0f, 0.0f, 0.5f},   1.7f,   trig));
  objs.emplace_back(MakeObject({-1.5f, 0.0f, -1.0f},  0.5f,   trig));
  objs.emplace_back(MakeObject({-2.5f, 0.0f, -2.5f},  0.2f,   trig));

  objs.emplace_back(MakeObject({2.5f, 0.0f, -0.3f},   0.5f,   trig));
  objs.emplace_back(MakeObject({3.5f, 0.0f, -0.3f},   0.4f,   trig));
  objs.emplace_back(MakeObject({2.4f, 0.0f, -0.7f},   0.2f,   trig));
  objs.emplace_back(MakeObject({3.5f, 0.0f, -1.5f},   0.4f,   trig));
  objs.emplace_back(MakeObject({-2.5f, 0.0f, -2.0f},  0.5f,   trig));

  objs.emplace_back(MakeObject({0.5f, 0.0f, -3.5f},   1.2f,   trig));

  return objs;
}

// Creates 9 small objects (y==0) with particular position and radius,
// each other placed onto the edges of 2 cubes

auto MakeObjectsPack_9_small_two()
{
  TrigTable  trig {};
  V_GlObject objs {};

  objs.emplace_back(MakeObject({-3.5f, 0.0f, 3.5f},    0.1f,   trig));
  objs.emplace_back(MakeObject({-1.5f, 0.0f, 3.5f},    0.1f,   trig));
  objs.emplace_back(MakeObject({0.5f, 0.0f, 3.5f},     0.1f,   trig));
  objs.emplace_back(MakeObject({2.5f, 0.0f, 3.5f},     0.1f,   trig));
  objs.emplace_back(MakeObject({-0.5f, 0.0f, 2.5f},    0.1f,   trig));
  objs.emplace_back(MakeObject({-0.5f, 0.0f, 0.5f},    0.1f,   trig));

  objs.emplace_back(MakeObject({-1.5f, 0.0f, -1.5f},   0.1f,   trig));
  objs.emplace_back(MakeObject({-0.5f, 0.0f, -1.5f},   0.1f,   trig));
  objs.emplace_back(MakeObject({2.5f, 0.0f, -1.5f},    0.1f,   trig));

  return objs;
}

// Creates 9 small objects with particular position and radius,
// each other placed into the middle of a cube

auto MakeObjectsPack_9_small_single()
{
  TrigTable  trig {};
  V_GlObject objs {};

  objs.emplace_back(MakeObject({-3.5f, 0.5f, 3.5f},    0.1f,   trig));
  objs.emplace_back(MakeObject({-1.5f, 0.5f, 3.5f},    0.1f,   trig));
  objs.emplace_back(MakeObject({0.5f, 0.5f, 3.5f},     0.1f,   trig));
  objs.emplace_back(MakeObject({2.5f, 0.5f, 3.5f},     0.1f,   trig));
  objs.emplace_back(MakeObject({-0.5f, 0.5f, 2.5f},    0.1f,   trig));
  objs.emplace_back(MakeObject({-0.5f, 0.5f, 0.5f},    0.1f,   trig));

  objs.emplace_back(MakeObject({-1.5f, 0.5f, -1.5f},   0.1f,   trig));
  objs.emplace_back(MakeObject({-0.5f, 0.5f, -1.5f},   0.1f,   trig));
  objs.emplace_back(MakeObject({2.5f, 0.5f, -1.5f},    0.1f,   trig));

  return objs;
}

auto MakeTree_8_3()
{
  constexpr float kWorldSize {8.0f};   // is size of cube side
  constexpr int   kTreeDepth {3};
  return Bvh(kTreeDepth, kWorldSize);
}

TEST_CASE("Insert 1 object in the middle of the world", "[insert]" ) {

  TrigTable  trig {};
  auto tree = MakeTree_8_3();
  auto objs = MakeObjectsPack_1_middle();

  for (auto& obj : objs)
    tree.Insert(obj);

  SECTION("Check what inserted") {

    auto obj = MakeObject({0.0f, 0.0f, 0.0f}, 20.0f, trig);  // more than world
    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);

    REQUIRE(pot.size() == 8);
    REQUIRE(col.size() == 1);

  }

  SECTION("Shouldn`t find") {

    auto obj = MakeObject({-3.5f, 0.0f, -3.5f}, 0.5f, trig);
    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);
    
    REQUIRE(pot.size() == 0);
    REQUIRE(col.size() == 0);
  }

  SECTION("Should find 8-1") {

    auto obj = MakeObject({ -0.5f, 0.0f, -0.5f}, 2.5f, trig);
    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);
    
    REQUIRE(pot.size() == 8);
    REQUIRE(col.size() == 1);
  }

}

TEST_CASE("Insert 13 big objects in the world", "[insert]" ) {

  TrigTable  trig {};
  auto tree = MakeTree_8_3();
  auto objs = MakeObjectsPack_13_big();

  for (auto& obj : objs)
    tree.Insert(obj);

  SECTION("Check what inserted") {

    auto obj = MakeObject({0.0f, 0.0f, 0.0f}, 20.0f, trig);  // more than world
    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);

    REQUIRE(pot.size() == 102);
    REQUIRE(col.size() == 13);

  }

}

TEST_CASE("Insert 13 small objects (each other in two cubes)", "[insert]" ) {

  TrigTable  trig {};
  auto tree = MakeTree_8_3();
  auto objs = MakeObjectsPack_9_small_two();

  for (auto& obj : objs)
    tree.Insert(obj);

  SECTION("Check what inserted") {

    auto obj = MakeObject({0.0f, 0.0f, 0.0f}, 20.0f, trig);  // more than world
    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);

    REQUIRE(pot.size() == 18);    // objs have y == 0.0f, thus each into the two cubes
    REQUIRE(col.size() == 9);

  }

}

TEST_CASE("Insert 13 small objects (each in single cube", "[insert]" ) {

  TrigTable  trig {};
  auto tree = MakeTree_8_3();
  auto objs = MakeObjectsPack_9_small_single();

  for (auto& obj : objs)
    tree.Insert(obj);

  SECTION("Check what inserted") {

    auto obj = MakeObject({0.0f, 0.0f, 0.0f}, 20.0f, trig);  // more than world
    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);

    REQUIRE(pot.size() == 9);    // objs have y == 0.0f, thus each into the two cubes
    REQUIRE(col.size() == 9);

  }

  auto obj = MakeObject({-0.5f, 0.0f, -0.5f}, 2.5f, trig);

  SECTION("Check intersection with middle object") {

    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);

    REQUIRE(pot.size() == 3);
    REQUIRE(col.size() == 2);

  }

  obj.world_pos_.y += 4.0f;

  SECTION("Check collision with those object but upper than it was") {

    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);

    REQUIRE(pot.size() == 0);
    REQUIRE(col.size() == 0);

  }  

  obj.world_pos_ = Vector{-0.5f, 0.0f, 3.5f};

  SECTION("Check collision with those object but placed in another place") {

    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);

    REQUIRE(pot.size() == 3);
    REQUIRE(col.size() == 3);

  }

  SECTION("Check collision with those object but less narrowed") {

    auto obj = MakeObject({-0.5f, 0.5f, 3.5f}, 0.9f, trig);    
    auto pot = tree.FindPotential(obj);
    auto col = tree.FindCollision(obj);

    REQUIRE(pot.size() == 3);
    REQUIRE(col.size() == 0);

  }

}