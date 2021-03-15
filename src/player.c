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

#include "camera.h"
#include "player.h"
#include "sprites.h"
#include "stage.h"
#include "utilities.h"

// player constants
#define PLAYER_SCREEN_BUFFER 2                               // pixels
#define PLAYER_ATTACK_COOLDOWN_DURATION (FIX32(0.05))        // seconds
#define PLAYER_INVULNERABILITY_COOLDOWN_DURATION (FIX32(1))  // seconds
#define PLAYER_BANKING_DIRECTION_DEFAULT (FIX16(0))
#define PLAYER_BANKING_DIRECTION_MAX_RIGHT (FIX16(2))
#define PLAYER_BANKING_DIRECTION_MAX_LEFT (-PLAYER_BANKING_DIRECTION_MAX_RIGHT)
#define PLAYER_HEALTH_DEFAULT 100

// player global properties
static V2u16 g_playerStartPosition;  // pixels
static V2u16 g_playerSpriteOffset;   // pixels
static V2f32 g_playerBuffer;         // pixels
static f32 g_playerVelocity;         // pixels/second

// player animation constants
static f16 g_playerBankingRate;  // fps

static void processPlayerMovement(Player* _player, const Stage* _stage) {
  V2f32 position = _player->position;
  const f32 previousPositionY = position.y;
  const u16 inputState = JOY_readJoypad(JOY_1);
  f16 bankDirection = _player->bankDirection;

  position.x = fix32Add(position.x, _stage->speed);

  if (inputState & BUTTON_LEFT) {
    position.x = fix32Sub(position.x, g_playerVelocity);
  }

  if (inputState & BUTTON_RIGHT) {
    position.x = fix32Add(position.x, g_playerVelocity);
  }

  if (inputState & BUTTON_UP) {
    position.y = fix32Sub(position.y, g_playerVelocity);
  }

  if (inputState & BUTTON_DOWN) {
    position.y = fix32Add(position.y, g_playerVelocity);
  }

  const f32 minimumX = fix32Add(_stage->minimumX, g_playerBuffer.x);
  const f32 maximumX = fix32Sub(_stage->maximumX, g_playerBuffer.x);
  const f32 minimumY = g_playerBuffer.y;
  const f32 maximumY = fix32Sub(intToFix32(_stage->height), g_playerBuffer.y);

  position.x = clamp(position.x, minimumX, maximumX);
  position.y = clamp(position.y, minimumY, maximumY);

  const s8 deltaY = fix32ToRoundedInt(fix32Sub(previousPositionY, position.y));

  if (deltaY == 0) {
    if (bankDirection < PLAYER_BANKING_DIRECTION_DEFAULT) {
      bankDirection = fix16Add(bankDirection, g_playerBankingRate);
    } else if (bankDirection > PLAYER_BANKING_DIRECTION_DEFAULT) {
      bankDirection = fix16Sub(bankDirection, g_playerBankingRate);
    }
  } else if (deltaY > 0 && bankDirection > PLAYER_BANKING_DIRECTION_MAX_LEFT) {
    bankDirection = fix16Sub(bankDirection, g_playerBankingRate);
  } else if (deltaY < 0 && bankDirection < PLAYER_BANKING_DIRECTION_MAX_RIGHT) {
    bankDirection = fix16Add(bankDirection, g_playerBankingRate);
  }

  _player->position = position;
  _player->bankDirection = bankDirection;
}

static void processPlayerAttack(Player* _player) {
  const u16 inputState = JOY_readJoypad(JOY_1);
  f32 attackCooldown = _player->attackCooldown;

  if (attackCooldown > intToFix32(0)) {
    attackCooldown = fix32Sub(attackCooldown, getFrameDeltaTime());
  } else if ((inputState & BUTTON_A)) {
    attackCooldown = PLAYER_ATTACK_COOLDOWN_DURATION;
  }

  _player->attackCooldown = attackCooldown;
}

