// running 2000000000 context switch

#include <stdio.h>
#include <stdlib.h>
#include "libctx.h"

#define STACK_SIZE 4096

void func_sum(ctx_t ctx, void *data) {
  for (;;) {
    ctx = ctx_jump(ctx, NULL).ctx;
  }
}

int main() {
  uint8_t *stack = malloc(STACK_SIZE);
  ctx_t ctx = ctx_make(stack + STACK_SIZE, &func_sum);

  for (int i = 0; i < 1000000000; i++) {
    ctx = ctx_jump(ctx, NULL).ctx;
  }

  free(stack);
  return 0;
}
