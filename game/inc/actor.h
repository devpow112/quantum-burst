// MIT License
//
// Copyright (c) 2024 Devon Powell
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

#ifndef __QUANTUM_BURST_ACTOR_H__
#define __QUANTUM_BURST_ACTOR_H__

#include <genesis.h>

#include "camera.h"
#include "stage.h"

// entity

struct _Actor;
typedef struct _Actor Actor;

typedef void (*ActorUpdateCallback)(Actor* _actor, const Stage* _stage);
typedef void (*ActorDrawCallback)(const Actor* _actor, const Camera* _camera);
typedef void (*ActorDestroyCallback)(Actor* _actor);

struct _Actor {
  V2f32 position;
  void* data;
  ActorUpdateCallback updateCallback;
  ActorDrawCallback drawCallback;
  ActorDestroyCallback destroyCallback;
};

// life-cycle

Actor* createActor(V2f32 _position, void* _data,
                   ActorUpdateCallback _updateCallback,
                   ActorDrawCallback _drawCallback,
                   ActorDestroyCallback _destroyCallback);

void setUpActor(Actor* _actor, V2f32 _position, void* _data,
                ActorUpdateCallback _updateCallback,
                ActorDrawCallback _drawCallback,
                ActorDestroyCallback _destroyCallback);

void updateActor(Actor* _actor, const Stage* _stage);

void drawActor(const Actor* _actor, const Camera* _camera);

void tearDownActor(Actor* _actor);

void destroyActor(Actor* _actor);

// properties

V2f32 getActorPosition(const Actor* _actor);

f32 getActorPositionX(const Actor* _actor);

f32 getActorPositionY(const Actor* _actor);

void setActorPosition(Actor* _actor, V2f32 _position);

void* getActorData(const Actor* _actor);

// utilities

V2f32 getDirectionTowardsActor(const Actor* _actor, const Actor* _target);

f32 getDistanceBetweenActors(const Actor* _actor1, const Actor* _actor2);

#endif  // __QUANTUM_BURST_ACTOR_H__
