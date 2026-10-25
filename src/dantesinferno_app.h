// dantesinferno - ReXGlue Recompiled Project
//
// The port's app: the common base of the Xbox 360 ports (port/port_app.h) and Dante's Inferno's own hooks. The game
// runs at 60 on the console, presenting every vertical blank: above 60 it follows the faster blank port/frame_rate.h sets.

#pragma once

#include "port/port_app.h"

class DantesInfernoApp : public PortApp {
 public:
  using PortApp::PortApp;

  static std::unique_ptr<rex::ui::WindowedApp> Create(rex::ui::WindowedAppContext& ctx) {
    crash_log::Install();
    return std::unique_ptr<DantesInfernoApp>(new DantesInfernoApp(ctx, "dantesinferno", PPCImageConfig));
  }
};
