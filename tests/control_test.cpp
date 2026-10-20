#include "../firmware/racing_car/control.h"
#include <assert.h>
#include <iostream>
using namespace racing;
int main() {
  Controller c;
  assert(c.output(0, true).left == 0);
  c.receive('F', 0, true); assert(!c.armed());
  c.receive('A', 0, true); assert(c.armed() && c.output(0, true).left == 0);
  c.receive('3', 0, true); assert(c.speed() == 85);
  c.receive('F', 10, true);
  assert(c.output(509, true).left == 85);
  assert(c.output(510, true).left == 0 && !c.armed());
  c.receive('F', 520, true); assert(c.output(520, true).left == 0);
  c.receive('A', 530, true); c.receive('B', 530, true);
  assert(c.output(530, true).left == -85);
  c.receive('L', 540, true); assert(c.output(540, true).right == 85);
  c.receive('R', 550, true); assert(c.output(550, true).left == 85);
  c.receive('S', 560, true); assert(c.output(560, true).left == 0 && c.armed());
  c.receive('F', 570, true); c.receive('9', 580, true);
  assert(c.speed() == 255 && c.output(580, true).left == 0);
  c.receive('F', 590, true); c.receive('\n', 600, true);
  assert(c.output(1090, true).left == 0 && !c.armed());
  c.receive('A', 1200, true); c.receive('F', 1200, true);
  assert(c.output(1201, false).left == 0 && !c.armed());
  c.receive('A', 1210, false); assert(!c.armed());
  c.receive('A', 1220, true); c.receive('?', 1220, true); assert(!c.armed());
  c.receive('A', 1230, true); c.receive('X', 1230, true); assert(!c.armed());
  Controller wrap;
  wrap.receive('A', UINT32_MAX - 99, true);
  wrap.receive('F', UINT32_MAX - 99, true);
  assert(wrap.output(10, true).left == 100);
  assert(wrap.output(400, true).left == 0);
  ReversalGuard g;
  assert(g.update(100, 0) == 100);
  assert(g.update(-100, 10) == 0);
  assert(g.update(-100, 89) == 0);
  assert(g.update(-100, 90) == -100);
  assert(g.update(0, 100) == 0);
  assert(g.update(100, 179) == 0);
  assert(g.update(100, 180) == 100);
  ReversalGuard w;
  assert(w.update(100, UINT32_MAX - 49) == 100);
  assert(w.update(-100, UINT32_MAX - 39) == 0);
  assert(w.update(-100, 39) == 0);
  assert(w.update(-100, 40) == -100);
  std::cout << "Racing car control tests passed\n";
}
