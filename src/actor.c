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

static Actor* g_firstActor = NULL;
static Actor* g_lastActor = NULL;

Actor* createActor(V2f32 _position, void* _data,
                   ActorUpdateCallback _updateCallback,
                   ActorDrawCallback _drawCallback,
                   ActorDestroyCallback _destroyCallback) {
  Actor* actor = malloc(sizeof(Actor));

  actor->position = _position;
  actor->data = _data;
  actor->updateCallback = _updateCallback;
  actor->drawCallback = _drawCallback;
  actor->destroyCallback = _destroyCallback;
  actor->next = NULL;

  if (g_firstActor == NULL) {
    actor->previous = NULL;

    g_firstActor = actor;
    g_lastActor = actor;
  } else if (g_lastActor != NULL) {
    actor->previous = g_lastActor;
    g_lastActor->next = actor;
    g_lastActor = actor;
  }

  return actor;
}

void updateActor(Actor* _actor, const Stage* _stage) {
  _actor->updateCallback(_actor, _stage);
}

void updateActors(const Stage* _stage) {
  Actor* actor = g_firstActor;

  while (actor != NULL) {
    updateActor(actor, _stage);

    actor = actor->next;
  }
}

void drawActor(const Actor* _actor, const Camera* _camera) {
  _actor->drawCallback(_actor, _camera);
}

void drawActors(const Camera* _camera) {
  Actor* actor = g_firstActor;

  while (actor != NULL) {
    drawActor(actor, _camera);

    actor = actor->next;
  }
}

void destroyActor(Actor* _actor) {
  if (_actor->destroyCallback != NULL) {
    _actor->destroyCallback(_actor);
  }

  if (_actor->previous != NULL) {
    _actor->previous->next = _actor->next;
  }

  if (_actor->next != NULL) {
    _actor->next->previous = _actor->previous;
  }

  free(_actor);
}

void destroyActors() {
  Actor* actor = g_firstActor;

  while (actor != NULL) {
    if (actor->destroyCallback != NULL) {
      actor->destroyCallback(actor);
    }

    Actor* next = actor->next;

    free(actor);

    actor = next;
  }

  g_firstActor = NULL;
  g_lastActor = NULL;
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
