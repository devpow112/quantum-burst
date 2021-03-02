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

#include "player.h"
#include "sprites.h"
#include "utilities.h"

#define PLAYER_VELOCITY 2
#define PLAYER_FIRE_DELAY 3
#define PLAYER_WIDTH 200
#define PLAYER_HEIGHT 48

void setUpPlayer(Player* _player, s16 _x, s16 _y, u16 _palette) {
  PAL_setPalette(_palette, titleSprite.palette->data);

  _player->position.x = _x;
  _player->position.y = _y;
  _player->fireDelay = 0;
  _player->sprite = SPR_addSpriteSafe(&titleSprite, _x, _y,
                                      TILE_ATTR(_palette, 0, FALSE, FALSE));
}

void updatePlayer(Player* _player) {
  const u16 inputState = JOY_readJoypad(JOY_1);

  if (inputState & BUTTON_LEFT) {
    _player->position.x -= PLAYER_VELOCITY;
  }

  if (inputState & BUTTON_RIGHT) {
    _player->position.x += PLAYER_VELOCITY;
  }

  if (inputState & BUTTON_UP) {
    _player->position.y -= PLAYER_VELOCITY;
  }

  if (inputState & BUTTON_DOWN) {
    _player->position.y += PLAYER_VELOCITY;
  }

  const s16 width = VDP_getScreenWidth();

  if (_player->position.x < 0) {
    _player->position.x = 0;
  } else if (_player->position.x > (width - PLAYER_WIDTH)) {
    _player->position.x = width - PLAYER_WIDTH;
  }

  const s16 height = VDP_getScreenHeight();

  if (_player->position.y < 0) {
    _player->position.y = 0;
  } else if (_player->position.y > (height - PLAYER_HEIGHT)) {
    _player->position.y = height - PLAYER_HEIGHT;
  }

  if ((inputState & BUTTON_A) && _player->fireDelay == 0) {
    showText("FIRE", 0);

    _player->fireDelay = PLAYER_FIRE_DELAY;
  } else if (_player->fireDelay != 0) {
    clearText(0);

    _player->fireDelay--;
  }

  SPR_setPosition(_player->sprite, _player->position.x, _player->position.y);
}

void tearDownPlayer(Player* _player) {
  SPR_releaseSprite(_player->sprite);
}
