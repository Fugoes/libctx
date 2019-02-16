#include <stdio.h>
#include <stdlib.h>
#include "libctx.h"

#define STACK_SIZE 4096

void func_sum(ctx_t ctx, void *data) {
  uint64_t sum = 0;
  for (;;) {
    sum += *((uint64_t *) data);
    ctx_rval_t rval = ctx_jump(ctx, &sum);
    ctx = rval.ctx;
    data = rval.data;
  }
}

void func_prod(ctx_t ctx, void *data) {
  uint64_t prod = 1;
  for (;;) {
    prod *= *((uint64_t *) data);
    ctx_rval_t rval = ctx_jump(ctx, &prod);
    ctx = rval.ctx;
    data = rval.data;
  }
}

int main() {
  uint8_t *stack_sum = malloc(STACK_SIZE);
  uint8_t *stack_prod = malloc(STACK_SIZE);
  ctx_t ctx_sum = ctx_make(stack_sum + STACK_SIZE, &func_sum);
  ctx_t ctx_prod = ctx_make(stack_prod + STACK_SIZE, &func_prod);
  ctx_rval_t rval;
  for (uint64_t i = 1; i <= 10; i++) {
    printf("i    = %lu\n", i);
    rval = ctx_jump(ctx_sum, &i);
    ctx_sum = rval.ctx;
    printf("sum  = %lu\n", *((uint64_t *) rval.data));
    rval = ctx_jump(ctx_prod, &i);
    ctx_prod = rval.ctx;
    printf("prod = %lu\n", *((uint64_t *) rval.data));
  }
  return 0;
}
