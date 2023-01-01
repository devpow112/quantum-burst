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
#include "camera.h"
#include "stage.h"
#include "utilities.h"

// global properties

static V2f32 g_cameraOffset;

// public functions

void initCamera() {
  g_cameraOffset.x = intToFix32(VDP_getScreenWidth() / 2);
  g_cameraOffset.y = intToFix32(VDP_getScreenHeight() / 2);
}

void setUpCamera(Camera* _camera, CameraPositionCallback _positionCallback,
                 bool _evaluateCallback) {
  V2f32 position;

  if (_positionCallback != NULL && _evaluateCallback) {
    position = _positionCallback();
  } else {
    position.x = 0;
    position.y = position.x;
  }

  _camera->positionCallback = _positionCallback;
  _camera->position = position;
}

void updateCamera(Camera* _camera) {
  CameraPositionCallback positionCallback = _camera->positionCallback;

  if (positionCallback == NULL) {
    return;
  }

  const V2f32 newPosition = positionCallback();
  const V2f32 position = {
    fix32Sub(newPosition.x, g_cameraOffset.x),  // x
    fix32Sub(newPosition.y, g_cameraOffset.y)   // y
  };

  _camera->position = position;
}

void tearDownCamera(Camera* _camera) {
  // nothing so far
}

V2s32 getCameraPositionRounded(const Camera* _camera) {
  const V2s32 positionRounded = {
    fix32ToRoundedInt(_camera->position.x),  // x
    fix32ToRoundedInt(_camera->position.y)   // y
  };

  return positionRounded;
}
