// MIT License
//
// Copyright (c) 2025 Devon Powell
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

  _stage->map = MAP_create(&k_stage1Map, BG_B, attributes);

  const f32 fps = FIX32(getFrameRate());
  const f32 screenWidth = FIX32(VDP_getScreenWidth());

  _stage->width = k_stage1Map.w * 128;
  _stage->height = k_stage1Map.h * 128;
  _stage->minimumX = 0;
  _stage->maximumX = _stage->minimumX + screenWidth;
  _stage->speed = F32_div(FIX32(120), fps);

  const V2f32 position = {
    0,                         // x
    FIX32(_stage->height / 2)  // y
  };

  _stage->startPosition = position;
}

void updateStage(Stage* _stage) {
  const f32 width = FIX32(_stage->width);
  const f32 speed = _stage->speed;
  const f32 screenWidth = FIX32(VDP_getScreenWidth());
  const f32 minimumXLow = 0;
  const f32 minimumXHigh = width - screenWidth;
  const f32 maximumXLow = screenWidth;
  const f32 maximumXHigh = width;
  f32 minimumX = _stage->minimumX;
  f32 maximumX = _stage->maximumX;

  minimumX = clamp(minimumX + speed, minimumXLow, minimumXHigh);
  maximumX = clamp(maximumX + speed, maximumXLow, maximumXHigh);
  _stage->minimumX = minimumX;
  _stage->maximumX = maximumX;
}

void drawStage(const Stage* _stage, const Camera* _camera) {
  const V2s32 position = getCameraPositionRounded(_camera);

  MAP_scrollTo(_stage->map, position.x, position.y);
}

void tearDownStage(Stage* _stage) {
  MAP_release(_stage->map);
}
