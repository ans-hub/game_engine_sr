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
  , state_{GameState::INIT}
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
  // Process level objects

  MoveStarfield();
  MoveWarships();
  AttackWarships();
  ProcessEnemyShots();
  ProcessExplosions();

  // Process keyboard and events base on it

  auto kbtn = win_.ReadKeyboardBtn(BtnType::KB_DOWN);
  auto mbtn = win_.ReadMouseBtn(BtnType::MS_DOWN);
  auto mpos = win_.ReadMousePos();

  ProcessCannon(mpos, mbtn);
  ProcessVelocity(kbtn);
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
    star.y = rand_toolkit::get_rand(-half_h, half_h);
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
      star.y = rand_toolkit::get_rand(-half_h, half_h);
      star.z = cfg::kStarFarZ;  
    }
  }
}

void Logic::MoveWarships()
{
  // Prepare color step

  double step = (cfg::kMaxBrightness - cfg::kMinBrightness) / cfg::kShipFarZ;

  for (auto& ship : level_.ships_)
  {
    if (ship.dead_)
      continue;

    ship.pos_.x += ship.vel_.x;
    ship.pos_.y += ship.vel_.y;
    ship.pos_.z += ship.vel_.z - level_.velocity_;

    double bright = cfg::kMaxBrightness - (ship.pos_.z * step);
    ship.color_ = color_helpers::IncreaseBrightness(cfg::kShipColor, bright);
    
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
    // if (ship.in_attack_)
      // continue;

    bool is_attack = !(bool)(rand_toolkit::get_rand(0, 500) % 250);
    if (is_attack && !ship.attack_seq_) {
      ship.attack_seq_ = rand_toolkit::get_rand(1,3);
      ship.attack_wait_ = 0;
    }

    if (ship.attack_seq_ && !ship.attack_wait_)
    {
      ship.attack_wait_ = 5;
      --ship.attack_seq_;
      const int kVelZ = -800;
      Point shot_end;
      shot_end.x = rand_toolkit::get_rand(-player.w*2, player.w*2);
      shot_end.y = rand_toolkit::get_rand(-player.h*2, player.h*2);
      shot_end.z = cfg::kEnemyAttackZ;

      Point shot_vel;
      shot_vel.z = kVelZ;
      double k_dz = std::abs((shot_end.z - ship.pos_.z) / kVelZ);
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
  
  int near_z = cfg::kNearZ;
  int far_z  = cfg::kShipFarZ;
  
  for (auto& shot : level_.enemy_shots_)
  {
    shot.first = shot.first + shot.second;
    std::cerr << player.w << ' ' << player.h << '-';
    std::cerr << shot.first.x << ' ' << shot.first.y << ' ' << shot.first.z << '\n';
    // if (shot.first.z > 50 && shot.first.z < 900) {
      if (polygon::PointInside(-player.w, -player.h, player.w, player.h,
      shot.first.x, shot.first.y)) {
        if (shot.first.z <= std::fabs(shot.second.z)) {
          audio_.Play(cfg::kScratchSnd);
          player.life -= cfg::kEnemyStrenght;
        }
      }
      else if (shot.first.z < 0)
        audio_.Play(cfg::kEnemySnd);
    // }
  }

  level_.enemy_shots_.erase(
  std::remove_if(level_.enemy_shots_.begin(), level_.enemy_shots_.end(), 
  [&near_z, &far_z](const auto& shot)
  {
    if (shot.first.z <= near_z)
      return true;
    return false;
  }), level_.enemy_shots_.end()
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
  
  explosions.erase(
  std::remove_if(explosions.begin(), explosions.end(), 
  [&near_z, &far_z](const auto& expl)
  {
    auto& edges = expl.first;
    for (auto& edge : edges) {
      if (edge.a.z <= near_z || edge.b.z >= far_z ||
          edge.a.z <= near_z || edge.b.z >= far_z) {
        return true;
      }
    }
    return false;
  }), explosions.end()
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

void Logic::ProcessVelocity(Btn key)
{
  if (key == Btn::Q)
    ++level_.velocity_;
  else if (key == Btn::A)
    --level_.velocity_;
  if (level_.velocity_ <= 0)
    level_.velocity_ = 0;
  else if (level_.velocity_ >= cfg::kMaxVelocity)
    level_.velocity_ = cfg::kMaxVelocity;
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
  if (kbtn == Btn::SPACE) {
    do{
      if (win_.ReadKeyboardBtn(BtnType::KB_DOWN) == Btn::SPACE)
        break;
    } while (true);
  }
#endif

  if (level_.player_.life <= 0)
    return false;

  if (kbtn == Btn::ESC)
    return false;
  else
    return true;
}

} // namespace anshub