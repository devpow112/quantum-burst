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
#define PLAYER_SCREEN_BUFFER 2                               // pixels
#define PLAYER_ATTACK_COOLDOWN_DURATION (FIX16(0.05))        // seconds
#define PLAYER_INVULNERABILITY_COOLDOWN_DURATION (FIX16(1))  // seconds
#define PLAYER_BANKING_DIRECTION_DEFAULT (FIX16(0))
#define PLAYER_BANKING_DIRECTION_MAX_RIGHT (FIX16(2))
#define PLAYER_BANKING_DIRECTION_MAX_LEFT (-PLAYER_BANKING_DIRECTION_MAX_RIGHT)
#define PLAYER_HEALTH_DEFAULT 100

// player global properties
static V2f16 g_playerMinPosition;
static V2f16 g_playerMaxPosition;
static V2u16 g_playerStartPosition;
static V2u16 g_playerSpriteOffset;
static f16 g_playerVelocity;

// player animation constants
static f16 g_playerBankingRate;

static void processPlayerMovement(Player* _player, u16 _inputState) {
  V2f16 position = _player->position;
  f16 bankDirection = _player->bankDirection;
  const f16 previousPositionY = position.y;

  if (_inputState & BUTTON_LEFT) {
    position.x = fix16Sub(position.x, g_playerVelocity);
  }

  if (_inputState & BUTTON_RIGHT) {
    position.x = fix16Add(position.x, g_playerVelocity);
  }

  if (_inputState & BUTTON_UP) {
    position.y = fix16Sub(position.y, g_playerVelocity);
  }

  if (_inputState & BUTTON_DOWN) {
    position.y = fix16Add(position.y, g_playerVelocity);
  }

  position.x = clamp(position.x, g_playerMinPosition.x, g_playerMaxPosition.x);
  position.y = clamp(position.y, g_playerMinPosition.y, g_playerMaxPosition.y);

  const s8 deltaY = fix16ToRoundedInt(fix16Sub(previousPositionY, position.y));

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

  _player->bankDirection = bankDirection;
  _player->position = position;
}

static void processPlayerAttack(Player* _player, u16 _inputState) {
  f16 attackCooldown = _player->attackCooldown;

  if (attackCooldown > FIX16(0)) {
    clearText(0);

    attackCooldown = fix16Sub(attackCooldown, getFrameDeltaTime());
  } else if ((_inputState & BUTTON_A)) {
    showText("FIRE", 0);

    attackCooldown = PLAYER_ATTACK_COOLDOWN_DURATION;
  }

  _player->attackCooldown = attackCooldown;
}

static void processPlayerDamage(Player* _player) {
  f16 damageCooldown = _player->damageCooldown;

  if (damageCooldown > FIX16(0)) {
    damageCooldown = fix16Sub(damageCooldown, getFrameDeltaTime());
  }

  _player->damageCooldown = damageCooldown;
}

static void updatePlayerSprite(const Player* _player) {
  const V2f16 position = _player->position;
  const u16 positionX = fix16ToRoundedInt(position.x) - g_playerSpriteOffset.x;
  const u16 positionY = fix16ToRoundedInt(position.y) - g_playerSpriteOffset.y;
  const s8 bankDirectionRounded = fix16ToRoundedInt(_player->bankDirection);
  const u8 bankDirectionIndex = abs(bankDirectionRounded);
  Sprite* sprite = _player->sprite;
  SpriteVisibility visibility;

  if (_player->damageCooldown > FIX16(0)) {
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

void initPlayer() {
  const u16 width = VDP_getScreenWidth();
  const u16 height = VDP_getScreenHeight();
  const f16 fps = intToFix16(getFrameRate());

  g_playerSpriteOffset.x = k_shipSprite.w / 2;
  g_playerSpriteOffset.y = k_shipSprite.h / 2;

  const u16 bufferXInt = PLAYER_SCREEN_BUFFER + g_playerSpriteOffset.x;
  const f16 bufferX = intToFix16(bufferXInt);
  const f16 bufferY = intToFix16(PLAYER_SCREEN_BUFFER + g_playerSpriteOffset.y);

  g_playerMinPosition.x = bufferX;
  g_playerMinPosition.y = bufferY;
  g_playerMaxPosition.x = fix16Sub(intToFix16(width), bufferX);
  g_playerMaxPosition.y = fix16Sub(intToFix16(height), bufferY);
  g_playerStartPosition.x = bufferXInt;
  g_playerStartPosition.y = height / 2;
  g_playerVelocity = fix16Div(FIX16(120), fps);
  g_playerBankingRate = fix16Div(FIX16(20), fps);
}

Player* createPlayer(u16 _palette) {
  Player* player = malloc(sizeof(Player));

  setUpPlayer(player, _palette);

  return player;
}

void setUpPlayer(Player* _player, u16 _palette) {
  _player->position.x = intToFix16(g_playerStartPosition.x);
  _player->position.y = intToFix16(g_playerStartPosition.y);
  _player->attackCooldown = PLAYER_ATTACK_COOLDOWN_DURATION;
  _player->bankDirection = PLAYER_BANKING_DIRECTION_DEFAULT;
  _player->damageCooldown = FIX16(0);
  _player->health = PLAYER_HEALTH_DEFAULT;
  _player->sprite = SPR_addSpriteSafe(&k_shipSprite, g_playerStartPosition.x,
                                      g_playerStartPosition.y,
                                      TILE_ATTR(_palette, 0, FALSE, FALSE));
}

void updatePlayer(Player* _player) {
  if (!isPlayerDead(_player)) {
    const u16 inputState = JOY_readJoypad(JOY_1);

    processPlayerMovement(_player, inputState);
    processPlayerAttack(_player, inputState);
    processPlayerDamage(_player);
  }

  updatePlayerSprite(_player);
}

void tearDownPlayer(Player* _player) {
  SPR_releaseSprite(_player->sprite);
}

void destroyPlayer(Player* _player) {
  tearDownPlayer(_player);
  free(_player);
}

void doPlayerHit(Player* _player, u8 _damageAmount) {
  f16 damageCooldown = _player->damageCooldown;
  u8 health = _player->health;

  if (health == 0 || damageCooldown > 0) {
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
