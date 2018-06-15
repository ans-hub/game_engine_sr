// ***************************************************************************
// File:    gl_bvh.cc
// Descr:   binary volume hierarchy tree
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// ***************************************************************************

#include "gl_bvh.h"

namespace anshub {

// Creates bhv-tree with max-radius root node placed in the middle of the world
// World size is the size of world cube side

Bvh::Bvh(int depth, float world_size)
  : root_{new Node({0.0f, 0.0f, 0.0f}, world_size)}
  , depth_{depth}
  , objects_cnt_{0}
  , octants_cnt_{1} // at the start world cube is one big octant
  , nodes_visits_{0}
{ }

// Destroys all nodes

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

// Inserts object's pointer in bvh-tree (public)

void Bvh::Insert(GlObject& obj)
{
  Insert(&obj, root_, 0, depth_);
}

// Inserts object's pointer into the leaf (private)  

void Bvh::Insert(GlObject* obj, Node* node, int curr_depth, int depth)
{
  if (!node->AreIntersect(obj))
    return;

  if (curr_depth == depth)  // we are in leaf
  {
    node->AddObject(obj);
    ++objects_cnt_;
  }
  else if (!node->HasChildren())
    octants_cnt_ += node->CreateChildren();

  for (auto* child : node->GetChildren())
    Insert(obj, child, curr_depth + 1, depth);
}

// Returns object`s pointers from leafs where obj might be placed (public)

std::vector<GlObject*> Bvh::FindPotential(const GlObject& obj)
{
  std::vector<GlObject*> objs {};
  nodes_visits_ = 0;
  TraversePotential(&obj, objs, root_);
  return objs;
}

// Returns pointers to objects which are collided with obj (public)

std::vector<GlObject*> Bvh::FindCollision(const GlObject& obj)
{
  std::vector<GlObject*> objs {};
  nodes_visits_ = 0;
  TraverseCollision(&obj, objs, root_);
  return objs;
}

// Taverses tree to determine objects which is potentially collided with
// object (private)

void Bvh::TraversePotential(const GlObject* obj, 
                            std::vector<GlObject*>& objs, Node* node)
{
  if (!node->AreIntersect(obj))
    return;

  auto& add = node->GetObjects();
  objs.insert(objs.end(), add.begin(), add.end());
 
  for (auto* child : node->GetChildren())
  {
    TraversePotential(obj, objs, child);
    ++nodes_visits_;
  }
}

// Traverses tree to determine objects which is really collided with an
// object. All checked objects marks as checked (private)

void Bvh::TraverseCollision(const GlObject* main, 
                            std::vector<GlObject*>& objs, Node* node)
{
  if (!node->AreIntersect(main))
    return;

  for (auto* obj : node->GetObjects())
  {
    if (!object::GetAuxFlag(*obj, AuxFlags::COLLIDED) && 
        bvh_helpers::AreCollided(*main, *obj))
    {
      object::SetAuxFlag(*obj, AuxFlags::COLLIDED);
      objs.push_back(obj);
    }
  }
 
  for (auto* child : node->GetChildren())
  {
    TraverseCollision(main, objs, child);
    ++nodes_visits_;
  }    
}

// Creates bvh-tree`s node

Node::Node(const Vector& pos, float cube_side)
  : pos_{pos}
  , cube_side_{cube_side}
  , sphere_rad_{(1.73f*cube_side_)/2}   // cube diameter / 2
{ }

// Checks intersects between bounding sphere of node and bounding sphere
// of object

bool Node::AreIntersect(const GlObject* obj)
{
  auto a_rad = sphere_rad_;
  auto a_pos = pos_;
  auto b_rad = obj->sphere_rad_;
  auto b_pos = obj->world_pos_;

  return bvh_helpers::AreCollided(a_pos, b_pos, a_rad, b_rad);
}

// Creates 8 children by dividing current node in 8 octants

int Node::CreateChildren()
{
  float sz = cube_side_ / 2;    // new side size
  float st = sz / 2;            // step to compute pos of cube

  children_.push_back(new Node({pos_.x-st, pos_.y+st, pos_.z+st}, sz));
  children_.push_back(new Node({pos_.x+st, pos_.y+st, pos_.z+st}, sz));
  children_.push_back(new Node({pos_.x-st, pos_.y+st, pos_.z-st}, sz));
  children_.push_back(new Node({pos_.x+st, pos_.y+st, pos_.z-st}, sz));
  children_.push_back(new Node({pos_.x-st, pos_.y-st, pos_.z+st}, sz));
  children_.push_back(new Node({pos_.x+st, pos_.y-st, pos_.z+st}, sz));
  children_.push_back(new Node({pos_.x-st, pos_.y-st, pos_.z-st}, sz));
  children_.push_back(new Node({pos_.x+st, pos_.y-st, pos_.z-st}, sz));
  
  return 8;   // 8 octants was added
}

// Checks if two spheres intersect (collide)

bool bvh_helpers::AreCollided(const Vector& a_pos, const Vector& b_pos,
                              float a_rad, float b_rad)
{
  auto dx = a_pos - b_pos;
  auto dx_len = dx.SquareLength();
  auto ab_len = std::pow(a_rad + b_rad, 2);

  return dx_len <= ab_len;
}


// Checks if two objects intersects (by their bounding circumscribed spheres)

bool bvh_helpers::AreCollided(const GlObject& obj1, const GlObject& obj2)
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