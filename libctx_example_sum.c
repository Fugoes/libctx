#include <stdio.h>
#include <stdlib.h>
#include "libctx.h"

#define STACK_SIZE 8192

struct ctx_with_data {
  struct ctx _p;
  int data;
};

char stack[STACK_SIZE];
char stack_mul[STACK_SIZE];

void func(struct ctx *ctx_l, struct ctx *ctx_r) {
  for (;;) {
    ctx_switch(ctx_r, ctx_l);
    ((struct ctx_with_data *) ctx_r)->data += ((struct ctx_with_data *) ctx_l)->data;
  }
}

void func_mul(struct ctx *ctx_l, struct ctx *ctx_r) {
  for (;;) {
    ctx_switch(ctx_r, ctx_l);
    ((struct ctx_with_data *) ctx_r)->data *= ((struct ctx_with_data *) ctx_l)->data;
  }
}

int main() {
  struct ctx_with_data *ctx_main = malloc(sizeof(struct ctx_with_data));
  struct ctx_with_data *ctx_sum = malloc(sizeof(struct ctx_with_data));
  struct ctx_with_data *ctx_mul = malloc(sizeof(struct ctx_with_data));

  ctx_sum->data = 0;
  ctx_make(&ctx_sum->_p, stack + STACK_SIZE, &func);
  ctx_switch(&ctx_main->_p, &ctx_sum->_p);

  ctx_mul->data = 1;
  ctx_make(&ctx_mul->_p, stack_mul + STACK_SIZE, &func_mul);
  ctx_switch(&ctx_main->_p, &ctx_mul->_p);

  for (int i = 1; i <= 10; i++) {
    ctx_main->data = i;
    ctx_switch(&ctx_main->_p, &ctx_sum->_p);
    printf("sum = %d\n", ctx_sum->data);
    ctx_switch(&ctx_main->_p, &ctx_mul->_p);
    printf("mul = %d\n", ctx_mul->data);
  }

  free(ctx_sum);
  free(ctx_main);
  free(ctx_mul);

  return 0;
}
