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

#include "utilities.h"

// constants

#define PAL_FRAME_RATE 50
#define NTSC_FRAME_RATE 60

// global properties

static u8 g_utiltiesFrameRate;
static f32 g_utiltiesFrameDeltaTime;

// public functions

void initUtilities() {
  const u8 fps = IS_PALSYSTEM ? PAL_FRAME_RATE : NTSC_FRAME_RATE;

  g_utiltiesFrameRate = fps;
  g_utiltiesFrameDeltaTime = fix32Div(intToFix32(1), intToFix32(fps));
}

void showText(char _text[], u8 _column) {
  VDP_drawText(_text, 20 - strlen(_text) / 2, _column);
}

void clearText(u8 _column) {
  VDP_clearText(0, _column, 32);
}

u8 getFrameRate() {
  return g_utiltiesFrameRate;
}

f32 getFrameDeltaTime() {
  return g_utiltiesFrameDeltaTime;
}

u16 secondsToFrames(f16 _seconds) {
  return fix16ToRoundedInt(fix16Mul(_seconds, intToFix16(getFrameRate())));
}
