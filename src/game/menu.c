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
#include "sprites.h"
#include "utilities.h"

void joyHandlerMenu(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    setGameState(STATE_MAIN);
  }
}

void processGameMenu() {
  JOY_setEventHandler(NULL);
  showText("MENU LEAD UP", 14);

  u8 timer = 200;

  while (isGameState(STATE_MENU)) {
    timer--;

    if (timer == 0) {
      break;
    }

    SYS_doVBlankProcess();
  }

  clearText(14);
  JOY_setEventHandler(&joyHandlerMenu);
  PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
  showText("PRESS START BUTTON", 16);

  u16 width = VDP_getScreenWidth();
  u16 x = (width - 200) / 2;
  Sprite* title =
      SPR_addSprite(&titleSprite, x, 64, TILE_ATTR(PAL1, 0, FALSE, FALSE));

  VDP_setPalette(PAL1, titleSprite.palette->data);

  while (isGameState(STATE_MENU)) {
    SPR_update();
    SYS_doVBlankProcess();
  }

  clearText(16);
  SPR_releaseSprite(title);
  SPR_update();
}
