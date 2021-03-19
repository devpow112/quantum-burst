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

#include "actor.h"
#include "actors/player.h"
#include "assert.h"
#include "camera.h"
#include "sprites.h"
#include "stage.h"
#include "utilities.h"

// constants

#define PLAYER_SCREEN_BUFFER 2                               // pixels
#define PLAYER_ATTACK_COOLDOWN_DURATION (FIX16(0.05))        // seconds
#define PLAYER_INVULNERABILITY_COOLDOWN_DURATION (FIX16(1))  // seconds
#define PLAYER_BANKING_DIRECTION_DEFAULT (FIX16(0))
#define PLAYER_BANKING_DIRECTION_MAX_RIGHT (FIX16(2))
#define PLAYER_BANKING_DIRECTION_MAX_LEFT (-PLAYER_BANKING_DIRECTION_MAX_RIGHT)
#define PLAYER_HEALTH_DEFAULT 100
#define PLAYER_SPRITE_FLAGS                                \
  (SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_SPRITE_ALLOC | \
   SPR_FLAG_AUTO_TILE_UPLOAD)

// global properties

static V2f32 g_playerBuffer;        // pixels
static V2s16 g_playerSpriteOffset;  // pixels
static f32 g_playerVelocity;        // pixels/second
static f16 g_playerBankingRate;     // fps

typedef struct {
  Sprite* sprite;
  f16 bankDirection;
  f16 attackCooldown;
  f16 damageCooldown;
  u8 health;
} PlayerData;

// private functions

static void processMovement(Actor* _actor, PlayerData* _data,
                            const Stage* _stage) {
  V2f32 position = getActorPosition(_actor);
  const f32 previousPositionY = position.y;
  const u16 inputState = JOY_readJoypad(JOY_1);
  f16 bankDirection = _data->bankDirection;

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

  _data->bankDirection = bankDirection;

  setActorPosition(_actor, position);
}

static void processAttack(PlayerData* _data) {
  const u16 inputState = JOY_readJoypad(JOY_1);
  f16 attackCooldown = _data->attackCooldown;

  if (attackCooldown > intToFix16(0)) {
    const f16 deltaTime = fix32ToFix16(getFrameDeltaTime());

    attackCooldown = fix16Sub(attackCooldown, deltaTime);
  } else if ((inputState & BUTTON_A)) {
    attackCooldown = PLAYER_ATTACK_COOLDOWN_DURATION;
  }

  _data->attackCooldown = attackCooldown;
}

static void processDamage(PlayerData* _data) {
  f16 damageCooldown = _data->damageCooldown;

  if (damageCooldown > intToFix16(0)) {
    const f16 deltaTime = fix32ToFix16(getFrameDeltaTime());

    damageCooldown = fix16Sub(damageCooldown, deltaTime);
  }

  _data->damageCooldown = damageCooldown;
}

static void update(Actor* _actor, const Stage* _stage) {
  PlayerData* data = (PlayerData*)getActorData(_actor);

  if (isPlayerDead(_actor)) {
    return;
  }

  processMovement(_actor, data, _stage);
  processAttack(data);
  processDamage(data);
}

static void draw(const Actor* _actor, const Camera* _camera) {
  const V2f32 position = getActorPosition(_actor);
  const V2s32 cameraPosition = getCameraPositionRounded(_camera);
  const u32 offsetX = g_playerSpriteOffset.x + cameraPosition.x;
  const u32 offsetY = g_playerSpriteOffset.y + cameraPosition.y;
  const u16 positionX = fix32ToRoundedInt(position.x) - offsetX;
  const u16 positionY = fix32ToRoundedInt(position.y) - offsetY;
  const PlayerData* data = (const PlayerData*)getActorData(_actor);
  const s8 bankDirectionRounded = fix16ToRoundedInt(data->bankDirection);
  const u8 bankDirectionIndex = abs(bankDirectionRounded);
  Sprite* sprite = data->sprite;
  SpriteVisibility visibility;

  if (data->damageCooldown > intToFix16(0)) {
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

static void destroy(Actor* _actor) {
  PlayerData* data = (PlayerData*)getActorData(_actor);

  SPR_releaseSprite(data->sprite);
  free(data);
}

// public functions

void initPlayer() {
  const V2s16 spriteOffset = {
    k_shipSprite.w / 2,  // x
    k_shipSprite.h / 2   // y
  };
  const V2f32 buffer = {
    intToFix32(PLAYER_SCREEN_BUFFER + spriteOffset.x),  // x
    intToFix32(PLAYER_SCREEN_BUFFER + spriteOffset.y),  // y
  };

  g_playerSpriteOffset = spriteOffset;
  g_playerBuffer = buffer;
  g_playerVelocity = fix32Div(intToFix32(120), intToFix32(getFrameRate()));
  g_playerBankingRate = fix16Div(intToFix16(20), intToFix16(getFrameRate()));
}

Actor* createPlayer(u16 _palette, const V2f32 _position) {
  const V2u16 spritePosition = {
    fix32ToRoundedInt(_position.x) + g_playerSpriteOffset.x,  // x
    fix32ToRoundedInt(_position.y) + g_playerSpriteOffset.y   // y
  };
  const u16 spriteAttributes = TILE_ATTR(_palette, TRUE, FALSE, FALSE);
  PlayerData* data = malloc(sizeof(PlayerData));

  ASSERT(data != NULL, "Failed to allocate player data");

  data->sprite =
    SPR_addSpriteExSafe(&k_shipSprite, spritePosition.x, spritePosition.y,
                        spriteAttributes, 0, PLAYER_SPRITE_FLAGS);
  data->bankDirection = PLAYER_BANKING_DIRECTION_DEFAULT;
  data->attackCooldown = PLAYER_ATTACK_COOLDOWN_DURATION;
  data->damageCooldown = PLAYER_INVULNERABILITY_COOLDOWN_DURATION;
  data->health = PLAYER_HEALTH_DEFAULT;

  return createActor(_position, data, &update, &draw, &destroy);
}

void doPlayerHit(Actor* _actor, u8 _damageAmount) {
  PlayerData* data = (PlayerData*)getActorData(_actor);
  f16 damageCooldown = data->damageCooldown;
  u8 health = data->health;

  if (health == 0 || damageCooldown > intToFix16(0)) {
    return;
  }

  damageCooldown = PLAYER_INVULNERABILITY_COOLDOWN_DURATION;

  if (health < _damageAmount) {
    health = 0;
  } else {
    health -= _damageAmount;
  }

  data->health = health;
  data->damageCooldown = damageCooldown;
}

bool isPlayerDead(const Actor* _actor) {
  const PlayerData* data = (const PlayerData*)getActorData(_actor);

  return data->health == 0;
}
