// MIT License
//
// Copyright (c) 2026 Devon Powell
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
#include "assert.h"

// public functions

Actor* createActor(V2f32 _position, void* _data,
                   ActorUpdateCallback _updateCallback,
                   ActorDrawCallback _drawCallback,
                   ActorDestroyCallback _destroyCallback) {
  Actor* actor = malloc(sizeof(Actor));

  assert(actor != NULL, "Failed to allocate actor");
  setUpActor(actor, _position, _data, _updateCallback, _drawCallback,
             _destroyCallback);

  return actor;
}

void setUpActor(Actor* _actor, V2f32 _position, void* _data,
                ActorUpdateCallback _updateCallback,
                ActorDrawCallback _drawCallback,
                ActorDestroyCallback _destroyCallback) {
  _actor->position = _position;
  _actor->data = _data;
  _actor->updateCallback = _updateCallback;
  _actor->drawCallback = _drawCallback;
  _actor->destroyCallback = _destroyCallback;
}

void updateActor(Actor* _actor, const Stage* _stage) {
  if (_actor == NULL || _actor->updateCallback == NULL || _stage == NULL) {
    return;
  }

  _actor->updateCallback(_actor, _stage);
}

void drawActor(const Actor* _actor, const Camera* _camera) {
  if (_actor == NULL || _actor->drawCallback == NULL || _camera == NULL) {
    return;
  }

  _actor->drawCallback(_actor, _camera);
}

void tearDownActor(Actor* _actor) {
  if (_actor->destroyCallback != NULL) {
    _actor->destroyCallback(_actor);
  }

  _actor->data = NULL;
}

void destroyActor(Actor* _actor) {
  tearDownActor(_actor);
  free(_actor);
}

V2f32 getActorPosition(const Actor* _actor) {
  return _actor->position;
}

f32 getActorPositionX(const Actor* _actor) {
  const V2f32 position = getActorPosition(_actor);

  return position.x;
}

f32 getActorPositionY(const Actor* _actor) {
  const V2f32 position = getActorPosition(_actor);

  return position.y;
}

void setActorPosition(Actor* _actor, V2f32 _position) {
  _actor->position = _position;
}

void* getActorData(const Actor* _actor) {
  return _actor->data;
}

V2f32 getDirectionTowardsActor(const Actor* _actor, const Actor* _target) {
  const V2f32 position1 = getActorPosition(_actor);
  const V2f32 position2 = getActorPosition(_target);
  const s32 deltaX = (s32)(position1.x - position2.x);
  const s32 deltaY = (s32)(position1.y - position2.y);
  const f32 magnitude = (f32)getApproximatedDistance(deltaX, deltaY);
  const V2f32 direction = {
    F32_div((f32)deltaX, magnitude),  // x
    F32_div((f32)deltaY, magnitude)   // y
  };

  return direction;
}

f32 getDistanceBetweenActors(const Actor* _actor1, const Actor* _actor2) {
  const V2f32 position1 = getActorPosition(_actor1);
  const V2f32 position2 = getActorPosition(_actor2);
  const s32 deltaX = (s32)(position1.x - position2.x);
  const s32 deltaY = (s32)(position1.y - position2.y);

  return (f32)getApproximatedDistance(deltaX, deltaY);
}
