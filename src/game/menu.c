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

#define LOGO_FADE_IN_TIME 300   // milliseconds
#define LOGO_FADE_OUT_TIME 500  // milliseconds
#define LOGO_END_POSITION_Y 64  // screen coordinate
#define FLASH_TIME 100          // milliseconds

static bool g_run_menu_exit;

static void joyHandlerMenu(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_run_menu_exit = TRUE;
  }
}

static void doFlash() {
  const u8 flashFrameCount = timeToFrames(FLASH_TIME);
  u16 flashPallet[32] = {0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
                         0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
                         0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
                         0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
                         0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF};

  PAL_fadeTo(0, 31, flashPallet, flashFrameCount, TRUE);

  while (PAL_isDoingFade()) {
    SPR_update();
    SYS_doVBlankProcess();
  }

  memset(flashPallet, palette_black[0], sizeof(u16) * 15);
  memcpy(&flashPallet[16], k_titleSprite.palette->data, sizeof(u16) * 16);
  PAL_fadeTo(0, 31, flashPallet, flashFrameCount, TRUE);

  while (PAL_isDoingFade()) {
    SPR_update();
    SYS_doVBlankProcess();
  }
}

void processGameMenu() {
  JOY_setEventHandler(NULL);

  const u16 screenWidth = VDP_getScreenWidth();
  const s16 titlePositionX = (screenWidth - k_titleSprite.w) / 2;
  f16 titlePositionY = intToFix16(-k_titleSprite.h);
  Sprite* title = SPR_addSpriteSafe(&k_titleSprite, titlePositionX,
                                    fix16ToInt(titlePositionY),
                                    TILE_ATTR(PAL1, 0, FALSE, FALSE));
  const u16 fadeInFrameCount = timeToFrames(LOGO_FADE_IN_TIME);

  PAL_fadeInPalette(PAL1, k_titleSprite.palette->data, fadeInFrameCount, TRUE);

  const f16 distance =
      fix16Sub(intToFix16(LOGO_END_POSITION_Y), titlePositionY);
  const f16 increment = fix16Div(distance, intToFix16(fadeInFrameCount));

  while (PAL_isDoingFade()) {
    titlePositionY = fix16Add(titlePositionY, increment);

    SPR_setPosition(title, titlePositionX, fix16ToInt(titlePositionY));
    SPR_update();
    SYS_doVBlankProcess();
  }

  SPR_setPosition(title, titlePositionX, LOGO_END_POSITION_Y);
  doFlash();
  showText("PRESS START BUTTON", 16);
  JOY_setEventHandler(&joyHandlerMenu);

  g_run_menu_exit = FALSE;

  while (!g_run_menu_exit) {
    SPR_update();
    SYS_doVBlankProcess();
  }

  clearText(16);
  JOY_setEventHandler(NULL);
  doFlash();
  PAL_fadeOutPalette(PAL1, timeToFrames(LOGO_FADE_OUT_TIME), TRUE);

  while (PAL_isDoingFade()) {
    SPR_update();
    SYS_doVBlankProcess();
  }

  setGameState(STATE_STAGE);
  SPR_releaseSprite(title);
  SPR_update();
  SYS_doVBlankProcess();
}
