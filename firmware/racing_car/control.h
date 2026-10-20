#pragma once
#include <stdint.h>
namespace racing {
constexpr uint32_t TIMEOUT_MS = 500;
constexpr uint32_t REVERSAL_MS = 80;
struct Pair { int16_t left; int16_t right; };
inline int8_t sign(int16_t v) { return v > 0 ? 1 : v < 0 ? -1 : 0; }
inline bool elapsed(uint32_t now, uint32_t then, uint32_t interval) {
  return uint32_t(now - then) >= interval;
}
class Controller {
  bool armed_ = false;
  char motion_ = 'S';
  uint8_t speed_ = 100;
  uint32_t lastMotion_ = 0;
public:
  void disarm() { armed_ = false; motion_ = 'S'; }
  bool armed() const { return armed_; }
  uint8_t speed() const { return speed_; }
  void receive(char c, uint32_t now, bool stopClosed) {
    if (!stopClosed) { disarm(); return; }
    if (c == '\r' || c == '\n' || c == ' ') return;
    if (c == 'X') { disarm(); return; }
    if (c == 'A') { armed_ = true; motion_ = 'S'; return; }
    if (c == 'S') { motion_ = 'S'; return; }
    if (c >= '0' && c <= '9') {
      speed_ = uint8_t((c - '0') * 255 / 9);
      motion_ = 'S'; // Require a fresh motion command after speed selection
      return;
    }
    if (c == 'F' || c == 'B' || c == 'L' || c == 'R') {
      if (armed_) { motion_ = c; lastMotion_ = now; }
      return;
    }
    disarm(); // Unknown protocol byte: stop and require re-arm
  }
  Pair output(uint32_t now, bool stopClosed) {
    if (!stopClosed) disarm();
    if (armed_ && motion_ != 'S' && elapsed(now, lastMotion_, TIMEOUT_MS)) disarm();
    if (!armed_) return {0, 0};
    const int16_t v = speed_;
    const int16_t reverse = int16_t(-v);
    switch (motion_) {
      case 'F': return {v, v};
      case 'B': return {reverse, reverse};
      case 'L': return {reverse, v};
      case 'R': return {v, reverse};
      default: return {0, 0};
    }
  }
};
class ReversalGuard {
  int16_t value_ = 0;
  int8_t lastSign_ = 0;
  uint32_t zeroAt_ = 0;
public:
  int16_t update(int16_t request, uint32_t now) {
    if (!request) {
      if (value_) zeroAt_ = now;
      value_ = 0; return 0;
    }
    if (value_ && sign(request) != sign(value_)) {
      lastSign_ = sign(value_); zeroAt_ = now; value_ = 0; return 0;
    }
    if (!value_ && lastSign_ && sign(request) != lastSign_ &&
        !elapsed(now, zeroAt_, REVERSAL_MS)) return 0;
    value_ = request; lastSign_ = sign(request); return value_;
  }
};
} // namespace racing
