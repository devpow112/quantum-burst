// MIT License
//
// Copyright (c) 2026 Devon Powell
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

#include "actors/enemies/homing_mine.h"
#include "actors/enemies/mine.h"
#include "actors/player.h"
#include "camera.h"
#include "game.h"
#include "log.h"
#include "managed_actor.h"
#include "stage.h"
#include "utilities.h"

// private functions

static void init(bool _hardReset) {
  log("initializing subsystems...");

  initUtilities();
  initStage();
  initCamera();
  initPlayer();
  initMine();
  initHomingMine();
  initManagedActors();

  log("initializing subsystems...done");

  if (_hardReset) {
    log("hard reset: yes");

    setGameState(STATE_LOGO);
  } else if (!isGameState(STATE_LOGO)) {
    log("hard reset: no");

    setGameState(STATE_MENU);
  }

  log("initializing vdp...");

  VDP_init();
  VDP_setScreenWidth320();

  if (IS_PAL_SYSTEM) {
    log("system type: pal");

    VDP_setScreenHeight240();
  } else {
    log("system type: ntsc");

    VDP_setScreenHeight224();
  }

  log("initializing vdp...done");
  log("initializing sprite engine...");

  SPR_init();

  log("initializing sprite engine...done");
}

// program entry

int main(bool _hardReset) {
  init(_hardReset);

  while (TRUE) {
    switch (getGameState()) {
      case STATE_LOGO:
        log("game state: logo");

        processGameLogo();

        break;
      case STATE_MENU:
        log("game state: menu");

        processGameMenu();

        break;
      case STATE_LOAD:
        log("game state: load");

        processGameLoad();

        break;
      case STATE_PLAY:
        log("game state: play");

        processGamePlay();

        break;
      case STATE_CREDITS:
        log("game state: credits");

        processGameCredits();

        break;
      default:
        return 1;
    }
  }

  return 0;
}
