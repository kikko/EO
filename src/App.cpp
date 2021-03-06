// TODO
// bug: CRITICAL app lock up
// audio: new clap sound
// audio: clap sound variations
// ux: prevent hidden balls from clapping
// gfx: roundy ends

#include <fstream>
#include <iostream>
#include <iostream>
#include <sstream>

#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Renderer.h>
#include <Magnum/Timeline.h>

#include "capture/FlyCaptureCamera.h"
#include "capture/SimulationCapture.h"
#include "tracking/BallTracker.h"
#include "utils.h"
#include "view/BallMenController.h"
#include "view/Label.h"
#include "view/MatView.h"

using namespace Magnum;

namespace eo {

using namespace view;
using namespace tracking;
using namespace capture;

class App : public Platform::Application {
public:
  explicit App(const Arguments &arguments);

private:
  void tickEvent() override;
  void drawEvent() override;
  void mouseMoveEvent(MouseMoveEvent &event) override;
  void keyPressEvent(KeyEvent &event) override;
  //
  void updateBallMen(const std::vector<FollowedCircle> &circles);
  // properties
  Label fpsView;
  Timeline timeline;
  MatView MatView;
  std::shared_ptr<AbstractCapture> capture;
  BallMenController ballmen;
  BallTracker ballTracker;
  bool debugMode;
  double ellapsedTime;
  double timer;
};

App::App(const Arguments &arguments)
    : Platform::Application(
          arguments, Configuration()
                         .setWindowFlags(Configuration::WindowFlag::Fullscreen)
                         .setTitle("EO beta")
                         .setSize({1920, 1080})),
      capture(new FlyCaptureCamera) {
  SDL_ShowCursor(SDL_DISABLE);
  ellapsedTime = timer = 0.f;
  debugMode = false;
  fpsView.setup();
  // setup flycapture cam
  bool isAvailable = capture->setup();
  if (!isAvailable) {
    capture = std::make_shared<SimulationCapture>();
    capture->setup();
  }
  // camera view
  MatView.setup();
  // ball tracker
  ballTracker.setup();
  // enable alpha blending
  Renderer::enable(Renderer::Feature::Blending);
  Renderer::setBlendFunction(Renderer::BlendFunction::SourceAlpha,
                             Renderer::BlendFunction::OneMinusSourceAlpha);
  Renderer::setBlendEquation(Renderer::BlendEquation::Add,
                             Renderer::BlendEquation::Add);
  // limit framrate to 60hz
  setMinimalLoopPeriod(16);
  timeline.start();
}

void App::tickEvent() {
  ellapsedTime += timeline.previousFrameDuration();
  timer += timeline.previousFrameDuration();
  if (timer > 3) {
    timer -= 3;
    std::ofstream myfile;
    myfile.open("log.txt");
    myfile << "running";
    myfile.close();
  }
  // update camera
  if (capture) {
    capture->update();
    if (capture->hasNewImage()) {
      ballTracker.update(capture->getCvImage(), debugMode);
    }
  }
  // update characters
  ballmen.update(ballTracker.follower.circles, ellapsedTime);
  // update framerate label
  if (debugMode) {
    const float fps = 1.0f / timeline.previousFrameDuration();
    std::ostringstream text;
    text << Int(fps) << "fps" << std::endl
         << "tracking: " << Int(ballTracker.getTrackTime()) << "ms" << std::endl
         << "detected: " << ballTracker.follower.circles.size() << std::endl
         << "balls: " << ballmen.getNum() << std::endl
         << "thresh: " << ballTracker.detector.minHue << "-"
         << ballTracker.detector.maxHue;
    fpsView.setText(text.str());
  }
}

void App::drawEvent() {
  // update display
  defaultFramebuffer.clear(FramebufferClear::Color);
  // draw our content
  if (capture) {
    if (capture->hasNewImage()) {
      MatView.updateTexture(ballTracker.getCurrFrame());
    }
    MatView.draw();
  }
  // draw debug data
  if (debugMode) {
    fpsView.draw();
  }
  // draw ballmen
  ballmen.draw();
  // swap buffers
  swapBuffers();
  // call next draw
  redraw();
  timeline.nextFrame();
}

void App::mouseMoveEvent(MouseMoveEvent &event) {
  Vector2i mouse = event.position();
  Vector2i size = defaultFramebuffer.viewport().size();
  float screenScale = static_cast<float>(size.x()) / 1280;
  float x = static_cast<float>(size.x() - mouse.x()) / screenScale;
  float y = static_cast<float>(mouse.y()) / screenScale + 150;
  SimulationCapture::mouse.x = x;
  SimulationCapture::mouse.y = y;
  event.setAccepted();
}

void App::keyPressEvent(KeyEvent &event) {
  if (event.key() == KeyEvent::Key::S) {
    // save image
    capture->saveImage(0.5);
  } else if (event.key() == KeyEvent::Key::Space) {
    ballTracker.setNextFrameId();
  } else if (event.key() == KeyEvent::Key::D) {
    debugMode = !debugMode;
  } else if (event.key() == KeyEvent::Key::Left) {
    ballTracker.detector.minHue -= 1;
  } else if (event.key() == KeyEvent::Key::Right) {
    ballTracker.detector.minHue += 1;
  } else if (event.key() == KeyEvent::Key::Down) {
    ballTracker.detector.maxHue -= 1;
  } else if (event.key() == KeyEvent::Key::Up) {
    ballTracker.detector.maxHue += 1;
  }
  event.setAccepted(true);
}
}

int main(int argc, char **argv) {
  eo::App app({argc, argv});
  return app.exec();
}
