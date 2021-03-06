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

// player constants
#define PLAYER_SCREEN_BUFFER 2
#define PLAYER_FIRE_DELAY_COOLDOWN_DEFAULT 0
#define PLAYER_FIRE_DELAY_COOLDOWN_DURATION 3
#define PLAYER_BANKING_DIRECTION_DEFAULT (intToFix16(0))
#define PLAYER_BANKING_DIRECTION_MAX_RIGHT (intToFix16(2))
#define PLAYER_BANKING_DIRECTION_MAX_LEFT (-PLAYER_BANKING_DIRECTION_MAX_RIGHT)

// player global properties
static V2f16 g_playerMinPosition;
static V2f16 g_playerMaxPosition;
static V2f16 g_playerStartPosition;
static f16 g_playerVelocity;

// player global flags
static bool g_playerVisibilityPulse;

// player animation frame rates
static f16 g_playerBankingFrameRate;

static void processPlayerMovement(Player* _player, u16 _inputState) {
  V2f16 position = _player->position;
  f16 bankDirection = _player->bankDirection;
  s8 movementDirection = 0;

  if (_inputState & BUTTON_LEFT) {
    position.x = fix16Sub(position.x, g_playerVelocity);
    movementDirection--;

    if (bankDirection > PLAYER_BANKING_DIRECTION_MAX_LEFT) {
      bankDirection = fix16Sub(bankDirection, g_playerBankingFrameRate);
    }
  }

  if (_inputState & BUTTON_RIGHT) {
    position.x = fix16Add(position.x, g_playerVelocity);
    movementDirection++;

    if (bankDirection < PLAYER_BANKING_DIRECTION_MAX_RIGHT) {
      bankDirection = fix16Add(bankDirection, g_playerBankingFrameRate);
    }
  }

  if (_inputState & BUTTON_UP) {
    position.y = fix16Sub(position.y, g_playerVelocity);
  }

  if (_inputState & BUTTON_DOWN) {
    position.y = fix16Add(position.y, g_playerVelocity);
  }

  position.x = clamp(position.x, g_playerMinPosition.x, g_playerMaxPosition.x);
  position.y = clamp(position.y, g_playerMinPosition.y, g_playerMaxPosition.y);

  if (movementDirection == 0) {
    if (bankDirection < PLAYER_BANKING_DIRECTION_DEFAULT) {
      bankDirection = fix16Add(bankDirection, g_playerBankingFrameRate);
    } else if (bankDirection > PLAYER_BANKING_DIRECTION_DEFAULT) {
      bankDirection = fix16Sub(bankDirection, g_playerBankingFrameRate);
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

static void processPlayerDamage(Player* _player) {
  if (_player->isInvulnerable) {
    g_playerVisibilityPulse = !g_playerVisibilityPulse;

    SPR_setVisibility(_player->sprite, g_playerVisibilityPulse);
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

  const u16 positionXRounded = fix16ToRoundedInt(_player->position.x);
  const u16 positionYRounded = fix16ToRoundedInt(_player->position.y);

  SPR_setPosition(_player->sprite, positionXRounded, positionYRounded);
}

void initPlayer() {
  const u16 width = VDP_getScreenWidth();
  const u16 height = VDP_getScreenHeight();
  const f16 frameRate =
      intToFix16(IS_PALSYSTEM ? PAL_FRAME_RATE : NTSC_FRAME_RATE);
  const f16 buffer = intToFix16(PLAYER_SCREEN_BUFFER);

  g_playerMinPosition.x = buffer;
  g_playerMinPosition.y = buffer;
  g_playerMaxPosition.x = fix16Sub(intToFix16(width - k_shipSprite.w), buffer);
  g_playerMaxPosition.y = fix16Sub(intToFix16(height - k_shipSprite.h), buffer);
  g_playerStartPosition.x = intToFix16((width - k_shipSprite.w) / 2);
  g_playerStartPosition.y = intToFix16(height - k_shipSprite.h);
  g_playerVelocity = fix16Div(intToFix16(120), frameRate);
  g_playerVisibilityPulse = FALSE;
  g_playerBankingFrameRate = fix16Div(intToFix16(20), frameRate);
}

Player* createPlayer(s16 _x, s16 _y, u16 _palette) {
  Player* player = malloc(sizeof(Player));

  setUpPlayer(player, _x, _y, _palette);

  return player;
}

void setUpPlayer(Player* _player, s16 _x, s16 _y, u16 _palette) {
  PAL_setPalette(_palette, k_shipSprite.palette->data);

  _player->position = g_playerStartPosition;
  _player->attackCooldown = PLAYER_FIRE_DELAY_COOLDOWN_DEFAULT;
  _player->bankDirection = PLAYER_BANKING_DIRECTION_DEFAULT;
  _player->isInvulnerable = FALSE;
  _player->sprite = SPR_addSpriteSafe(&k_shipSprite, _x, _y,
                                      TILE_ATTR(_palette, 0, FALSE, FALSE));
}

void updatePlayer(Player* _player) {
  const u16 inputState = JOY_readJoypad(JOY_1);

  processPlayerMovement(_player, inputState);
  processPlayerDamage(_player);
  processPlayerAttack(_player, inputState);
  updatePlayerPositionAndAnimation(_player);
}

void tearDownPlayer(Player* _player) {
  SPR_releaseSprite(_player->sprite);
}

void destroyPlayer(Player* _player) {
  tearDownPlayer(_player);
  free(_player);
}
