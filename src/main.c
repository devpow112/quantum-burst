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

#include "actors/enemies/mine-homing.h"
#include "actors/enemies/mine.h"
#include "actors/player.h"
#include "camera.h"
#include "game.h"
#include "stage.h"
#include "utilities.h"

// private functions

static void init(bool _hardReset) {
  VDP_init();
  VDP_setScreenWidth320();

  if (IS_PALSYSTEM) {
    VDP_setScreenHeight240();
  } else {
    VDP_setScreenHeight224();
  }

  SPR_init();

  if (_hardReset) {
    setGameState(STATE_LOGO);
  } else if (!isGameState(STATE_LOGO)) {
    setGameState(STATE_MENU);
  }

  initUtilities();
  initStage();
  initCamera();
  initPlayer();
  initMine();
  initMineHoming();
}

// program entry

int main(bool _hardReset) {
  init(_hardReset);

  while (TRUE) {
    switch (getGameState()) {
      case STATE_LOGO:
        processGameLogo();
        break;
      case STATE_MENU:
        processGameMenu();
        break;
      case STATE_LOAD:
        processGameLoad();
        break;
      case STATE_PLAY:
        processGamePlay();
        break;
      case STATE_CREDITS:
        processGameCredits();
        break;
      default:
        return 1;
    }
  }

  return 0;
}
