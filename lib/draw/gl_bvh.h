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
#include "lib/draw/gl_aliases.h"

namespace anshub {

//****************************************************************************
// Online binary volume hierarchy tree with spheres as bounding volumes
//****************************************************************************

// Node of binary volume hierarchy tree

struct Node
{
  Node(const Vector& pos, float radius);

  void  AddObject(const GlObject* obj) { objects_.push_back(obj); }
  bool  HasChildren() const { return !children_.empty(); }
  void  CreateChildren();
  bool  IsIntersects(const GlObject*);
  auto& GetChildren() { return children_; }
  auto& GetObjects() { return objects_; }

private:
  Vector pos_;
  float  radius_;
  std::vector<const GlObject*> objects_;
  std::vector<Node*> children_;

}; // struct Node

// Binary volume hierarchy tree. Stores nodes with pointer to objects

struct Bvh
{
  Bvh(int depth, float world_radius);
  ~Bvh();

  void Insert(const GlObject&);
  std::vector<const GlObject*> FindPotential(const GlObject&);
  std::vector<const GlObject*> FindCollision(const GlObject&);

private:
  Node* root_;
  int depth_;         // 0 - one level, 1 - two levels, etc...
  int count_;

  void Clear(Node*);
  void Insert(const GlObject*, Node*, int depth, int max_depth);
  void TraversePotential(const GlObject*, std::vector<const GlObject*>&, Node*);
  void TraverseCollision(const GlObject*, std::vector<const GlObject*>&, Node*);

}; // struct Bvh

// Bvh helpers

namespace bvh_helpers {

  bool IsIntersects(cVector& a_pos, cVector& b_pos, float a_rad, float b_rad);
  bool IsIntersects(const GlObject&, const GlObject&);

} // namespace bvh_helpers

} // namespace anshub

#endif // GL_BVH_H

// Note: 
//  useful: http://www.miguelcasillas.com/?mcportfolio=collision-detection-c