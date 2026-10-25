// dantesinferno - ReXGlue Recompiled Project
//
// The port's app: the common base of the Xbox 360 ports (port/port_app.h) and Dante's Inferno's own hooks, the frame
// rate (frame_rate.h).

#pragma once

#include "port/port_app.h"

#include "frame_rate.h"

class DantesInfernoApp : public PortApp {
 public:
  using PortApp::PortApp;

  static std::unique_ptr<rex::ui::WindowedApp> Create(rex::ui::WindowedAppContext& ctx) {
    crash_log::Install();
    return std::unique_ptr<DantesInfernoApp>(new DantesInfernoApp(ctx, "dantesinferno", PPCImageConfig));
  }

  void OnPreSetup(rex::RuntimeConfig& config) override {
    PortApp::OnPreSetup(config);
    ApplyFrameRate();
  }
};
