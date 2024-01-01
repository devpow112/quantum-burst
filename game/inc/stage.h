// MIT License
//
// Copyright (c) 2024 Devon Powell
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

#ifndef __QUANTUM_BURST_STAGE_H__
#define __QUANTUM_BURST_STAGE_H__

#include <genesis.h>

#include "camera.h"

// entity

typedef struct {
  V2f32 startPosition;
  Map* map;
  u32 width;
  u32 height;
  f32 minimumX;
  f32 maximumX;
  f32 speed;
} Stage;

// life-cycle

void initStage();

void setUpStage(Stage* _stage, u16 _palette);

void updateStage(Stage* _stage);

void drawStage(const Stage* _stage, const Camera* _camera);

void tearDownStage(Stage* _stage);

#endif  // __QUANTUM_BURST_STAGE_H__
