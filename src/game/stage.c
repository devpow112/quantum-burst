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
#include "sprites.h"
#include "utilities.h"

static Player g_player;
static bool g_paused;

static void joyHandlerGameStage(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_paused = !g_paused;

    if (g_paused) {
      showText("PAUSED", 14);
      PAL_fadeOutPalette(PAL1, 3, TRUE);
    } else {
      clearText(14);
      PAL_fadeInPalette(PAL1, k_primarySpritePalette.data, 3, TRUE);
    }
  }
}

static void setUpGameStage() {
  JOY_setEventHandler(&joyHandlerGameStage);
  PAL_setColor(0, RGB24_TO_VDPCOLOR(0x888888));
  PAL_setPalette(PAL1, k_primarySpritePalette.data);
  setUpPlayer(&g_player, PAL1);

  g_paused = FALSE;
}

static void updateGameStage() {
#ifdef DEBUG
  VDP_showFPS(TRUE);
#endif

  SPR_update();
  SYS_doVBlankProcess();
}

static void tearDownGameStage() {
  tearDownPlayer(&g_player);
  SPR_update();
  SYS_doVBlankProcess();
}

void processGameStage() {
  setUpGameStage();

  while (isGameState(STATE_STAGE)) {
    if (!g_paused) {
      updatePlayer(&g_player);
    }

    updateGameStage();
  }

  tearDownGameStage();
}
