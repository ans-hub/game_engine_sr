// ***************************************************************************
// File:    gl_bvh.h
// Descr:   binary volume hierarchy tree
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// ***************************************************************************

#ifndef GL_BVH_H
#define GL_BVH_H

#include <vector>

#include "lib/draw/gl_object.h"
#include "lib/draw/gl_enums.h"
#include "lib/draw/gl_aliases.h"

namespace anshub {

//****************************************************************************
// Online binary volume hierarchy tree with circimsribes spheres as bounding
// volumes
//****************************************************************************

// Node of binary volume hierarchy tree. Represents a cube with a
// circumscribed sphere

struct Node
{
  Node(const Vector& pos, float cube_side);

  int   CreateChildren();
  void  AddObject(GlObject* obj) { objects_.push_back(obj); }
  
  bool  AreIntersect(const GlObject*);
  bool  HasChildren() const { return !children_.empty(); }
  auto& GetChildren() const { return children_; }
  auto& GetObjects() const { return objects_; }

private:
  Vector pos_;          // world pos
  float  cube_side_;    // side of cube
  float  sphere_rad_;   // radius of circumscribed sphere
  
  std::vector<Node*>     children_;
  std::vector<GlObject*> objects_;

}; // struct Node

// Binary volume hierarchy tree. Stores nodes with pointer to objects

struct Bvh
{
  Bvh(int depth, float world_radius);
  ~Bvh();

  void Insert(GlObject&);
  std::vector<GlObject*> FindPotential(const GlObject&);
  std::vector<GlObject*> FindCollision(const GlObject&);

private:
  Node* root_;
  int depth_;          // 0 - one level, 1 - two, etc...
  int objects_cnt_;
  int octants_cnt_;

  void Clear(Node*);
  void Insert(GlObject*, Node*, int depth, int max_depth);
  void TraversePotential(const GlObject*, std::vector<GlObject*>&, Node*);
  void TraverseCollision(const GlObject*, std::vector<GlObject*>&, Node*);

}; // struct Bvh

// Bvh helpers

namespace bvh_helpers {

  bool AreCollided(cVector& a_pos, cVector& b_pos, float a_rad, float b_rad);
  bool AreCollided(const GlObject&, const GlObject&);

} // namespace bvh_helpers

} // namespace anshub

#endif // GL_BVH_H

// Note: 
//  useful: http://www.miguelcasillas.com/?mcportfolio=collision-detection-c