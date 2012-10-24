/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "uv.h"
#include "task.h"

#define NUM_PREPARE_HANDLES 10
static uv_prepare_t prepare_handles[NUM_PREPARE_HANDLES];
static int prepare_cb_called = 0;
static int close_cb_called = 0;


static void close_cb(uv_handle_t* handle) {
  ASSERT(handle != NULL);
  ASSERT(!uv_is_active(handle));

  close_cb_called++;
}

static void prepare_cb(uv_prepare_t *handle, int status) {
  ASSERT(handle);
  ASSERT(status == 0);

  static int called = 0;
  int i, r;
  if (!called) {
    for (i=0; i < NUM_PREPARE_HANDLES; i++) {
        uv_close((uv_handle_t*)&prepare_handles[i], close_cb);
    }
  }
  called = 1;
  prepare_cb_called++;
}

static void prepare_cb2(uv_prepare_t *handle, int status) {
  ASSERT(handle);
  ASSERT(status == 0);

  static int called = 0;
  int i, r;
  if (!called) {
    for (i=NUM_PREPARE_HANDLES-1; i >= 0; i--) {
        uv_close((uv_handle_t*)&prepare_handles[i], close_cb);
    }
  }
  called = 1;
  prepare_cb_called++;
}

TEST_IMPL(prepare_close) {
  int i, r;

  for (i=0; i < NUM_PREPARE_HANDLES; i++) {
    r = uv_prepare_init(uv_default_loop(), &prepare_handles[i]);
    ASSERT(r == 0);
    r = uv_prepare_start(&prepare_handles[i], prepare_cb);
    ASSERT(r == 0);
  }

  r = uv_run(uv_default_loop());
  ASSERT(r == 0);
  printf("Prepare CB called. %d\n", prepare_cb_called);
  ASSERT(prepare_cb_called == 1);
  ASSERT(close_cb_called == NUM_PREPARE_HANDLES);

  MAKE_VALGRIND_HAPPY();
  return 0;
}

TEST_IMPL(prepare_close_reverse) {
  int i, r;

  for (i=0; i < NUM_PREPARE_HANDLES; i++) {
    r = uv_prepare_init(uv_default_loop(), &prepare_handles[i]);
    ASSERT(r == 0);
    r = uv_prepare_start(&prepare_handles[i], prepare_cb2);
    ASSERT(r == 0);
  }

  r = uv_run(uv_default_loop());
  ASSERT(r == 0);
  printf("Prepare CB called. %d\n", prepare_cb_called);
  ASSERT(prepare_cb_called == 1);
  ASSERT(close_cb_called == NUM_PREPARE_HANDLES);

  MAKE_VALGRIND_HAPPY();
  return 0;
}
