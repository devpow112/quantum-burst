// MIT License
//
// Copyright (c) 2021 Devon Powell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <genesis.h>

#include "collision.h"

bool doTriangleCircleCollision(TriangleCollision _triangleCollision,
                               V2s32 _trianglePosition,
                               CircleCollision _circleCollision,
                               V2s32 _circlePosition) {
  const s32 r = _circleCollision.ray;
  const V2s32 c = {
    _circleCollision.x + _circlePosition.x,  // x
    _circleCollision.y + _circlePosition.y   // y
  };
  const V2s32 t1 = {
    _triangleCollision.p1.x + _trianglePosition.x,  // x
    _triangleCollision.p1.y + _trianglePosition.y   // y
  };
  const s32 d1 = getApproximatedDistance(c.x - t1.x, c.y - t1.y);

  if (d1 <= r) {
    return TRUE;
  }

  const V2s32 t2 = {
    _triangleCollision.p2.x + _trianglePosition.x,  // x
    _triangleCollision.p2.y + _trianglePosition.y   // y
  };
  const s32 d2 = getApproximatedDistance(c.x - t2.x, c.y - t2.y);

  if (d2 <= r) {
    return TRUE;
  }

  const V2s32 t3 = {
    _triangleCollision.p3.x + _trianglePosition.x,  // x
    _triangleCollision.p3.y + _trianglePosition.y   // y
  };
  const s32 d3 = getApproximatedDistance(c.x - t3.x, c.y - t3.y);

  return d3 <= r;
}
