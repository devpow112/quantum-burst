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
#include <string.h>

#include "sprites.h"

enum GAME_STATE {
  STATE_LOGO,
  STATE_TITLE,
  STATE_MENU,
  STATE_PLAY,
  STATE_CREDITS
};

enum GAME_STATE g_game_state;
bool g_play_paused;

void show_text(char _text[], u8 _column) {
  VDP_drawText(_text, 20 - strlen(_text) / 2, _column);
}

void clear_text(u8 _column) {
  VDP_clearText(0, _column, 32);
}

void init(bool _hard_reset) {
  if (IS_PALSYSTEM) {
    VDP_setScreenHeight240();
  } else {
    VDP_setScreenHeight224();
  }

  VDP_setScreenWidth320();
  JOY_init();
  SPR_init();

  if (!_hard_reset) {
    SPR_reset();

    if (g_game_state > STATE_TITLE) {
      g_game_state = STATE_TITLE;
    }
  } else {
    g_game_state = STATE_LOGO;
  }
}

void joy_handler_menu(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_game_state = STATE_PLAY;
  }
}

void joy_handler_play(u16 _joy, u16 _changed, u16 _state) {
  if (_state & _changed & BUTTON_START) {
    g_play_paused = !g_play_paused;
  }

  if (g_play_paused) {
    if (_state & _changed & BUTTON_A) {
      g_game_state = STATE_TITLE;
    }

    return;  // don't process more input
  }

  if (_state & _changed & BUTTON_B) {
    g_game_state = STATE_CREDITS;
  }
}

void process_state_logo() {
  JOY_setEventHandler(NULL);
  show_text("LOGO", 14);

  u8 timer = 200;

  while (g_game_state == STATE_LOGO) {
    timer--;

    if (timer == 0) {
      g_game_state = STATE_TITLE;
    }

    SYS_doVBlankProcess();
  }

  clear_text(14);
}

void process_state_title() {
  JOY_setEventHandler(NULL);
  show_text("TITLE", 14);

  u8 timer = 200;

  while (g_game_state == STATE_TITLE) {
    timer--;

    if (timer == 0) {
      g_game_state = STATE_MENU;
    }

    SYS_doVBlankProcess();
  }

  clear_text(14);
}

void process_state_menu() {
  JOY_setEventHandler(&joy_handler_menu);
  PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
  show_text("PRESS START BUTTON", 16);

  u16 screen_width = VDP_getScreenWidth();
  u16 position_x = (screen_width - 200) / 2;
  Sprite* title = SPR_addSprite(&title_sprite, position_x, 64,
                                TILE_ATTR(PAL1, 0, FALSE, FALSE));

  VDP_setPalette(PAL1, title_sprite.palette->data);

  while (g_game_state == STATE_MENU) {
    SPR_update();
    SYS_doVBlankProcess();
  }

  clear_text(16);
  SPR_releaseSprite(title);
  SPR_update();
}

void process_state_play() {
  JOY_setEventHandler(&joy_handler_play);

  g_play_paused = FALSE;

  while (g_game_state == STATE_PLAY) {
    if (!g_play_paused) {
      clear_text(14);
      show_text("PLAY", 14);
    } else {
      show_text("PAUSED", 14);
    }

    SYS_doVBlankProcess();
  }

  clear_text(14);
}

void process_state_credits() {
  JOY_setEventHandler(NULL);
  PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
  show_text("CREDITS", 14);

  u16 timer = 500;

  while (g_game_state == STATE_CREDITS) {
    timer--;

    if (timer == 0) {
      g_game_state = STATE_TITLE;
    }

    SYS_doVBlankProcess();
  }

  clear_text(14);
}

int main(bool _hard_reset) {
  init(_hard_reset);

  while (TRUE) {
    switch (g_game_state) {
      case STATE_LOGO:
        process_state_logo();
        break;
      case STATE_TITLE:
        process_state_title();
        break;
      case STATE_MENU:
        process_state_menu();
        break;
      case STATE_PLAY:
        process_state_play();
        break;
      case STATE_CREDITS:
        process_state_credits();
        break;
    }
  }

  return 0;
}
