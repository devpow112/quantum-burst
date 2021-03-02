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
#include "player.h"
#include "utilities.h"

static Player g_player;
static bool g_paused;

static void joyHandlerGameStage(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_paused = !g_paused;

    if (g_paused) {
      PAL_fadeOutPalette(PAL1, 3, TRUE);
    } else {
      PAL_fadeInPalette(PAL1, g_player.sprite->definition->palette->data, 3,
                        TRUE);
    }
  }
}

static void initializeGameStage() {
  JOY_setEventHandler(&joyHandlerGameStage);

  g_paused = FALSE;
}

void processGameStage() {
  initializeGameStage();
  setUpPlayer(&g_player, 0, 0, PAL1);

  while (isGameState(STATE_STAGE)) {
    clearText(14);

    if (!g_paused) {
      updatePlayer(&g_player);
    } else {
      showText("PAUSED", 14);
    }

    SPR_update();
    SYS_doVBlankProcess();
  }

  tearDownPlayer(&g_player);
  SPR_update();
  SYS_doVBlankProcess();
}
