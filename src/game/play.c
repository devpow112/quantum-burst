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
#include "game.h"
#include "maps.h"
#include "player.h"
#include "sprites.h"
#include "stage.h"
#include "utilities.h"

static Camera g_camera;
static Player g_player;
static Stage g_stage;
static bool g_paused;

static void joyHandlerGamePlay(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_paused = !g_paused;

    if (g_paused) {
      return;  // don't process any more input events
    }
  }

  if (_state & _changed & BUTTON_A) {
    doPlayerHit(&g_player, 10);
  }
}

static void setUpGamePlay() {
  JOY_setEventHandler(&joyHandlerGamePlay);
  PAL_setPalette(PAL0, k_stage1Palette.data);
  PAL_setPalette(PAL2, k_primarySpritePalette.data);
  PAL_setColor(0, RGB24_TO_VDPCOLOR(0x666666));

  g_paused = FALSE;

  setUpStage(&g_stage, PAL0);
  setUpPlayer(&g_player, PAL1, &g_stage);
  setUpCamera(&g_camera, &g_player, &g_stage);
}

static void updateGamePlay() {
#ifdef DEBUG
  VDP_showFPS(TRUE);
#endif

  SPR_update();
  SYS_doVBlankProcess();
}

static void tearDownGamePlay() {
  tearDownPlayer(&g_player);
  tearDownCamera(&g_camera);
  tearDownStage(&g_stage);
  JOY_setEventHandler(NULL);
}

void processGamePlay() {
  setUpGamePlay();

  while (isGameState(STATE_PLAY)) {
    if (!g_paused) {
      updateStage(&g_stage);
      updatePlayer(&g_player, &g_stage);
      updateCamera(&g_camera, &g_player, &g_stage);

      if (isPlayerDead(&g_player)) {
        setGameState(STATE_MENU);
      }
    }

    drawPlayer(&g_player, &g_camera);
    drawStage(&g_stage, &g_camera);
    updateGamePlay();
  }

  tearDownGamePlay();
}
