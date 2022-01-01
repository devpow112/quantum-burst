// MIT License
//
// Copyright (c) 2022 Devon Powell
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

// constants

#define LOGO_FADE_IN_TIME (FIX16(0.3))   // seconds
#define LOGO_FADE_OUT_TIME (FIX16(0.5))  // seconds
#define FLASH_TIME (FIX16(0.1))          // seconds
#define LOGO_END_POSITION_Y 64           // pixels

// global properties

static bool g_runMenuExit;

// private functions

static void joyHandlerMenu(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_runMenuExit = TRUE;
  }
}

static void doFlash() {
  const u16 flashFrameCount = secondsToFrames(FLASH_TIME);
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

// public functions

void processGameMenu() {
  JOY_setEventHandler(NULL);
  VDP_resetScreen();

  const u16 screenWidth = VDP_getScreenWidth();
  const s16 titlePositionX = (screenWidth - k_titleSprite.w) / 2;
  f32 titlePositionY = intToFix32(-k_titleSprite.h);
  Sprite* title = SPR_addSpriteSafe(&k_titleSprite, titlePositionX,
                                    fix32ToInt(titlePositionY),
                                    TILE_ATTR(PAL1, 0, FALSE, FALSE));
  const u16 fadeInFrameCount = secondsToFrames(LOGO_FADE_IN_TIME);

  PAL_fadeInPalette(PAL1, k_titleSprite.palette->data, fadeInFrameCount, TRUE);

  const f32 logoEndPositionY = intToFix32(LOGO_END_POSITION_Y);
  const f32 distance = fix32Sub(logoEndPositionY, titlePositionY);
  const f32 increment = fix32Div(distance, intToFix32(fadeInFrameCount));

  while (PAL_isDoingFade()) {
    titlePositionY = fix32Add(titlePositionY, increment);

    SPR_setPosition(title, titlePositionX, fix32ToInt(titlePositionY));
    SPR_update();
    SYS_doVBlankProcess();
  }

  SPR_setPosition(title, titlePositionX, LOGO_END_POSITION_Y);
  doFlash();
  showText("PRESS START BUTTON", 16);
  JOY_setEventHandler(&joyHandlerMenu);

  g_runMenuExit = FALSE;

  while (!g_runMenuExit) {
    SPR_update();
    SYS_doVBlankProcess();
  }

  clearText(16);
  JOY_setEventHandler(NULL);
  doFlash();
  PAL_fadeOutPalette(PAL1, secondsToFrames(LOGO_FADE_OUT_TIME), TRUE);

  while (PAL_isDoingFade()) {
    SPR_update();
    SYS_doVBlankProcess();
  }

  setGameState(STATE_PLAY);
  SPR_releaseSprite(title);
  SPR_update();
  SYS_doVBlankProcess();
}
