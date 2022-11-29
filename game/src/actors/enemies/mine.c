// MIT License
//
// Copyright (c) 2022 Devon Powell
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
#include "actors/enemies/mine.h"
#include "actors/player.h"
#include "assert.h"
#include "managed-actor.h"
#include "sprites.h"

// global properties

static V2s16 g_mineSpriteOffset;  // pixels
static u8 g_mineExplosionRadius;  // pixels

typedef struct {
  Sprite* sprite;
  Actor* player;
  bool exploded;
} MineData;

// private functions

static void update(Actor* _actor, const Stage* _stage) {
  MineData* data = (MineData*)getActorData(_actor);
  bool exploded = data->exploded;

  if (exploded) {
    return;
  }

  Actor* player = data->player;
  const u8 radius = getPlayerRadius(player);
  const f32 explodeRadius = intToFix32(g_mineExplosionRadius + radius);
  const f32 magnitude = getDistanceBetweenActors(_actor, player);

  if (magnitude <= explodeRadius) {
    exploded = TRUE;

    setManagedActorCleanUp(_actor);
    doPlayerHit(player);
  }

  data->exploded = exploded;
}

static void draw(const Actor* _actor, const Camera* _camera) {
  const MineData* data = (const MineData*)getActorData(_actor);
  const bool exploded = data->exploded;
  Sprite* sprite = data->sprite;

  if (exploded) {
    SPR_setVisibility(sprite, HIDDEN);

    return;
  }

  const V2f32 position = getActorPosition(_actor);
  const V2s32 cameraPosition = getCameraPositionRounded(_camera);
  const u32 offsetX = g_mineSpriteOffset.x + cameraPosition.x;
  const u32 offsetY = g_mineSpriteOffset.y + cameraPosition.y;
  const u16 positionX = fix32ToRoundedInt(position.x) - offsetX;
  const u16 positionY = fix32ToRoundedInt(position.y) - offsetY;

  SPR_setPosition(sprite, positionX, positionY);
}

static void destroy(Actor* _actor) {
  MineData* data = (MineData*)getActorData(_actor);

  SPR_releaseSprite(data->sprite);
  free(data);
}

// public functions

void initMine() {
  const u8 spriteHalfWidth = k_mineSprite.w / 2;

  g_mineSpriteOffset.x = spriteHalfWidth;
  g_mineSpriteOffset.y = k_mineSprite.h / 2;
  g_mineExplosionRadius = spriteHalfWidth;
}

void createMine(u16 _palette, V2f32 _position, Actor* _player) {
  MineData* data = malloc(sizeof(MineData));

  assert(data != NULL, "Failed to allocate mine data");

  data->player = _player;
  data->exploded = FALSE;

  const f32 x = fix32ToRoundedInt(_position.x) + g_mineSpriteOffset.x;
  const f32 y = fix32ToRoundedInt(_position.y) + g_mineSpriteOffset.y;
  const u16 attributes = TILE_ATTR(_palette, FALSE, FALSE, FALSE);

  data->sprite = SPR_addSpriteSafe(&k_mineSprite, x, y, attributes);

  createManagedActor(_position, data, &update, &draw, &destroy);
}
