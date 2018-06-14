// ***************************************************************************
// File:    gl_bvh.cc
// Descr:   binary volume hierarchy tree
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// ***************************************************************************

#include "gl_bvh.h"

namespace anshub {

// Creates bhv-tree with max-radius root node placed in the middle of the world

Bvh::Bvh(int depth, float world_radius)
  : root_{new Node({0.0f, 0.0f, 0.0f}, world_radius)}
  , depth_{depth}
  , count_{0}
{ }

// Destroy all nodes

Bvh::~Bvh()
{
  Clear(root_);
}

// Clears bvh-tree

void Bvh::Clear(Node* node)
{
  if (!node->HasChildren())
    return;
  for (auto* node : node->GetChildren())
  {
    Clear(node);
    delete node;
  }
}

// Inserts object`s pointer in bvh-tree

void Bvh::Insert(const GlObject& obj)
{
  Insert(&obj, root_, 0, depth_);
}

// Inserts object`s pointer on node and its children 

void Bvh::Insert(const GlObject* obj, Node* node, int curr_depth, int depth)
{
  if (!node->IsIntersects(obj))
    return;

  if (curr_depth == depth)
  {
    node->AddObject(obj);
    ++count_;
  }
  else if (!node->HasChildren())
    node->CreateChildren();

  for (auto* node : node->GetChildren())
    Insert(obj, node, curr_depth + 1, depth);
}

// Returns object`s pointers from leafs where obj might be placed

std::vector<const GlObject*> Bvh::FindPotential(const GlObject& obj)
{
  std::vector<const GlObject*> objs {};
  TraversePotential(&obj, objs, root_);
  return objs;
}

// Returns pointers to objects which are collided with obj

std::vector<const GlObject*> Bvh::FindCollision(const GlObject& obj)
{
  std::vector<const GlObject*> objs {};
  TraverseCollision(&obj, objs, root_);
  return objs;
}

// Private traversal function to determine objects which is potentially
// collided with object

void Bvh::TraversePotential(const GlObject* obj, 
                            std::vector<const GlObject*>& objs, Node* node)
{
  if (!node->IsIntersects(obj))
    return;

  auto& add = node->GetObjects();
  objs.insert(objs.end(), add.begin(), add.end());
 
  for (auto* node : node->GetChildren())
    TraversePotential(obj, objs, node);
}

// Private traversal function to determine objects which is really
// collided with object

void Bvh::TraverseCollision(const GlObject* main, 
                            std::vector<const GlObject*>& objs, Node* node)
{
  if (!node->IsIntersects(main))
    return;

  for (auto* obj : node->GetObjects())
  {
    if (bvh_helpers::IsIntersects(*obj, *obj))
      objs.push_back(obj);
  }
 
  for (auto* node : node->GetChildren())
    TraversePotential(main, objs, node);
}

// Creates bvh-tree`s node

Node::Node(const Vector& pos, float radius)
  : pos_{pos}
  , radius_{radius}
{ }

// Checks intersects between bounding sphere of node and bounding sphere
// of object

bool Node::IsIntersects(const GlObject* obj)
{
  auto a_rad = radius_;
  auto a_pos = pos_;
  auto b_rad = obj->sphere_rad_;
  auto b_pos = obj->world_pos_;

  return bvh_helpers::IsIntersects(a_pos, b_pos, a_rad, b_rad);
}

// Creates 8 children by dividing current node in 8 octants

void Node::CreateChildren()
{
  auto r = radius_ / 2;
  children_.push_back(new Node({pos_.x-r, pos_.y+r, pos_.z+r}, r));
  children_.push_back(new Node({pos_.x+r, pos_.y+r, pos_.z+r}, r));
  children_.push_back(new Node({pos_.x-r, pos_.y+r, pos_.z-r}, r));
  children_.push_back(new Node({pos_.x+r, pos_.y+r, pos_.z-r}, r));
  children_.push_back(new Node({pos_.x-r, pos_.y-r, pos_.z+r}, r));
  children_.push_back(new Node({pos_.x+r, pos_.y-r, pos_.z+r}, r));
  children_.push_back(new Node({pos_.x-r, pos_.y-r, pos_.z-r}, r));
  children_.push_back(new Node({pos_.x+r, pos_.y-r, pos_.z-r}, r));
}

// Checks if two spheres intersects

bool bvh_helpers::IsIntersects(const Vector& a_pos, const Vector& b_pos,
                               float a_rad, float b_rad)
{
  auto dx = a_pos - b_pos;
  auto dx_len = dx.SquareLength();
  auto ab_len = std::pow(a_rad + b_rad, 2);
  
  return dx_len <= ab_len;
}


// Checks if two objects intersects (by bounding spheres)

bool bvh_helpers::IsIntersects(const GlObject& obj1, const GlObject& obj2)
{
  auto a_rad = obj1.sphere_rad_;
  auto a_pos = obj1.world_pos_;
  auto b_rad = obj2.sphere_rad_;
  auto b_pos = obj2.world_pos_;

  auto dx = a_pos - b_pos;
  auto dx_len = dx.SquareLength();
  auto ab_len = std::pow(a_rad + b_rad, 2);
  
  return dx_len <= ab_len;
}

} // namespace anshub