static void processPlayerDamage(Player* _player) {
  f32 damageCooldown = _player->damageCooldown;

  if (damageCooldown > intToFix32(0)) {
    damageCooldown = fix32Sub(damageCooldown, getFrameDeltaTime());
  }

  _player->damageCooldown = damageCooldown;
}

void initPlayer() {
  const f32 fps = intToFix32(getFrameRate());
  const V2u16 spriteOffset = {k_shipSprite.w / 2, k_shipSprite.h / 2};
  const V2f32 buffer = {
    intToFix32(PLAYER_SCREEN_BUFFER + spriteOffset.x),  // x
    intToFix32(PLAYER_SCREEN_BUFFER + spriteOffset.y),  // y
  };

  g_playerSpriteOffset = spriteOffset;
  g_playerBuffer = buffer;
  g_playerVelocity = fix32Div(intToFix32(120), fps);
  g_playerBankingRate = fix32Div(intToFix32(20), fps);
}

void setUpPlayer(Player* _player, u16 _palette, const Stage* _stage) {
  _player->attackCooldown = PLAYER_ATTACK_COOLDOWN_DURATION;
  _player->bankDirection = PLAYER_BANKING_DIRECTION_DEFAULT;
  _player->damageCooldown = PLAYER_INVULNERABILITY_COOLDOWN_DURATION;
  _player->health = PLAYER_HEALTH_DEFAULT;
  _player->sprite = SPR_addSpriteSafe(&k_shipSprite, g_playerStartPosition.x,
                                      g_playerStartPosition.y,
                                      TILE_ATTR(_palette, TRUE, FALSE, FALSE));
  _player->position.x = g_playerBuffer.x;
  _player->position.y = intToFix32(_stage->height / 2);
}

void updatePlayer(Player* _player, const Stage* _stage) {
  if (isPlayerDead(_player)) {
    return;
  }

  processPlayerMovement(_player, _stage);
  processPlayerAttack(_player);
  processPlayerDamage(_player);
}

void drawPlayer(const Player* _player, const Camera* _camera) {
  const V2f32 position = _player->position;
  const V2s32 cameraPosition = getCameraPositionRounded(_camera);
  const u32 offsetX = g_playerSpriteOffset.x + cameraPosition.x;
  const u32 offsetY = g_playerSpriteOffset.y + cameraPosition.y;
  const u16 positionX = fix32ToRoundedInt(position.x) - offsetX;
  const u16 positionY = fix32ToRoundedInt(position.y) - offsetY;
  const s16 bankDirectionRounded = fix16ToRoundedInt(_player->bankDirection);
  const u16 bankDirectionIndex = abs(bankDirectionRounded);
  Sprite* sprite = _player->sprite;
  SpriteVisibility visibility;

  if (_player->damageCooldown > intToFix32(0)) {
    visibility = SPR_isVisible(sprite, FALSE) ? HIDDEN : VISIBLE;
  } else {
    visibility = VISIBLE;
  }

  SPR_setVisibility(sprite, visibility);

  if (visibility != VISIBLE) {
    return;
  }

  SPR_setPosition(sprite, positionX, positionY);

  if (bankDirectionIndex > 0) {
    SPR_setVFlip(sprite, bankDirectionRounded > 0);
    SPR_setAnimAndFrame(sprite, 1, bankDirectionIndex - 1);
  } else {
    SPR_setAnim(sprite, 0);
  }
}

void tearDownPlayer(Player* _player) {
  SPR_releaseSprite(_player->sprite);
}

void doPlayerHit(Player* _player, u8 _damageAmount) {
  f32 damageCooldown = _player->damageCooldown;
  u8 health = _player->health;

  if (health == 0 || damageCooldown > intToFix32(0)) {
    return;
  }

  damageCooldown = PLAYER_INVULNERABILITY_COOLDOWN_DURATION;

  if (health < _damageAmount) {
    health = 0;
  } else {
    health -= _damageAmount;
  }

  _player->health = health;
  _player->damageCooldown = damageCooldown;
}

bool isPlayerDead(Player* _player) {
  return _player->health == 0;
}
