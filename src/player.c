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

#include "maths.h"
#include "player.h"
#include "sprites.h"
#include "utilities.h"

// constants
#define PLAYER_VELOCITY 2
#define PLAYER_FIRE_DELAY_COOLDOWN_DEFAULT 0
#define PLAYER_FIRE_DELAY_COOLDOWN_DURATION 3
#define PLAYER_BANKING_DIRECTION_DEFAULT (FIX16(0))
#define PLAYER_BANKING_DIRECTION_MAX_RIGHT (FIX16(2))
#define PLAYER_BANKING_DIRECTION_MAX_LEFT (-PLAYER_BANKING_DIRECTION_MAX_RIGHT)
#define PLAYER_HORIZONTAL_BUFFER 2
#define PLAYER_VERTICAL_BUFFER PLAYER_HORIZONTAL_BUFFER

// animation frame rates
#define PLAYER_BANKING_FRAME_RATE (fix16Div(FIX16(20), FIX16(60)))

typedef struct {
  u8 w;  // width
  u8 h;  // height
} PlayerDimensions;

static PlayerDimensions g_playerBounds;
static V2u16 g_playerMinPosition;
static V2u16 g_playerMaxPosition;

static void processPlayerMovement(Player* _player, u16 _inputState) {
  V2s16 position = _player->position;
  f16 bankDirection = _player->bankDirection;
  s8 movementDirection = 0;

  if (_inputState & BUTTON_LEFT) {
    position.x -= PLAYER_VELOCITY;
    movementDirection--;

    if (bankDirection > PLAYER_BANKING_DIRECTION_MAX_LEFT) {
      bankDirection = fix16Sub(bankDirection, PLAYER_BANKING_FRAME_RATE);
    }
  }

  if (_inputState & BUTTON_RIGHT) {
    position.x += PLAYER_VELOCITY;
    movementDirection++;

    if (bankDirection < PLAYER_BANKING_DIRECTION_MAX_RIGHT) {
      bankDirection = fix16Add(bankDirection, PLAYER_BANKING_FRAME_RATE);
    }
  }

  if (_inputState & BUTTON_UP) {
    position.y -= PLAYER_VELOCITY;
  }

  if (_inputState & BUTTON_DOWN) {
    position.y += PLAYER_VELOCITY;
  }

  if (position.x < g_playerMinPosition.x) {
    position.x = g_playerMinPosition.x;
  } else if (position.x > g_playerMaxPosition.x) {
    position.x = g_playerMaxPosition.x;
  }

  if (position.y < g_playerMinPosition.y) {
    position.y = g_playerMinPosition.y;
  } else if (position.y > g_playerMaxPosition.y) {
    position.y = g_playerMaxPosition.y;
  }

  if (movementDirection == 0) {
    if (bankDirection < PLAYER_BANKING_DIRECTION_DEFAULT) {
      bankDirection = fix16Add(bankDirection, PLAYER_BANKING_FRAME_RATE);
    } else if (bankDirection > PLAYER_BANKING_DIRECTION_DEFAULT) {
      bankDirection = fix16Sub(bankDirection, PLAYER_BANKING_FRAME_RATE);
    }
  }

  _player->bankDirection = bankDirection;
  _player->position = position;
}

static void processPlayerAttack(Player* _player, u16 _inputState) {
  if ((_inputState & BUTTON_A) &&
      _player->attackCooldown == PLAYER_FIRE_DELAY_COOLDOWN_DEFAULT) {
    showText("FIRE", 0);

    _player->attackCooldown = PLAYER_FIRE_DELAY_COOLDOWN_DURATION;
  } else if (_player->attackCooldown != 0) {
    clearText(0);

    _player->attackCooldown--;
  }
}

static void updatePlayerPositionAndAnimation(Player* _player) {
  const s8 bankDirectionRounded = fix16ToRoundedInt(_player->bankDirection);
  const u8 bankDirectionIndex = abs(bankDirectionRounded);

  SPR_setHFlip(_player->sprite, bankDirectionRounded > 0);

  if (bankDirectionIndex > 0) {
    SPR_setAnimAndFrame(_player->sprite, 1, bankDirectionIndex - 1);
  } else {
    SPR_setAnim(_player->sprite, 0);
  }

  SPR_setPosition(_player->sprite, _player->position.x, _player->position.y);
}

void initPlayer() {
  g_playerBounds.w = k_shipSprite.w + (PLAYER_HORIZONTAL_BUFFER * 2);
  g_playerBounds.h = k_shipSprite.h + (PLAYER_VERTICAL_BUFFER * 2);
  g_playerMinPosition.x = PLAYER_HORIZONTAL_BUFFER;
  g_playerMinPosition.y = PLAYER_VERTICAL_BUFFER;
  g_playerMaxPosition.x = VDP_getScreenWidth() - g_playerBounds.w;
  g_playerMaxPosition.y = VDP_getScreenHeight() - g_playerBounds.h;
}

void setUpPlayer(Player* _player, s16 _x, s16 _y, u16 _palette) {
  PAL_setPalette(_palette, k_shipSprite.palette->data);

  _player->position.x = (VDP_getScreenWidth() - g_playerBounds.w) / 2;
  _player->position.y = VDP_getScreenHeight() - g_playerBounds.h;
  _player->attackCooldown = PLAYER_FIRE_DELAY_COOLDOWN_DEFAULT;
  _player->bankDirection = PLAYER_BANKING_DIRECTION_DEFAULT;
  _player->sprite = SPR_addSpriteSafe(&k_shipSprite, _x, _y,
                                      TILE_ATTR(_palette, 0, FALSE, FALSE));
}

void updatePlayer(Player* _player) {
  const u16 inputState = JOY_readJoypad(JOY_1);

  processPlayerMovement(_player, inputState);
  processPlayerAttack(_player, inputState);
  updatePlayerPositionAndAnimation(_player);
}

void tearDownPlayer(Player* _player) {
  SPR_releaseSprite(_player->sprite);
}
