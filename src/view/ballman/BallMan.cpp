#include "view/ballman/BallMan.h"

#include <Magnum/Math/Vector.h>

using namespace Magnum;

namespace eo {
namespace view {

BallMan::~BallMan() {
  if (leftArm.targetLeg) {
    leftArm.targetLeg->targetLeg = nullptr;
  }
  if (rightArm.targetLeg) {
    rightArm.targetLeg->targetLeg = nullptr;
  }
}

void BallMan::setup(Vector2 p, float r, Color3 color,
                    const tracking::DetectedCircle &circle) {
  // setup body
  position = p;
  body.setup(100, 64);
  // radius should be used to accurately initialize the legs like in update
  radius = r;
  // setup arms & legs
  leftArm.setup(p, 25);
  rightArm.setup(p, 25);
  leftLeg.setup(p, 17);
  rightLeg.setup(p, 17);
  // setup face
  face.setup();
  // set color;
  setColor(color);
  // setup contour
  contours.setup(circle.blob);
  mergedCounter = 0;
  //
  // inteRadius = 0;
}

void BallMan::setColor(Magnum::Color3 color) {
  leftArm.color = color;
  rightArm.color = color;
  leftLeg.color = color;
  rightLeg.color = color;
  body.color = color;
  face.setColor(color);
  contours.color = color;
}

void BallMan::update(Vector2 p, float r, float t,
                     const tracking::DetectedCircle &circle) {
  // check merged status
  if (circle.isCircle) {
    mergedCounter = 0;
    leftArm.isVisible = rightArm.isVisible = true;
    leftLeg.isVisible = rightLeg.isVisible = true;
  } else {
    mergedCounter++;
    if (mergedCounter > 9) {
      leftArm.isVisible = rightArm.isVisible = false;
      leftLeg.isVisible = rightLeg.isVisible = false;
    }
  }
  // apply positions and scale
  position += (p - position) * 0.45;
  // std::cout << radius << std::endl;
  // inteRadius += r - radius;
  radius += (r - radius) * 0.45;
  Vector2 s = Vector2{radius / body.radius, radius / body.radius};
  // update arms
  leftArm.update(Vector2(position.x() - radius, position.y()), t);
  leftArm.scale = s;
  rightArm.update(Vector2(position.x() + radius, position.y()), t);
  rightArm.scale = s;
  // update legs
  Vector2 legsOffset =
      Matrix3::rotation(60.0_degf).transformVector({radius, 0});
  leftLeg.update(position + legsOffset, t);
  leftLeg.scale = s;
  Vector2 rightLegOffset{position.x() - legsOffset.x(),
                         position.y() + legsOffset.y()};
  rightLeg.update(rightLegOffset, t);
  rightLeg.scale = s;
  // update body radius
  body.scale = s;
  body.position = position;
  // update face
  face.update(p, s);
  // update contours
  contours.scale = s;
  contours.position = position;
  contours.update(circle.blob);
  // if (inteRadius > 100) {
  //   if (yeye->state() != Magnum::Audio::Source::State::Playing) {
  //     yeye->play();
  //   }
  // }
  // inteRadius *= 0.9;
}

void BallMan::draw() {
  if (!visible) {
    return;
  }
  // if (isMerged) {
  //   contours.draw();
  //   return;
  // }
  // draw body
  // body.draw();
  // draw arms & legs
  if (leftArm.isVisible == true) {
    leftArm.draw();
    rightArm.draw();
    leftLeg.draw();
    rightLeg.draw();
  } else {
    // draw contours
    contours.draw();
  }
  // draw face
  face.draw();
}

} // namespace view
} // namespace eo
