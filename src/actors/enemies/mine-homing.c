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
#include "actors/enemies/mine-homing.h"
#include "actors/player.h"
#include "assert.h"
#include "managed-actor.h"
#include "sprites.h"
#include "utilities.h"

// constants

#define MINE_HOMING_SPEED FIX32(1.5)
#define MINE_HOMING_SPRITE_FLAGS                         \
  (SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | \
   SPR_FLAG_AUTO_SPRITE_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD)

// global properties

static V2s16 g_mineHomingSpriteOffset;  // pixels
static f32 g_mineHomingSpeed;           // pixels/frame

typedef struct {
  Sprite* sprite;
  Actor* player;
  u8 homingRadius;
  u8 explosionRadius;
  bool exploded;
} MineHomingData;

// private functions

static void update(Actor* _actor, const Stage* _stage) {
  MineHomingData* data = (MineHomingData*)getActorData(_actor);
  bool exploded = data->exploded;

  if (exploded) {
    return;
  }

  Actor* player = data->player;
  const u8 mineHomingRadius = data->homingRadius;
  const u8 mineExplosionRadius = data->explosionRadius;
  const u8 playerRadius = getPlayerRadius(player);
  const f32 homingRadius = intToFix32(mineHomingRadius + playerRadius);
  const f32 explodeRadius = intToFix32(mineExplosionRadius + playerRadius);
  const f32 distanceToPlayer = getDistanceBetweenActors(_actor, player);

  if (distanceToPlayer <= explodeRadius) {
    exploded = TRUE;

    doPlayerHit(player);
  } else if (distanceToPlayer <= homingRadius) {
    V2f32 position = getActorPosition(_actor);
    const V2f32 directionToPlayer = getDirectionTowardsActor(_actor, player);
    const f32 speedX = fix32Mul(directionToPlayer.x, g_mineHomingSpeed);
    const f32 speedY = fix32Mul(directionToPlayer.y, g_mineHomingSpeed);

    position.x = fix32Sub(position.x, speedX);
    position.y = fix32Sub(position.y, speedY);

    setActorPosition(_actor, position);
  }

  data->exploded = exploded;
}

static void draw(const Actor* _actor, const Camera* _camera) {
  const MineHomingData* data = (const MineHomingData*)getActorData(_actor);
  const bool exploded = data->exploded;
  Sprite* sprite = data->sprite;

  if (exploded) {
    SPR_setVisibility(sprite, HIDDEN);
  }

  const V2f32 position = getActorPosition(_actor);
  const V2s32 cameraPosition = getCameraPositionRounded(_camera);
  const u32 offsetX = g_mineHomingSpriteOffset.x + cameraPosition.x;
  const u32 offsetY = g_mineHomingSpriteOffset.y + cameraPosition.y;
  const u16 positionX = fix32ToRoundedInt(position.x) - offsetX;
  const u16 positionY = fix32ToRoundedInt(position.y) - offsetY;

  SPR_setPosition(sprite, positionX, positionY);
}

static void destroy(Actor* _actor) {
  MineHomingData* data = (MineHomingData*)getActorData(_actor);

  SPR_releaseSprite(data->sprite);
  free(data);
}

// public functions

void initMineHoming() {
  const V2s16 spriteOffset = {
    k_mineSprite.w / 2,  // x
    k_mineSprite.h / 2   // y
  };
  const u8 fps = getFrameRate();

  g_mineHomingSpriteOffset = spriteOffset;
  g_mineHomingSpeed = fix32Div(intToFix32(75), intToFix32(fps));
}

void createMineHoming(u16 _palette, V2f32 _position, Actor* _player) {
  const V2u16 spritePosition = {
    fix32ToRoundedInt(_position.x) + g_mineHomingSpriteOffset.x,  // x
    fix32ToRoundedInt(_position.y) + g_mineHomingSpriteOffset.y   // y
  };
  const u16 spriteAttributes = TILE_ATTR(_palette, FALSE, FALSE, FALSE);
  MineHomingData* data = malloc(sizeof(MineHomingData));

  assert(data != NULL, "Failed to allocate mine homing data");

  data->sprite =
    SPR_addSpriteExSafe(&k_mineSprite, spritePosition.x, spritePosition.y,
                        spriteAttributes, 0, MINE_HOMING_SPRITE_FLAGS);
  data->player = _player;
  data->exploded = FALSE;

  const u8 explosionRadius = k_mineSprite.w / 2;

  data->explosionRadius = explosionRadius;
  data->homingRadius = explosionRadius * 10;

  createManagedActor(_position, data, &update, &draw, &destroy);
}
