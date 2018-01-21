// *************************************************************
// File:    logic.cc
// Descr:   main game logic
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "logic.h"

namespace anshub {

Logic::Logic(GlWindow& win, Level& level, AudioOut& audio)
  : win_{win}
  , level_(level)
  , audio_{audio}
{
  // Preload music in memory

  audio.Load(cfg::kBackgroundMusic, false);
  audio.Load(cfg::kExplodeSnd, false);
  audio.Load(cfg::kShotSnd, false);
  audio.Load(cfg::kWingSnd, false);
  audio.Load(cfg::kEnemySnd, false);
  audio.Play(cfg::kBackgroundMusic);
  
  InitCannon();
  InitStarfield();
  for (auto& ship : level_.ships_)
    InitWarship(ship);
}

bool Logic::Process()
{
  // Process keyboard and events base on it

  auto kbtn = win_.ReadKeyboardBtn(BtnType::KB_DOWN);
  auto mbtn = win_.ReadMouseBtn(BtnType::MS_DOWN);
  auto mpos = win_.ReadMousePos();
  
  // Process level objects

  MoveStarfield();
  MoveWarships();
  ProcessExplosions();
  ProcessEnemyShots();

  if (level_.state_ == GameState::PLAY)
  {
    AttackWarships();
    ProcessCannon(mpos, mbtn);
    ProcessViewport(kbtn);
  }  
  return ProcessGameState(kbtn);
}

void Logic::InitCannon()
{
  level_.cannon_.mid_.x = win_.Width()/2;
  level_.cannon_.mid_.y = win_.Height()/2;
  level_.cannon_.color_ = cfg::kAimColor;
}

void Logic::InitStarfield()
{
  int half_w = win_.Width() >> 1;
  int half_h = win_.Height() >> 1;

  for (auto& star : level_.stars_)
  {
    star.x = rand_toolkit::get_rand(-half_w, half_w);
    star.y = rand_toolkit::get_rand(-half_w, half_w);
    star.z = rand_toolkit::get_rand(cfg::kNearZ, cfg::kStarFarZ);    
  }
}

void Logic::InitWarship(Starship& ship)
{ 
  int half_w = win_.Width() >> 1;
  int half_h = win_.Height() >> 1;

  ship.pos_.x = rand_toolkit::get_rand(-half_w, half_w);
  ship.pos_.y = rand_toolkit::get_rand(-half_h, half_h);
  ship.pos_.z = rand_toolkit::get_rand(cfg::kShipFarZ >> 1, cfg::kShipFarZ);
  ship.vel_.x = rand_toolkit::get_rand(-10, 10);
  ship.vel_.y = rand_toolkit::get_rand(-10, 10);
  ship.vel_.z = rand_toolkit::get_rand(-cfg::kMaxVelocity, -cfg::kMaxVelocity>>1);
  
  ship.color_ = cfg::kSpaceColor;
  ship.dead_ = false;
  ship.audible_ = false;
}

// Todo: move colors changing into the scene class

void Logic::MoveStarfield()
{
  int half_w = win_.Width() >> 1;
  int half_h = win_.Height() >> 1;

  for (auto& star : level_.stars_)
  {
    star.z -= level_.velocity_;
    if (star.z <= cfg::kNearZ) {
      star.x = rand_toolkit::get_rand(-half_w, half_w);
      star.y = rand_toolkit::get_rand(-half_w, half_w);
      star.z = cfg::kStarFarZ;  
    }
  }
}

void Logic::MoveWarships()
{
  for (auto& ship : level_.ships_)
  {
    if (ship.dead_)
      continue;

    ship.pos_.x += ship.vel_.x;
    ship.pos_.y += ship.vel_.y;
    ship.pos_.z += ship.vel_.z - level_.velocity_;
    
    int audible_dist = cfg::kNearZ + cfg::kAudibleShip;
    if (ship.pos_.z <= audible_dist && !ship.audible_) {
      ship.audible_ = true;
      audio_.Play(cfg::kWingSnd);
    }

    if (ship.pos_.z <= cfg::kNearZ) {
      InitWarship(ship);
    }
  }
}

// Prepare enemy attack

void Logic::AttackWarships()
{
  auto& player = level_.player_;

  for (auto& ship : level_.ships_)
  {
    if (ship.dead_)
      continue;

    if (ship.pos_.z <= cfg::kMinShotDist)
      continue;

    bool is_attack = !(bool)(rand_toolkit::get_rand(0, 500) % 250);
    if (is_attack && !ship.attack_seq_)
    {
      ship.attack_seq_ = rand_toolkit::get_rand(3, 5);
      ship.attack_wait_ = 0;
    }

    // If in active attack state, the shot, else wait attack_wait_ == 0 

    if (ship.attack_seq_ && !ship.attack_wait_)
    {
      ship.attack_wait_ = 5;
      --ship.attack_seq_;
      
      // Define end point of the shot

      Point shot_end;
      shot_end.x = rand_toolkit::get_rand(-player.w_*2, player.w_*2);
      shot_end.y = rand_toolkit::get_rand(-player.h_*2, player.h_*2);
      shot_end.z = cfg::kEnemyAttackZ;

      // Evaluate velocity vector of the shot

      Point shot_vel;
      shot_vel.z = cfg::kEnemyShotVel;
      double k_dz = std::abs((shot_end.z - ship.pos_.z) / shot_vel.z);
      shot_vel.x = (shot_end.x - ship.pos_.x) / k_dz;
      shot_vel.y = (shot_end.y - ship.pos_.y) / k_dz;

      level_.enemy_shots_.push_back(std::make_pair(ship.pos_, shot_vel));
    }
    else
      --ship.attack_wait_;  // todo: possible overflow???
  }
}

// Process enemy attack

void Logic::ProcessEnemyShots()
{
  auto& player = level_.player_;
  auto& enemy_shots = level_.enemy_shots_;
  
  for (auto& shot : enemy_shots)
  {
    auto& shot_pos = shot.first;
    auto& shot_vel = shot.second;

    shot_pos = shot_pos + shot_vel;

    if (polygon::PointInside(
      -player.w_, -player.h_, player.w_, player.h_, shot.first.x, shot.first.y))
    {
      if (shot.first.z <= std::fabs(shot.second.z))
      {
        audio_.Play(cfg::kScratchSnd);
        player.life_ -= cfg::kEnemyStrenght;
        player.offset_angle_ += rand_toolkit::get_rand(-5, 5);  // shake after hit
      }
    }
    else if (shot.first.z < 0)
      audio_.Play(cfg::kEnemySnd);
  }

  // Clear enemy shots which is out of viewpoint
  
  int  dist = cfg::kNearZ;
  auto predicate = [&dist](const auto& shot) {
    if (shot.first.z <= dist) 
      return true;
    return false;
  };
  enemy_shots.erase(
    std::remove_if(enemy_shots.begin(), enemy_shots.end(), predicate),
    enemy_shots.end()
  );
}

void Logic::ProcessExplosions()
{
  if (level_.cannon_.shot_)
  {
    level_.cannon_.shot_ = false;
    for (auto& ship : level_.ships_)
    {
      if (ship.dead_)
        continue;

      auto& min = ship.bounding_box_.a;
      auto& max = ship.bounding_box_.b;
      auto& mid = level_.cannon_.mid_;

      if (mid.x <= min.x || mid.x >= max.x || mid.y <= min.y || mid.y >= max.y)
        continue;
      else {
        ship.dead_ = true;
        ++level_.ships_destroyed_;
        audio_.Play(cfg::kExplodeSnd);
        PrepareExplosion(ship);
      }
    }
    audio_.Play(cfg::kShotSnd);
  }

  // Move each edge of explosing using random early prepared velocity 

  auto& explosions = level_.explosions_;

  for (auto& expl : level_.explosions_)
  {
    auto& edges = expl.first;
    auto& vels  = expl.second;

    int i {0};
    for (auto& edge : edges)
    {
      edge.a.x += vels[i].x;
      edge.a.y += vels[i].y;
      edge.a.z += vels[i].z;
      edge.b.x += vels[i].x;
      edge.b.y += vels[i].y;
      edge.b.z += vels[i].z;
      ++i;
    }
  }

  // If one of the explosing edge is out of screen then end explosing 

  int near_z = cfg::kNearZ;
  int far_z  = cfg::kShipFarZ;
  
  auto predicate = [&near_z, &far_z](const auto& expl)
  {
    auto& edges = expl.first;
    for (auto& edge : edges) {
      if (edge.a.z <= near_z || edge.b.z >= far_z ||
          edge.a.z <= near_z || edge.b.z >= far_z) {
        return true;
      }
    }
    return false;
  };

  explosions.erase(
    std::remove_if(explosions.begin(), explosions.end(), predicate),
    explosions.end()
  );
}

void Logic::PrepareExplosion(Starship& ship)
{
  std::vector<Segment>  edges;
  std::vector<Vector3d> velocities;
  for (auto& edge : ship.ed_)
  {
    // Here we send ship pos to edges, since edges in ship has 
    // local coordinates. Now when ship is destroyed, edges are
    // objects by ownselves. Yet we change an angle of the edges
    // by add/dec some rand value to points of edges

    Segment s;
    s.a = ship.vx_[edge.v1];
    s.a.x = ship.pos_.x + rand_toolkit::get_rand(-10, 10);
    s.a.y = ship.pos_.y + rand_toolkit::get_rand(-10, 10);
    s.a.z = ship.pos_.z + rand_toolkit::get_rand(-10, 10);
    s.b = ship.vx_[edge.v2];
    s.b.x = ship.pos_.x + rand_toolkit::get_rand(-10, 10);
    s.b.y = ship.pos_.y + rand_toolkit::get_rand(-10, 10);
    s.b.z = ship.pos_.z + rand_toolkit::get_rand(-10, 10);
    
    // Now we multiplie edges count for visual effects

    for (int i = 0; i < cfg::kDebrisCnt; ++i)
      edges.push_back(s);
  }
  
  // Here we fill velocities for each edge in our explosion object

  for (std::size_t i = 0; i < edges.size(); ++i)
  {
    Vector3d edge_vel;  // move ship vel to edges
    edge_vel.x = 2 * rand_toolkit::get_rand(ship.vel_.x-10, ship.vel_.x+10);
    edge_vel.y = 2 * rand_toolkit::get_rand(ship.vel_.y-10, ship.vel_.y+10);
    edge_vel.z = rand_toolkit::get_rand(ship.vel_.z-5, ship.vel_.z-5);
    velocities.push_back(edge_vel);
  }
 
  // Add now created explosion to explosions list

  level_.explosions_.push_back(
    std::make_pair(edges, velocities)
  );
}

void Logic::ProcessViewport(Btn key)
{
  auto& player = level_.player_;
  
  // Process velocity

  if (key == Btn::W)
    ++level_.velocity_;
  else if (key == Btn::S)
    --level_.velocity_;
  if (level_.velocity_ <= 0)
    level_.velocity_ = 0;
  else if (level_.velocity_ >= cfg::kMaxVelocity)
    level_.velocity_ = cfg::kMaxVelocity;
  
  // Process velocity

  if (key == Btn::A)
    --player.curr_angle_;
  if (key == Btn::D)
    ++player.curr_angle_;  

  // Process angle offset
  
    if (player.offset_angle_ < 0)
      player.offset_angle_ = -(player.offset_angle_ + 1);
    else if (player.offset_angle_ > 0)
      player.offset_angle_ = -(player.offset_angle_ - 1);
}

void Logic::ProcessCannon(Pos& pos, Btn key)
{
  auto& cannon = level_.cannon_;

  if (pos.x != -1) { 
    cannon.mid_.x = pos.x;
    cannon.mid_.y = win_.Height() - pos.y;
  }
  
  if (key == Btn::LMB && cannon.ready_) {
    cannon.ready_ = false;
    cannon.shot_ = true;
  }

  if (!cannon.ready_)
    ++cannon.wait_;

  if (cannon.wait_ == cfg::kCannonWait) {
    cannon.wait_ = 0;
    cannon.ready_ = true;
  }
}

bool Logic::ProcessGameState(Btn kbtn)
{
#ifdef DEBUG
  
  // Pause

  if (kbtn == Btn::SPACE) {
    do{
      if (win_.ReadKeyboardBtn(BtnType::KB_DOWN) == Btn::SPACE)
        break;
    } while (true);
  }
  
  // Fast destroy enemy

  if (kbtn == Btn::L)
    level_.ships_destroyed_ = level_.ships_.size();
#endif

  // Process dead state

  if (level_.player_.life_ <= 0)
  {
    level_.state_ = GameState::DEAD;
    // level_.enemy_shots_.clear();    // enemies doesn't shot in
  }

  // Process win state

  if (level_.ships_.size() == level_.ships_destroyed_)
  {
    level_.state_ = GameState::WIN;
    // level_.enemy_shots_.clear();
  }

  // Process repeat game

  if (level_.state_ != GameState::PLAY)
  {
    if (kbtn == Btn::Y) {
      level_.state_ = GameState::PLAY;
      level_.ships_destroyed_ = 0;
      level_.player_.life_ = cfg::kPlayerLife;
      for (auto& ship : level_.ships_)
        ship.dead_ = false;
      return true;
    }
    else if (kbtn == Btn::N)
      return false;
  } 

  // Process exit
  
  if (kbtn == Btn::ESC)
    return false;
  else
    return true;
}

} // namespace anshub