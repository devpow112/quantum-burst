// MIT License
//
// Copyright (c) 2023 Devon Powell
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
#include "managed-actor.h"

// entity

struct _ManagedActor;
typedef struct _ManagedActor ManagedActor;

struct _ManagedActor {
  Actor actor;
  ManagedActor* previous;
  ManagedActor* next;
  bool cleanUp;
};

// global properties

static ManagedActor* g_firstManagedActor = NULL;
static ManagedActor* g_lastManagedActor = NULL;

// public functions

void initManagedActors() {
  if (g_firstManagedActor != NULL) {
    destroyManagedActors();
  }

  g_firstManagedActor = NULL;
  g_lastManagedActor = NULL;
}

void createManagedActor(V2f32 _position, void* _data,
                        ActorUpdateCallback _updateCallback,
                        ActorDrawCallback _drawCallback,
                        ActorDestroyCallback _destroyCallback) {
  ManagedActor* managedActor = malloc(sizeof(ManagedActor));

  assert(managedActor != NULL, "Failed to allocate managed actor");

  Actor* actor = &managedActor->actor;

  setUpActor(actor, _position, _data, _updateCallback, _drawCallback,
             _destroyCallback);

  managedActor->next = NULL;
  managedActor->cleanUp = FALSE;

  if (g_firstManagedActor == NULL) {
    managedActor->previous = NULL;
    g_firstManagedActor = managedActor;
    g_lastManagedActor = managedActor;
  } else if (g_lastManagedActor != NULL) {
    managedActor->previous = g_lastManagedActor;
    g_lastManagedActor->next = managedActor;
    g_lastManagedActor = managedActor;
  }
}

void updateManagedActors(const Stage* _stage) {
  if (_stage == NULL) {
    return;
  }

  ManagedActor* managedActor = g_firstManagedActor;

  while (managedActor != NULL) {
    if (managedActor->cleanUp) {
      Actor* actor = &managedActor->actor;

      if (actor->destroyCallback != NULL) {
        actor->destroyCallback(actor);
      }

      ManagedActor* managedActorNext = managedActor->next;

      managedActor->previous->next = managedActorNext;
      managedActor->next->previous = managedActor->previous;

      free(managedActor);

      managedActor = managedActorNext;

      continue;
    }

    updateActor(&managedActor->actor, _stage);

    managedActor = managedActor->next;
  }
}

void drawManagedActors(const Camera* _camera) {
  if (_camera == NULL) {
    return;
  }

  ManagedActor* managedActor = g_firstManagedActor;

  while (managedActor != NULL) {
    drawActor(&managedActor->actor, _camera);

    managedActor = managedActor->next;
  }
}

void destroyManagedActors() {
  ManagedActor* managedActor = g_firstManagedActor;

  while (managedActor != NULL) {
    Actor* actor = &managedActor->actor;

    if (actor->destroyCallback != NULL) {
      actor->destroyCallback(actor);
    }

    ManagedActor* managedActorNext = managedActor->next;

    free(managedActor);

    managedActor = managedActorNext;
  }

  g_firstManagedActor = NULL;
  g_lastManagedActor = NULL;
}

void setManagedActorCleanUp(Actor* _actor) {
  ManagedActor* managedActor = (ManagedActor*)_actor;

  managedActor->cleanUp = TRUE;
}
