// fps = 60 (the original) or more. Dante's Inferno already runs at 60 on the Xbox 360: it presents every vertical
// blank and times its play by the rate the display reports (XGetVideoMode). Above 60 the console's vertical blank
// itself runs faster (video_mode_refresh_rate, unless set), and the game follows it.

#pragma once

#include <string>

#include <rex/cvar.h>

REXCVAR_DEFINE_INT32(fps, 60, "Port", "Frame rate: 60 (the original), 120...");

// before the GPU starts: the vertical blank at the frame rate when it is above 60
inline void ApplyFrameRate() {
  int fps = REXCVAR_GET(fps);
  if (fps > 60 && rex::cvar::GetFlagSource("video_mode_refresh_rate") == rex::cvar::Source::kDefault)
    rex::cvar::SetFlagByName("video_mode_refresh_rate", std::to_string(fps));
}
