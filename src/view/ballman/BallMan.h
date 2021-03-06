#ifndef BallMan_h
#define BallMan_h

#include "view/ballman/Contours.h"
#include "view/ballman/Face.h"
#include "view/ballman/Leg.h"
#include "view/fx/Clap.h"
#include "view/primitives/Circle.h"
#include <Magnum/Audio/Source.h>

namespace eo {
namespace view {

class BallMan {
public:
  BallMan() : visible(true){};
  ~BallMan();
  void setup(Vector2 p, float radius, Color3 color,
             const tracking::DetectedCircle &circle);
  void update(Vector2 p, float radius, float t,
              const tracking::DetectedCircle &circle);
  void draw();

  void setColor(Magnum::Color3 color);

  bool visible;
  Vector2 position;
  float radius;
  // float inteRadius;

  int mergedCounter;

  Circle body;
  Leg leftArm;
  Leg rightArm;
  Leg leftLeg;
  Leg rightLeg;
  Face face;
  Contours contours;
  // std::shared_ptr<Magnum::Audio::Source> yeye;
};

} // namespace view
} // namespace eo

#endif /* end of include guard: BallMan_h */
