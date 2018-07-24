// *************************************************************
// File:    scene.cc
// Descr:   builds visual game scene
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "scene.h"

namespace anshub {

Scene::Scene(GlWindow& win, Level& level)
  : win_{win}
  , w_{win_.Width()}
  , h_{win_.Height()}
  , half_w_{w_ >> 1}
  , half_h_{h_ >> 1}
  , level_(level)
  , timer_{}
  , buffer_(w_, h_, cfg::kFillColor)
  , text_{win_}
  , curr_fps_{0}
  , prev_fps_{0}
  , time_passed_{0}
  , trig_table_{}
{
  timer_.Start();
}

// Main member function which define sequence of actions

void Scene::Build()
{
  buffer_.Clear();
  DrawPaddle();
  DrawBlocks();
  DrawBalls();
  buffer_.SendDataToFB();

  PrintInfo();
  if (level_.state_ == GameState::WIN)
    PrintCentered("YOU ARE WIN! RETRY? (Y/N)");    
  else if (level_.state_ == GameState::DEAD)
    PrintCentered("YOU ARE DEAD. RETRY? (Y/N)");
  CountFPS();
}

//****************************************************************************
// PRIVATE MEMBER FUNCTIONS
//****************************************************************************

void Scene::DrawPaddle()
{
  DrawRectangle(level_.paddle_.pos_, 40.0f, 10.0f, cfg::kPaddleColor);
}

void Scene::DrawBlocks()
{
  for (auto& block : level_.blocks_)
  {
    if (block.active_)
      DrawRectangle(block.pos_, 20.0f, 10.0f, block.color_);
  }
}

void Scene::DrawBalls()
{
  for (auto& ball : level_.balls_)
  {
    if (ball.active_)
      DrawRectangle(ball.pos_, 10.0f, 10.0f, ball.color_);
  }
}

void Scene::DrawRectangle(const Vector& mid, float x_offset, float y_offset, 
                          int color)
{
  Vector lt {mid.x - x_offset, mid.y - y_offset, 0.0f};
  Vector rt {mid.x + x_offset, mid.y - y_offset, 0.0f};
  Vector lb {mid.x - x_offset, mid.y + y_offset, 0.0f};
  Vector rb {mid.x + x_offset, mid.y + y_offset, 0.0f};
  if (segment2d::Clip(0, 0, w_-1, h_-1, lt.x, lt.y, rt.x, rt.y))
    raster::LineBres(lt.x, lt.y, rt.x, rt.y, color, buffer_);
  if (segment2d::Clip(0, 0, w_-1, h_-1, lb.x, lb.y, rb.x, rb.y))
    raster::LineBres(lb.x, lb.y, rb.x, rb.y, color, buffer_);
  if (segment2d::Clip(0, 0, w_-1, h_-1, lt.x, lt.y, lb.x, lb.y))
    raster::LineBres(lt.x, lt.y, lb.x, lb.y, color, buffer_);
  if (segment2d::Clip(0, 0, w_-1, h_-1, rt.x, rt.y, rb.x, rb.y))  
    raster::LineBres(rt.x, rt.y, rb.x, rb.y, color, buffer_);
}

void Scene::PrintInfo()
{
  std::ostringstream oss {};
  text_.PrintString(60, 70, oss.str().c_str());

  oss.str("");
  oss.clear();
  oss << std::setw(20) << std::left << "Test task for Gaijin by Anton Novoselov";

  text_.PrintString(60, 50, oss.str().c_str());  
}

void Scene::PrintCentered(const char* msg)
{
  std::ostringstream oss {msg};
  text_.PrintString(w_/2-oss.str().size()*7/2, h_/2, oss.str().c_str());  
}

void Scene::CountFPS()
{
  timer_.End();
  ++curr_fps_;
  time_passed_ += timer_.GetEndTime() - timer_.GetStartTime();
  if (time_passed_ >= 1000) {
    time_passed_ = 0;
    prev_fps_ = curr_fps_;
    curr_fps_ = 0;
  }
  timer_.Start(); 
}
 
} // namespace anshub