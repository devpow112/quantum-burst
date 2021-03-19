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

#include "actor.h"
#include "actors/player.h"
#include "camera.h"
#include "game.h"
#include "maps.h"
#include "sprites.h"
#include "stage.h"
#include "utilities.h"

// global entities

static Stage g_stage;
static Camera g_camera;
static Actor* g_player;

// global properties

static bool g_paused;

// private functions

static void joyHandlerGamePlay(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_paused = !g_paused;

    if (g_paused) {
      return;  // don't process any more input events
    }
  }

  if (_state & _changed & BUTTON_A) {
    doPlayerHit(g_player, 10);
  }
}

static V2f32 cameraPositionCallback() {
  const f32 playerPositionY = getActorPositionY(g_player);
  const f32 halfScreenHeight = intToFix32(VDP_getScreenHeight() / 2);
  const f32 minimumY = halfScreenHeight;
  const f32 maximumY = fix32Sub(intToFix32(g_stage.height), halfScreenHeight);
  const V2f32 position = {
    fix32Avg(g_stage.minimumX, g_stage.maximumX),  // x
    clamp(playerPositionY, minimumY, maximumY)     // y
  };

  return position;
}

// public functions

static void setUpGamePlay() {
  JOY_setEventHandler(&joyHandlerGamePlay);
  PAL_setPalette(PAL0, k_stage1Palette.data);
  PAL_setPalette(PAL1, k_primarySpritePalette.data);
  setUpStage(&g_stage, PAL0);
  setUpCamera(&g_camera, &cameraPositionCallback);

  g_player = createPlayer(PAL1, g_stage.startPosition);
  g_paused = FALSE;
}

static void updateGamePlay() {
#ifdef DEBUG
  VDP_showFPS(TRUE);
#endif

  SPR_update();
  SYS_doVBlankProcess();
}

static void tearDownGamePlay() {
  destroyActors();
  tearDownCamera(&g_camera);
  tearDownStage(&g_stage);
  JOY_setEventHandler(NULL);
}

void processGamePlay() {
  setUpGamePlay();

  while (isGameState(STATE_PLAY)) {
    if (!g_paused) {
      updateStage(&g_stage);
      updateActors(&g_stage);
      updateCamera(&g_camera);

      if (isPlayerDead(g_player)) {
        setGameState(STATE_MENU);
      }
    }

    drawStage(&g_stage, &g_camera);
    drawActors(&g_camera);
    updateGamePlay();
  }

  tearDownGamePlay();
}
