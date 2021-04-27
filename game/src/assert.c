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

#include "assert.h"

void _assert(bool _exp, const char* _file, u16 _line, const char* _msg) {
  if (!_exp) {
    SYS_setInterruptMaskLevel(7);

    if (SPR_isInitialized()) {
      SPR_end();
    }

    VDP_init();

    char messageFull[720] = {0};

    sprintf(messageFull, "[%s:%d] %s", _file, _line, _msg);

    char* messageFullCurrent = messageFull;
    u8 column = 0;

    VDP_drawText("Assert!", 0, column++);

    do {
      VDP_drawText(messageFullCurrent, 0, column++);

      const u16 length = strlen(messageFullCurrent);

      messageFullCurrent += length < 40 ? length : 40;
    } while (*messageFullCurrent);

    VDP_drawText("Cannot continue...", 0, column++);

    while (1) {
      // loop forever
    }
  }
}
