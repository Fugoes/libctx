#ifndef LIBCTX_H
#define LIBCTX_H

struct ctx {
  void *rip;
  void *rsp;
  void *rbx;
  void *rbp;
  void *r12;
  void *r13;
  void *r14;
  void *r15;
  void *mxcsr_and_cw;
};

typedef void (*ctx_task_t)(struct ctx *ctx_l, struct ctx *ctx_r);

// switch from ctx_l to ctx_r
void *ctx_switch(struct ctx *ctx_l, struct ctx *ctx_r);
void ctx_make(struct ctx *ctx, char *stack_base, ctx_task_t ctx_task);

__asm__(
".globl ctx_switch\n"
".type ctx_switch, @function\n" // %rdi: ctx_l, %rsi: ctx_r
"ctx_switch:\n"
// store %rip in %r10
"    movq    0x00(%rsp), %r10\n"
// store %rsp in %r11
"    leaq    0x08(%rsp), %r11\n"
// saving %rip and %rsp
"    movq    %r10, 0x00(%rdi)\n"
"    movq    %r11, 0x08(%rdi)\n"
// saving registers
"    movq    %rbx, 0x10(%rdi)\n"
"    movq    %rbp, 0x18(%rdi)\n"
"    movq    %r12, 0x20(%rdi)\n"
"    movq    %r13, 0x28(%rdi)\n"
"    movq    %r14, 0x30(%rdi)\n"
"    movq    %r15, 0x38(%rdi)\n"
// store MXCSR
"    stmxcsr 0x40(%rdi)\n"
// store x87 FPU control word
"    fnstcw  0x44(%rdi)\n"
// setting %rip and %rsp
"    movq    0x00(%rsi), %r10\n"
"    movq    0x08(%rsi), %rsp\n"
// setting registers
"    movq    0x10(%rsi), %rbx\n"
"    movq    0x18(%rsi), %rbp\n"
"    movq    0x20(%rsi), %r12\n"
"    movq    0x28(%rsi), %r13\n"
"    movq    0x30(%rsi), %r14\n"
"    movq    0x38(%rsi), %r15\n"
// load MXCSR
"    ldmxcsr 0x40(%rsi)\n"
// load x87 FPU control word
"    fldcw   0x44(%rsi)\n"
// jump!
"    jmpq    *%r10\n"
);

void ctx_make(struct ctx *ctx, char *stack_base, ctx_task_t ctx_task) {
  ctx->rip = (void *) ctx_task;
  ctx->rsp = stack_base;
  ctx->rbx = 0;
  ctx->r12 = 0;
  ctx->r13 = 0;
  ctx->r14 = 0;
  ctx->r15 = 0;
  ctx->mxcsr_and_cw = 0;
}

#endif //LIBCTX_H
