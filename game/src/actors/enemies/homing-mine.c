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
#include "actors/enemies/homing-mine.h"
#include "actors/player.h"
#include "assert.h"
#include "managed-actor.h"
#include "sprites.h"
#include "utilities.h"

// constants

#define HOMING_MINE_SPEED FIX32(1.5)
#define HOMING_MINE_SPRITE_FLAGS                         \
  (SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | \
   SPR_FLAG_AUTO_SPRITE_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD)

// global properties

static V2s16 g_homingMineSpriteOffset;  // pixels
static u8 g_homingMineExplosionRadius;  // pixels
static u8 g_homingMineHomingRadius;     // pixels
static f32 g_homingMineSpeed;           // pixels/frame

typedef struct {
  Sprite* sprite;
  Actor* player;
  bool exploded;
} HomingMineData;

// private functions

static void update(Actor* _actor, const Stage* _stage) {
  HomingMineData* data = (HomingMineData*)getActorData(_actor);
  bool exploded = data->exploded;

  if (exploded) {
    return;
  }

  Actor* player = data->player;
  V2f32 position = getActorPosition(_actor);
  const V2f32 playerPosition = getActorPosition(player);
  const f32 deltaX = fix32Sub(position.x, playerPosition.x);
  const f32 deltaY = fix32Sub(position.y, playerPosition.y);
  const u8 radius = getPlayerRadius(player);
  const f32 homingRadius = intToFix32(g_homingMineHomingRadius + radius);
  const f32 explodeRadius = intToFix32(g_homingMineExplosionRadius + radius);
  const f32 magnitude = (f32)getApproximatedDistance((s32)deltaX, (s32)deltaY);

  if (magnitude <= explodeRadius) {
    exploded = TRUE;

    setManagedActorCleanUp(_actor);
    doPlayerHit(player);
  } else if (magnitude <= homingRadius) {
    const f32 speedX = fix32Mul(fix32Div(deltaX, magnitude), g_homingMineSpeed);
    const f32 speedY = fix32Mul(fix32Div(deltaY, magnitude), g_homingMineSpeed);

    position.x = fix32Sub(position.x, speedX);
    position.y = fix32Sub(position.y, speedY);

    setActorPosition(_actor, position);
  }

  data->exploded = exploded;
}

static void draw(const Actor* _actor, const Camera* _camera) {
  const HomingMineData* data = (const HomingMineData*)getActorData(_actor);
  const bool exploded = data->exploded;
  Sprite* sprite = data->sprite;

  if (exploded) {
    SPR_setVisibility(sprite, HIDDEN);

    return;
  }

  const V2f32 position = getActorPosition(_actor);
  const V2s32 cameraPosition = getCameraPositionRounded(_camera);
  const u32 offsetX = g_homingMineSpriteOffset.x + cameraPosition.x;
  const u32 offsetY = g_homingMineSpriteOffset.y + cameraPosition.y;
  const u16 positionX = fix32ToRoundedInt(position.x) - offsetX;
  const u16 positionY = fix32ToRoundedInt(position.y) - offsetY;

  SPR_setPosition(sprite, positionX, positionY);
}

static void destroy(Actor* _actor) {
  HomingMineData* data = (HomingMineData*)getActorData(_actor);

  SPR_releaseSprite(data->sprite);
  free(data);
}

// public functions

void initHomingMine() {
  const u8 spriteHalfWidth = k_mineSprite.w / 2;

  g_homingMineSpriteOffset.x = spriteHalfWidth;
  g_homingMineSpriteOffset.x = k_mineSprite.h / 2;
  g_homingMineExplosionRadius = spriteHalfWidth;
  g_homingMineHomingRadius = spriteHalfWidth * 10;
  g_homingMineSpeed = fix32Div(intToFix32(75), intToFix32(getFrameRate()));
}

void createHomingMine(u16 _palette, V2f32 _position, Actor* _player) {
  HomingMineData* data = malloc(sizeof(HomingMineData));

  assert(data != NULL, "Failed to allocate mine homing data");

  data->player = _player;
  data->exploded = FALSE;

  const f32 x = fix32ToRoundedInt(_position.x) + g_homingMineSpriteOffset.x;
  const f32 y = fix32ToRoundedInt(_position.y) + g_homingMineSpriteOffset.y;
  const u16 attributes = TILE_ATTR(_palette, FALSE, FALSE, FALSE);

  data->sprite = SPR_addSpriteExSafe(&k_mineSprite, x, y, attributes, 0,
                                     HOMING_MINE_SPRITE_FLAGS);

  createManagedActor(_position, data, &update, &draw, &destroy);
}
