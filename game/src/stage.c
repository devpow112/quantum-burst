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

#include "camera.h"
#include "maps.h"
#include "stage.h"
#include "utilities.h"

// public functions

void initStage() {
  // nothing so far
}

void setUpStage(Stage* _stage, u16 _palette) {
  const u16 attributes = TILE_ATTR(_palette, FALSE, FALSE, FALSE);

  _stage->map = MAP_create(&k_stage1Map, BG_A, attributes);

  const f32 fps = intToFix32(getFrameRate());
  const f32 screenWidth = intToFix32(VDP_getScreenWidth());

  _stage->width = k_stage1Map.w * 128;
  _stage->height = k_stage1Map.h * 128;
  _stage->minimumX = intToFix32(0);
  _stage->maximumX = fix32Add(_stage->minimumX, screenWidth);
  _stage->speed = fix32Div(intToFix32(120), fps);

  const V2f32 position = {
    0,                              // x
    intToFix32(_stage->height / 2)  // y
  };

  _stage->startPosition = position;
}

void updateStage(Stage* _stage) {
  const f32 width = intToFix32(_stage->width);
  const f32 speed = _stage->speed;
  const f32 screenWidth = intToFix32(VDP_getScreenWidth());
  const f32 minimumXLow = intToFix32(0);
  const f32 minimumXHigh = fix32Sub(width, screenWidth);
  const f32 maximumXLow = screenWidth;
  const f32 maximumXHigh = width;
  f32 minimumX = _stage->minimumX;
  f32 maximumX = _stage->maximumX;

  minimumX = clamp(fix32Add(minimumX, speed), minimumXLow, minimumXHigh);
  maximumX = clamp(fix32Add(maximumX, speed), maximumXLow, maximumXHigh);
  _stage->minimumX = minimumX;
  _stage->maximumX = maximumX;
}

void drawStage(const Stage* _stage, const Camera* _camera) {
  const V2s32 position = getCameraPositionRounded(_camera);

  MAP_scrollTo(_stage->map, position.x, position.y);
}

void tearDownStage(Stage* _stage) {
  MEM_free(_stage->map);
}
