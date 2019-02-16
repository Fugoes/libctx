// running 2000000000 context switch

#include <stdio.h>
#include <stdlib.h>
#include "libctx.h"

#define STACK_SIZE 4096

void func(ctx_t ctx, void *data) {
  ctx_rval_t rval = {ctx, NULL};
  for (;;) {
    rval = ctx_jump(rval.ctx, NULL);
  }
}

int main() {
  uint8_t *stack = malloc(STACK_SIZE);
  ctx_rval_t rval = {ctx_make(stack + STACK_SIZE, &func), NULL};

  for (int i = 0; i < 1000000000; i++) {
    rval = ctx_jump(rval.ctx, NULL);
  }

  free(stack);

  return 0;
}
