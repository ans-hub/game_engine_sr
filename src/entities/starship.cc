// *************************************************************
// File:    starship.cc
// Descr:   starship entitie
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "starship.h"

namespace anshub {

Starship::Starship()
  : pos_{}
  , vel_{}
  , color_{cfg::kShipColor}
  , vx_{
      Point(-40, 40, 0),
      Point(-40, 0, 0),
      Point(-40,- 40, 0),
      Point(-10, 0, 0),
      Point(0, 20, 0),
      Point(10, 0, 0),
      Point(0, -20, 0),
      Point(40, 40, 0),
      Point(40, 0, 0),
      Point(40, -40, 0) }
  , ed_{
      Edge(0, 2),
      Edge(1, 3),
      Edge(3, 4),
      Edge(4, 5),
      Edge(5, 6),
      Edge(6, 3),
      Edge(5, 8),
      Edge(7, 9) }
  , bounding_box_{}
  , dead_{true}
  , audible_{false}
  , in_attack_{false}
  , aim_attack_{-1,-1,-1}
{ }

} // namespace anshub