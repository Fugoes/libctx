#include <stdio.h>
#include <stdlib.h>
#include "libctx.h"

#define STACK_SIZE 4096

void func_sum(ctx_t ctx, void *data) {
  uint32_t sum = 0;
  ctx_rval_t rval = {ctx, data};
  for (;;) {
    sum += *((uint32_t *) rval.data);
    rval = ctx_jump(rval.ctx, &sum);
  }
}

void func_prod(ctx_t ctx, void *data) {
  uint32_t prod = 1;
  ctx_rval_t rval = {ctx, data};
  for (;;) {
    prod *= *((uint32_t *) rval.data);
    rval = ctx_jump(rval.ctx, &prod);
  }
}

int main() {
  uint8_t *stack_sum = malloc(STACK_SIZE);
  uint8_t *stack_prod = malloc(STACK_SIZE);
  ctx_rval_t rval_sum = {ctx_make(stack_sum + STACK_SIZE, &func_sum), NULL};
  ctx_rval_t rval_prod = {ctx_make(stack_prod + STACK_SIZE, &func_prod), NULL};
  for (uint32_t i = 1; i <= 16; i++) {
    printf("i    = %u\n", i);
    rval_sum = ctx_jump(rval_sum.ctx, &i);
    printf("sum  = %u\n", *((uint32_t *) rval_sum.data));
    rval_prod = ctx_jump(rval_prod.ctx, &i);
    printf("prod = %u\n", *((uint32_t *) rval_prod.data));
  }
  return 0;
}
