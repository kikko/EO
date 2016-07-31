#include <iomanip>
#include <iostream>
#include <sstream>

#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Renderer.h>
#include <Magnum/Timeline.h>

#include "CameraView.h"
#include "FlyCaptureCamera.h"
#include "Label.h"

using namespace Magnum;

class App : public Platform::Application {
public:
  explicit App(const Arguments &arguments);

private:
  void tickEvent() override;
  void drawEvent() override;
  void mouseMoveEvent(MouseMoveEvent &event) override;
  Label fpsView;
  Timeline timeline;
  // camera view
  CameraView cameraView;
  // flycapture camera
  FlyCaptureCamera fcCamera;
};

App::App(const Arguments &arguments)
    : Platform::Application(
          arguments,
          Configuration().setTitle("EO beta").setSize({1280, 1024})) {
  fpsView.setup();
  // setup flycapture cam
  fcCamera.setup();
  // camera view
  cameraView.setup();
  // enable alpha blending
  Renderer::enable(Renderer::Feature::Blending);
  Renderer::setBlendFunction(Renderer::BlendFunction::SourceAlpha,
                             Renderer::BlendFunction::OneMinusSourceAlpha);
  Renderer::setBlendEquation(Renderer::BlendEquation::Add,
                             Renderer::BlendEquation::Add);
  // limit framrate to 60hz
  // setMinimalLoopPeriod(16);
  timeline.start();
}

void App::tickEvent() {
  // update camera
  fcCamera.update();
  // update framerate label
  const float fps = 1.0f / timeline.previousFrameDuration();
  std::ostringstream text;
  text << Int(fps) << "fps";
  fpsView.setText(text.str());
}

void App::drawEvent() {
  // update display
  defaultFramebuffer.clear(FramebufferClear::Color);
  // draw our content
  cameraView.updateTexture(fcCamera.rawImage);
  cameraView.draw();
  fpsView.draw();
  // swap buffers
  swapBuffers();
  // call next draw
  redraw();
  timeline.nextFrame();
}

void App::mouseMoveEvent(MouseMoveEvent &event) { (void)event; }

MAGNUM_APPLICATION_MAIN(App)
