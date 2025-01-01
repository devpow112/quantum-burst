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

#include "actor.h"
#include "actors/enemies/homing-mine.h"
#include "actors/enemies/mine.h"
#include "actors/player.h"
#include "camera.h"
#include "game.h"
#include "managed-actor.h"
#include "maps.h"
#include "sprites.h"
#include "stage.h"
#include "utilities.h"

// global entities

static Stage g_stage;
static Camera g_camera;
static Actor* g_player = NULL;

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
}

static V2f32 cameraPositionCallback() {
  const f32 playerPositionY = getActorPositionY(g_player);
  const f32 halfScreenHeight = intToFix32(VDP_getScreenHeight() / 2);
  const f32 minimumY = halfScreenHeight;
  const f32 maximumY = intToFix32(g_stage.height) - halfScreenHeight;
  const V2f32 position = {
    fix32Avg(g_stage.minimumX, g_stage.maximumX),  // x
    clamp(playerPositionY, minimumY, maximumY)     // y
  };

  return position;
}

static void setUpActors(const Stage* _stage, u16 _palette) {
  g_player = createPlayer(PAL2, _stage->startPosition);

  const V2f32 mine1Position = {
    _stage->startPosition.x + intToFix32(200),  // x
    _stage->startPosition.y - intToFix32(100)   // y
  };
  const V2f32 mine2Position = {
    _stage->startPosition.x + intToFix32(400),  // x
    _stage->startPosition.y + intToFix32(100)   // y
  };
  const V2f32 mine3Position = {
    _stage->startPosition.x + intToFix32(600),  // x
    _stage->startPosition.y - intToFix32(100)   // y
  };

  createMine(_palette, mine1Position, g_player);
  createHomingMine(_palette, mine2Position, g_player);
  createHomingMine(_palette, mine3Position, g_player);
}

static void updateActors(const Stage* _stage) {
  updateActor(g_player, _stage);
  updateManagedActors(_stage);
}

static void drawActors(const Camera* _camera) {
  drawManagedActors(_camera);
  drawActor(g_player, _camera);
}

static void tearDownActors() {
  destroyManagedActors();
  destroyActor(g_player);

  g_player = NULL;
}

static void setUpGamePlay() {
  JOY_setEventHandler(&joyHandlerGamePlay);
  VDP_resetScreen();
  PAL_setPalette(PAL1, k_stage1Palette.data, DMA);
  PAL_setPalette(PAL2, k_primarySpritePalette.data, DMA);
  setUpStage(&g_stage, PAL1);
  setUpActors(&g_stage, PAL2);
  setUpCamera(&g_camera, &cameraPositionCallback, TRUE);

  g_paused = FALSE;
}

static void updateGamePlay() {
  SPR_update();

#ifdef DEBUG
  VDP_showFPS(FALSE, 1, 1);
  VDP_showCPULoad(1, 2);
#endif

  SYS_doVBlankProcess();
}

static void tearDownGamePlay() {
  JOY_setEventHandler(NULL);
  tearDownCamera(&g_camera);
  tearDownActors();
  tearDownStage(&g_stage);
  updateGamePlay();
}

// public functions

void processGamePlay() {
  setUpGamePlay();

  while (isGameState(STATE_PLAY)) {
    if (!g_paused) {
      updateStage(&g_stage);
      updateActors(&g_stage);
      updateCamera(&g_camera);

      if (isPlayerDead(g_player)) {
        setGameState(STATE_CREDITS);
      }
    }

    drawStage(&g_stage, &g_camera);
    drawActors(&g_camera);
    updateGamePlay();
  }

  tearDownGamePlay();
}
