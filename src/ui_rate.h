// Dante's Inferno's interface at 60 Hz whatever the frame rate. The game itself follows the refresh rate (it reads it
// at start and steps by 1/rate), but its interface, EA's APT (Flash), steps once a frame: above 60 the menus, the HUD
// and the absolve minigame would run faster. The technique is the one florinp93's hells-gate-recomp documents for its
// build (docs/frame_rate.md); the places, found anew in this build, are:
//   sub_821A37A8  the frame's messages: iMsgSystemTick (object 0x82AE9CC0; a message's id is its object's first word),
//                 then iMsgRunningTick or iMsgPausedTick (0x82ADE22C, 0x82ADE20C)
//   sub_824719B0  the screens' handler: on SystemTick, sub_82421FA0(screens, milliseconds now), which times the
//                 transitions in real milliseconds, then steps each of the 5 screen layers
//   sub_8240E210  the APT manager's handler: on Running/PausedTick it advances the movies, sub_823CE710(20), the same
//                 20 whatever the time gone by
//   sub_8242E990  the input manager's poll, on RunningTick: for each of 4 pads, at +60 + pad * 68, the buttons (+0)
//                 and the frame before's (+2), from which presses are found
// Above 60, whether a frame is one of the interface's is decided once, on its SystemTick (1/60 s of real time gathered
// since the last), and both handlers run on those frames only, so the screens and their movies keep in step. Their
// other messages always reach them. So that a press on a frame the interface skips is still a press when it runs,
// the pads' "before" buttons are, while it runs, the buttons it saw on its own last frame; the game's are put back
// after. PORT_UI_RATE_LOG: the counts every 5 s.

#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>

#include <rex/hook.h>
#include <rex/logging.h>
#include <rex/ppc.h>
#include <rex/system/xmemory.h>

#include "port/frame_rate.h"

namespace ui_rate {

constexpr uint32_t kSystemTick = 0x82AE9CC0;
constexpr uint32_t kRunningTick = 0x82ADE22C;
constexpr uint32_t kPausedTick = 0x82ADE20C;
constexpr uint32_t kPads = 60;  // the input manager's pads: buttons, then the frame before's
constexpr uint32_t kPadStride = 68;
constexpr double kPeriod = 1.0 / 60.0;

inline uint32_t g_input_manager = 0;
inline bool g_ui_frame = true;                     // this frame is one the interface runs on
inline std::array<uint16_t, 4> g_seen{};           // the buttons the interface saw last, this frame or before
inline std::array<uint16_t, 4> g_seen_last_frame{};  // and as its last frame ended: its "before" while it runs
inline double g_gathered = 0.0;
inline std::chrono::steady_clock::time_point g_last_tick{};

inline uint32_t Load32(uint8_t* base, uint32_t address) {
  return __builtin_bswap32(*rex::memory::GuestPtr<uint32_t*>(base, address));
}
inline uint16_t* Pad16(uint8_t* base, uint32_t address) { return rex::memory::GuestPtr<uint16_t*>(base, address); }
inline bool Is(uint8_t* base, uint32_t message, uint32_t object) {
  return Load32(base, message) == Load32(base, object);
}
inline bool Throttled() { return TargetFps() > 60; }  // unlimited (0) too

// on each SystemTick above 60: whether 1/60 s of real time has gathered
inline bool Due() {
  auto now = std::chrono::steady_clock::now();
  if (g_last_tick.time_since_epoch().count() == 0) {
    g_last_tick = now;
    return true;
  }
  g_gathered += std::chrono::duration<double>(now - g_last_tick).count();
  g_last_tick = now;
  // a quarter of a period early still counts, so a steady 120 runs every other frame and not one in three
  if (g_gathered < kPeriod * 0.75) return false;
  g_gathered -= kPeriod;
  if (g_gathered > kPeriod) g_gathered = 0.0;  // after a stall, no burst of catching up
  if (g_gathered < -kPeriod * 0.5) g_gathered = -kPeriod * 0.5;
  return true;
}

// the interface running: the pads' "before" are its own last frame's, and the game's again after
class InputScope {
 public:
  explicit InputScope(uint8_t* base) : base_(base), pads_(g_input_manager ? g_input_manager + kPads : 0) {
    if (!pads_) return;
    for (uint32_t p = 0; p < 4; ++p) {
      uint16_t* before = Pad16(base_, pads_ + p * kPadStride + 2);
      game_[p] = *before;
      *before = g_seen_last_frame[p];  // guest byte order both, copied as they are
    }
  }
  ~InputScope() {
    if (!pads_) return;
    for (uint32_t p = 0; p < 4; ++p) {
      *Pad16(base_, pads_ + p * kPadStride + 2) = game_[p];
      g_seen[p] = *Pad16(base_, pads_ + p * kPadStride);
    }
  }

 private:
  uint8_t* base_;
  uint32_t pads_;
  std::array<uint16_t, 4> game_{};
};

inline void Count(int handler, bool ran) {
  static const bool on = std::getenv("PORT_UI_RATE_LOG") != nullptr;
  if (!on) return;
  static int ticks[2] = {}, runs[2] = {};
  static auto since = std::chrono::steady_clock::now();
  ++ticks[handler];
  runs[handler] += ran;
  auto now = std::chrono::steady_clock::now();
  if (now - since >= std::chrono::seconds(5)) {
    REXLOG_INFO("ui rate: screens {} ticks {} runs, movies {} ticks {} runs, in 5 s", ticks[0], runs[0], ticks[1],
                runs[1]);
    ticks[0] = ticks[1] = runs[0] = runs[1] = 0;
    since = now;
  }
}

}  // namespace ui_rate

REX_EXTERN(__imp__sub_8242E990);
REX_EXTERN(__imp__sub_824719B0);
REX_EXTERN(__imp__sub_8240E210);

// the input manager's poll: only to know where the manager is
REX_HOOK_RAW(sub_8242E990) {
  ui_rate::g_input_manager = ctx.r3.u32;
  __imp__sub_8242E990(ctx, base);
}

// the screens' handler(this, message)
REX_HOOK_RAW(sub_824719B0) {
  using namespace ui_rate;
  if (!Throttled() || !Is(base, ctx.r4.u32, kSystemTick)) {
    __imp__sub_824719B0(ctx, base);
    return;
  }
  g_ui_frame = Due();
  Count(0, g_ui_frame);
  if (!g_ui_frame) return;
  g_seen_last_frame = g_seen;
  InputScope input(base);
  __imp__sub_824719B0(ctx, base);
}

// the APT manager's handler(this, message)
REX_HOOK_RAW(sub_8240E210) {
  using namespace ui_rate;
  if (!Throttled() || !(Is(base, ctx.r4.u32, kRunningTick) || Is(base, ctx.r4.u32, kPausedTick))) {
    __imp__sub_8240E210(ctx, base);
    return;
  }
  Count(1, g_ui_frame);
  if (!g_ui_frame) return;
  InputScope input(base);
  __imp__sub_8240E210(ctx, base);
}
