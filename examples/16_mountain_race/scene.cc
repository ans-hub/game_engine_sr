// *************************************************************
// File:    scene.cc
// Descr:   scene class for mountain race game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "scene.h"

namespace anshub {

Scene::Scene(const Config&, GlWindow& win, Level& level)
  : win_{win}
  , level_{level}
  , tris_base_{triangles::MakeBaseContainer(0)}
  , tris_sky_{triangles::MakeBaseContainer(0)}
  , tris_ptrs_{triangles::MakePtrsContainer(0)}
  , hidden_surfaces_{0}
  , objects_culled_{0}
  , triangles_culled_{0}
{ }

// Builds scenes for further rendering

void Scene::Build()
{
  hidden_surfaces_ = 0;
  objects_culled_ = 0;
  triangles_culled_ = 0;
  
  auto& cam_curr  = level_.camman_.GetCurrentCamera();
  auto  cam_state = level_.camman_.GetState(CamState::WIRED_MODE);

  BuildPlayer(cam_curr);
  BuildSkybox(cam_curr);
  BuildWater(cam_curr);
  BuildNature(cam_curr);
  BuildRain(cam_curr);
  BuildTerrain(cam_curr);

  MakeTriangles();
  ProcessTriangles(cam_curr);

  level_.render_ctx_.is_wired_ = cam_state;
  render::Context(tris_ptrs_, level_.render_ctx_);
}

// Builds player

void Scene::BuildPlayer(const GlCamera& cam)
{
  object::Translate(level_.player_, level_.player_.world_pos_);
  object::ResetAttributes(level_.player_);
  object::ComputeFaceNormals(level_.player_, true);
  hidden_surfaces_ += object::RemoveHiddenSurfaces(level_.player_, cam);
}

// Builds skybox

void Scene::BuildSkybox(const GlCamera& cam)
{
  level_.skybox_.world_pos_ = cam.vrp_;
  level_.skybox_.SetCoords(Coords::TRANS);
  level_.skybox_.CopyCoords(Coords::LOCAL, Coords::TRANS);
  object::Translate(level_.skybox_, level_.skybox_.world_pos_);
  object::ResetAttributes(level_.skybox_);
  hidden_surfaces_ += object::RemoveHiddenSurfaces(level_.skybox_, cam);
  light::Object(level_.skybox_, level_.lights_);
  object::World2Camera(level_.skybox_, cam, level_.trig_);
}

// Builds water

void Scene::BuildWater(const GlCamera& cam)
{
  level_.water_.SetCoords(Coords::TRANS);
  level_.water_.CopyCoords(Coords::LOCAL, Coords::TRANS);
  object::Translate(level_.water_, level_.water_.world_pos_);
  object::ResetAttributes(level_.water_);
  object::ComputeFaceNormals(level_.water_, true);
  object::RemoveHiddenSurfaces(level_.water_, cam);
  object::CullZ(level_.water_, cam, level_.trig_);
  object::CullX(level_.water_, cam, level_.trig_);
  object::CullY(level_.water_, cam, level_.trig_);
  object::VerticesNormals2Camera(level_.water_, cam, level_.trig_);
}

// Builds nature

void Scene::BuildNature(const GlCamera& cam)
{
  auto& trees = level_.trees_.GetObjects();
  
  for (auto& obj : trees) {
    obj.SetCoords(Coords::TRANS);
    obj.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::Translate(obj, obj.world_pos_);
    object::ResetAttributes(obj);
    object::ComputeFaceNormals(obj, true);
    object::RemoveHiddenSurfaces(obj, cam);
    objects_culled_ += object::CullZ(obj, cam, level_.trig_);
    objects_culled_ += object::CullX(obj, cam, level_.trig_);
    objects_culled_ += object::CullY(obj, cam, level_.trig_);
  }
  
  auto& nature = level_.nature_.GetObjects();

  for (auto& obj : nature) {
    obj.SetCoords(Coords::TRANS);
    obj.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::Translate(obj, obj.world_pos_);
    object::ResetAttributes(obj);
    object::ComputeFaceNormals(obj, true);
    object::RemoveHiddenSurfaces(obj, cam);
    objects_culled_ += object::CullZ(obj, cam, level_.trig_);
    objects_culled_ += object::CullX(obj, cam, level_.trig_);
    objects_culled_ += object::CullY(obj, cam, level_.trig_);
  }
}

// Builds rain

void Scene::BuildRain(const GlCamera& cam)
{
  auto& blobs = level_.rain_.GetObjects();
  for (auto& blob : blobs)
  {
    blob.SetCoords(Coords::TRANS);      
    blob.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(blob);
    object::ComputeFaceNormals(blob, true);
    object::RemoveHiddenSurfaces(blob, cam);
    object::Translate(blob, blob.world_pos_);
    object::CullZ(blob, cam, level_.trig_);
    object::CullX(blob, cam, level_.trig_);
    object::CullY(blob, cam, level_.trig_);
  } 
}

// Builds terrain

void Scene::BuildTerrain(const GlCamera& cam)
{ 
  auto& chunks = level_.terrain_.GetChunks();
  for (auto& chunk : chunks)
  {
    object::ResetAttributes(chunk);
    objects_culled_ += object::CullZ(chunk, cam, level_.trig_);
    objects_culled_ += object::CullX(chunk, cam, level_.trig_);
    objects_culled_ += object::CullY(chunk, cam, level_.trig_);
  }
  
  for (auto& chunk : chunks)
  {
    if (!chunk.active_)
      continue;

    chunk.CopyCoords(Coords::LOCAL, Coords::TRANS);
    chunk.SetCoords(Coords::TRANS);

    object::ComputeFaceNormals(chunk, true);
    hidden_surfaces_ += object::RemoveHiddenSurfaces(chunk, cam);
    object::VerticesNormals2Camera(chunk, cam, level_.trig_);
  }
}

// Makes triangles from objects

void Scene::MakeTriangles()
{
  tris_base_.resize(0);
  tris_ptrs_.resize(0);
  tris_sky_.resize(0);
  
  auto& chunks = level_.terrain_.GetChunks();
  for (auto& chunk : chunks)
  {
    if (chunk.active_)
      triangles::AddFromObject(chunk, tris_base_);
  }

  auto& blobs = level_.rain_.GetObjects();
  for (auto& blob : blobs)
  {
    if (blob.active_)
      triangles::AddFromObject(blob, tris_base_);
  }
  triangles::AddFromObjects(level_.trees_.GetObjects(), tris_base_);
  triangles::AddFromObjects(level_.nature_.GetObjects(), tris_base_);

  triangles::AddFromObject(level_.water_, tris_base_);
  triangles::AddFromObject(level_.player_, tris_base_);
}

// Processes triangles throught render pipeline

void Scene::ProcessTriangles(const GlCamera& cam)
{
  triangles::World2Camera(tris_base_, cam, level_.trig_);
  triangles_culled_ += triangles::CullAndClip(tris_base_, cam);
  triangles::ComputeNormals(tris_base_);
  
  // Convert all lights but point to camera coordinates (since point is car`s)

  light::World2Camera(level_.lights_, cam, level_.trig_);
  if (!level_.lights_.point_.empty())
    level_.lights_.point_.front().Reset();

  light::Triangles(tris_base_, level_.lights_);
  light::Reset(level_.lights_);

  // Make triangles for skybox (we want light it sepearately)

  triangles::AddFromObject(level_.skybox_, tris_sky_);
  triangles_culled_ += triangles::CullAndClip(tris_sky_, cam);
  triangles::AddFromTriangles(tris_sky_, tris_base_);

    // Triangles merging

  triangles::MakePointers(tris_base_, tris_ptrs_);
  triangles::SortZAvgCounting(tris_ptrs_, cam.z_far_);

  triangles::Camera2Persp(tris_base_, cam);
  triangles::Persp2Screen(tris_base_, cam);
}

} // namespace anshub