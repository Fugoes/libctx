// running 2000000000 context switch

#include <stdio.h>
#include <stdlib.h>
#include "ctx.h"

#define STACK_SIZE 4096

ctx_t ctx_main;
ctx_t ctx_func;

void func(ctx_t *ctx_from, ctx_t *ctx_to, void *arg) {
  printf("%p == %p, %p == %p, %p == (nil)\n", &ctx_main, ctx_from, &ctx_func, ctx_to, arg);
  printf("%f\n", 123 / 1.32);
  for (;;) {
    ctx_from = ctx_switch(ctx_to, ctx_from);
  }
}

uint8_t stack_func[STACK_SIZE];

int main() {
  ctx_init(&ctx_func, stack_func + STACK_SIZE, &func, 0);
  for (int i = 0; i < 1000000000; i++) {
    ctx_switch(&ctx_main, &ctx_func);
  }
  return 0;
}
