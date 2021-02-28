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

#define PLAYER_VELOCITY 2

void setUpPlayer(Player* _player, s16 _x, s16 _y) {
  _player->position.x = _x;
  _player->position.y = _y;
  _player->sprite =
      SPR_addSpriteSafe(&titleSprite, _x, _y, TILE_ATTR(PAL1, 0, FALSE, FALSE));

  PAL_setPalette(PAL1, titleSprite.palette->data);
}

void updatePlayer(Player* _player) {
  u16 inputState = JOY_readJoypad(JOY_1);

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

  if (_player->position.x <= 0) {
    _player->position.x = 2;
  }

  if (_player->position.x >= 320 - 32) {
    _player->position.x = 320 - 32 - 2;
  }

  if (_player->position.y <= 0) {
    _player->position.y = 2;
  }

  if (_player->position.y >= 224 - 24) {
    _player->position.y = 224 - 24 - 2;
  }

  SPR_setPosition(_player->sprite, _player->position.x, _player->position.y);
  SPR_update();
}

void tearDownPlayer(Player* _player) {
  SPR_releaseSprite(_player->sprite);
}