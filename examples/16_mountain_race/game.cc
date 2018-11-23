// *************************************************************
// File:    game.cc
// Descr:   example of simple terrain game
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include <stdexcept>

#include "scene.h"
#include "level.h"
#include "logic.h"

#include "lib/data/cfg_loader.h"

#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;
using vector::operator<<;

auto ProcessConfig(int argc, const char** argv)
{
  if (argc != 2)
    throw std::runtime_error("You must input config file name");
  return Config(argv[1]);
}

void PrintDebug(FpsCounter& fps, const Level&, const Scene& scene)
{
  std::cerr << "Frames per second: " << fps.ReadPrev() << '\n';
  std::cerr << "Chunks culled: " << scene.GetObjectsCulled() << '\n';
  std::cerr << "Hidden surfaces: " << scene.GetSurfacesHidden() << '\n';
  std::cerr << "Triangles total: " << scene.GetTrianglesCount() << '\n';
  std::cerr << "Triangles culled: " << scene.GetTrianglesCulled() << '\n';   
  std::cerr << '\n';
}

int main(int argc, const char** argv)
{
  auto cfg = ProcessConfig(argc, argv);

  const int kWinWidth {cfg.Get<int>("win_w")};
  const int kWinHeight {cfg.Get<int>("win_h")};
  const bool kDebugShow {cfg.Get<bool>("dbg_show_info")};  
  const int kFpsWait {cfg.Get<int>("win_fps")};

  // Create window and usefull stuff
 
  auto pos  = io_helpers::GetXYToMiddle(kWinWidth, kWinHeight); 
  auto mode = io_helpers::FindVideoMode(kWinWidth, kWinHeight);
  GlWindow win (pos.x, pos.y, kWinWidth, kWinHeight, "Mountain ride");

  if (cfg.Get<bool>("win_fs"))
    win.ToggleFullscreen(mode);
  win.HideCursor();
  
  rand_toolkit::start_rand();

  // Create entities

  FpsCounter fps     {};
  Timer      timer   (kFpsWait);
  Level      level   {cfg};
  Logic      logic   {cfg, win, level};
  Scene      scene   {cfg, win, level};

  const  int MS_PER_FRAME {30};
  double prev = timer.GetCurrentClock();
  double lags = 0.0f;

  do {
    double curr = timer.GetCurrentClock();
    double elapsed = curr - prev;
    prev = curr;
    lags += elapsed;
    win.Clear();

    while (lags >= MS_PER_FRAME)
    {      
      lags -= MS_PER_FRAME;
      logic.Process();
    }
    
    scene.Build(lags / MS_PER_FRAME);
    fps.Count();
    win.Render();

    if (kDebugShow && fps.Ready())
      PrintDebug(fps, level, scene);
  
  } while (!win.Closed());

  return 0;
}
