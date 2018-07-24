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
  audio.Load(cfg::kBackgroundMusic, true);
  audio.Load(cfg::kScratchSnd, false);
  audio.Load(cfg::kClickSnd, false);
  audio.Play(cfg::kBackgroundMusic);

  InitLogic();
}

void Logic::InitLogic()
{
  InitPaddle();
  InitBalls();
  InitBlocks();
  level_.state_ = GameState::PLAY;
}

bool Logic::Process()
{
  auto kbtn = win_.ReadKeyboardBtn(BtnType::KB_DOWN);
  
  if (level_.state_ == GameState::INIT)
  {
    InitLogic();
  }

  if (level_.state_ == GameState::PLAY)
  {
    ProcessBalls(kbtn);
    MovePaddle(kbtn);
  }

  return ProcessGameState(kbtn);
}

void Logic::InitPaddle()
{
  level_.paddle_.pos_.x = win_.Width()/2;
  level_.paddle_.pos_.y = 100;
}

void Logic::InitBalls()
{
  int half_w = win_.Width() >> 1;
  for (auto& ball : level_.balls_)
  {
    ball.active_ = true;
    ball.color_ = cfg::kBallColor;
    ball.pos_.x = rand_toolkit::get_rand(0.0f, win_.Width()-10.0f);
    ball.pos_.y = win_.Height() / 2;
    ball.vel_.x = rand_toolkit::get_rand(4.0f, 6.0f);
    ball.vel_.y = rand_toolkit::get_rand(4.0f, 6.0f);
    if (rand_toolkit::coin_toss())
      ball.vel_.x = -ball.vel_.x;
    if (rand_toolkit::coin_toss())
      ball.vel_.y = -ball.vel_.y;    
  }
}

void Logic::InitBlocks()
{
  int blocks_in_row = 15;
  int rows = 5;

  float x_step   = win_.Width() / (blocks_in_row + 1);
  float y_step   = win_.Height() / 20;
  float y_offset = win_.Height() - 200.0f;

  level_.blocks_.clear();
  for (int row = 0; row < rows; ++row)
  {
    float x_offset = 50.0f;
    for (int col = 0; col < blocks_in_row; ++col)
    {
      Block block {};
      block.pos_.x = x_offset;
      block.pos_.y = y_offset;
      block.color_ = cfg::kBlockColor >> row;
      level_.blocks_.push_back(block);
      x_offset += x_step;
    }
    y_offset += y_step;
  }
}

void Logic::ProcessBalls(Btn key)
{
  // Process balls and paddle collision
  
  for (auto& ball : level_.balls_)
  {
    if (ball.pos_.x > (level_.paddle_.pos_.x - 40.0f) &&
        ball.pos_.x < (level_.paddle_.pos_.x + 40.0f) && 
        ball.pos_.y - 10.0f < (level_.paddle_.pos_.y + 10.0f) &&
        ball.pos_.y - 10.0f > (level_.paddle_.pos_.y - 10.0f))
    {
      audio_.Play(cfg::kClickSnd);
      
      // Change vertical direction of the ball

      ball.vel_.y = -ball.vel_.y;
     
      // Change trajectory since paddle moved

      if (key == Btn::RIGHT)
        ball.vel_.x -= rand_toolkit::get_rand(0,100) % 3;
      else if (key == Btn::LEFT)
        ball.vel_.x += rand_toolkit::get_rand(0,100) % 3;
      else
        ball.vel_.x += -1 + (rand_toolkit::get_rand(0,100) % 3);
    }
  }

  // Process balls and screen walls

  for (auto& ball : level_.balls_)
  {
    if (ball.active_)
    {
      if (ball.pos_.x > win_.Width() - 10.0f)
      {
        audio_.Play(cfg::kClickSnd);
        ball.vel_.x = -ball.vel_.x;
      }
      else if (ball.pos_.x < 10.0f)
      {
        audio_.Play(cfg::kClickSnd);
        ball.vel_.x = -ball.vel_.x;
      }
      else if (ball.pos_.y > win_.Height() - 10.0f)
      {
        audio_.Play(cfg::kClickSnd);
        ball.vel_.y = -ball.vel_.y;
      }
      else if (ball.pos_.y < 10.0f)
        ball.active_ = false;
    }
  }

  // Process balls and blocks collision

  for (auto& ball : level_.balls_)
  {
    for (auto& block : level_.blocks_)
    {
      if (block.active_ &&
          ball.pos_.x > (block.pos_.x - 20.0f) &&
          ball.pos_.x < (block.pos_.x + 20.0f) && 
          ball.pos_.y + 10.0f < (block.pos_.y + 10.0f) &&
          ball.pos_.y + 10.0f > (block.pos_.y - 10.0f))
      {
        audio_.Play(cfg::kScratchSnd);
        block.active_ = false;
  
        if (ball.vel_.y > 0)
          ball.vel_.y = -ball.vel_.y;
        
        if (ball.pos_.x > (block.pos_.x - 20.0f) &&
            ball.pos_.x < (block.pos_.x + 20.0f))
          ball.vel_.x = -ball.vel_.x;
      }
    }
  } 

  // Process balls positions and velocity change

  for (auto& ball : level_.balls_)
  {
    ball.pos_.y += ball.vel_.y;
    ball.pos_.x += ball.vel_.x;
  }
}

void Logic::MovePaddle(Btn key)
{
  if (key == Btn::LEFT && level_.paddle_.pos_.x > 60.0f)
    level_.paddle_.pos_.x -= 18.0f;
  else if (key == Btn::RIGHT && level_.paddle_.pos_.x < win_.Width()-60.0f)
    level_.paddle_.pos_.x += 18.0f;
}

bool Logic::ProcessGameState(Btn kbtn)
{
  // Pause

  if (kbtn == Btn::SPACE) {
    do{
      if (win_.ReadKeyboardBtn(BtnType::KB_DOWN) == Btn::SPACE)
        break;
    } while (true);
  }

  // Process dead

  int balls_alive {0};
  for (auto& ball : level_.balls_)
  {
    if (ball.active_)
      ++balls_alive;
  }
  if (balls_alive == 0)
    level_.state_ = GameState::DEAD;

  // Process win

  int blocks_alive {0};
  for (auto& block : level_.blocks_)
  {
    if (block.active_)
      ++blocks_alive;
  }
  if (blocks_alive == 0)
    level_.state_ = GameState::WIN;

  // Process game state change

  if (level_.state_ == GameState::DEAD && kbtn == Btn::Y ||
      level_.state_ == GameState::WIN && kbtn == Btn::Y)
  {

    level_.state_ = GameState::INIT;
  }

  // Process exit
  
  if (kbtn == Btn::ESC)
    return false;
  else
    return true;
}

} // namespace anshub 