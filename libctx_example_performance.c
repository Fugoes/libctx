// running 2000000000 context switch

#include <stdio.h>
#include <stdlib.h>
#include "libctx.h"

#define STACK_SIZE 4096

void func(ctx_t *ctx_from, ctx_t *ctx_to, void *arg) {
  for (;;) {
    ctx_from = ctx_switch(ctx_to, ctx_from);
  }
}

ctx_t ctx_main;
ctx_t ctx_func;

uint8_t stack_func[STACK_SIZE];

int main() {
  ctx_init(&ctx_func, &func, stack_func + STACK_SIZE, 0);
  for (int i = 0; i < 1000000000; i++) {
    ctx_switch(&ctx_main, &ctx_func);
  }
  return 0;
}
