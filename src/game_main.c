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

#include "game.h"
#include "utilities.h"

bool g_paused;

void joyHandlerGameMain(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_paused = !g_paused;
  }

  if (g_paused) {
    if (_state & _changed & BUTTON_A) {
      setGameState(STATE_MENU);
    }

    return;  // don't process more input
  }

  if (_state & _changed & BUTTON_B) {
    setGameState(STATE_CREDITS);
  }
}

void processGameMain() {
  JOY_setEventHandler(&joyHandlerGameMain);

  g_paused = FALSE;

  while (isGameState(STATE_MAIN)) {
    if (!g_paused) {
      clearText(14);
      showText("PLAY", 14);
    } else {
      showText("PAUSED", 14);
    }

    SYS_doVBlankProcess();
  }

  clearText(14);
}
