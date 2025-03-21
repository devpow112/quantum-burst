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

#include "game.h"
#include "utilities.h"

// private functions

static void setUpGameCredits() {
  JOY_setEventHandler(NULL);
  VDP_resetScreen();
  PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
}

// public functions

void processGameCredits() {
  setUpGameCredits();
  showText("CREDITS", 14);

  u16 timer = 500;

  while (isGameState(STATE_CREDITS)) {
    timer--;

    if (timer == 0) {
      setGameState(STATE_MENU);
      clearText(14);
    }

    SYS_doVBlankProcess();
  }
}
