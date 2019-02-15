#include <stdio.h>
#include <stdlib.h>
#include "libctx.h"

#define STACK_SIZE 8192

char stack[STACK_SIZE];

void func(struct ctx *ctx_l, struct ctx *ctx_r) {
  for (;;) {
    ctx_switch(ctx_r, ctx_l);
  }
}

int main() {
  struct ctx *ctx_main = malloc(sizeof(struct ctx));
  struct ctx *ctx_func = malloc(sizeof(struct ctx));

  ctx_make(ctx_func, stack + STACK_SIZE, &func);

  for (int i = 1; i <= 1000000000 / 2; i++) {
    ctx_switch(ctx_main, ctx_func);
  }

  free(ctx_func);
  free(ctx_main);

  return 0;
}
