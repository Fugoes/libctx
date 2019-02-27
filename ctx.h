#ifndef LIBCTX_H
#define LIBCTX_H

#include <stdint.h>

#if defined(__i386__)
typedef struct {
  uint32_t eip;
  uint32_t esp;
  uint32_t ebx;
  uint32_t esi;
  uint32_t edi;
  uint32_t ebp;
  uint32_t mxcsr;
  uint32_t cw;
} ctx_t;

ctx_t *ctx_switch(ctx_t *ctx_from, ctx_t *ctx_to);
void ctx_init(ctx_t *ctx, uint8_t *stack_hi, void (*func)(ctx_t *, ctx_t *, void *), void *arg);

void __ctx_wrapper();

__asm__(
"\
.globl ctx_switch                    \n\
.type ctx_switch, @function          \n\
.align 4                             \n\
ctx_switch:                          \n\
    movl    0x04(%esp), %eax         \n\
    popl    %edx                     \n\
    movl    %edx, 0x00(%eax)         \n\
    movl    %esp, 0x04(%eax)         \n\
    movl    %ebx, 0x08(%eax)         \n\
    movl    %esi, 0x0c(%eax)         \n\
    movl    %edi, 0x10(%eax)         \n\
    movl    %ebp, 0x14(%eax)         \n\
    stmxcsr 0x18(%eax)               \n\
    fnstcw  0x1c(%eax)               \n\
    movl    0x04(%esp), %ecx         \n\
    movl    0x00(%ecx), %edx         \n\
    movl    0x04(%ecx), %esp         \n\
    movl    0x08(%ecx), %ebx         \n\
    movl    0x0c(%ecx), %esi         \n\
    movl    0x10(%ecx), %edi         \n\
    movl    0x14(%ecx), %ebp         \n\
    ldmxcsr 0x18(%ecx)               \n\
    fldcw   0x1c(%ecx)               \n\
    jmpl    *%edx                    \n\
"
);

__asm__(
"\
.globl __ctx_wrapper                 \n\
.type __ctx_wrapper, @function       \n\
.align 4                             \n\
__ctx_wrapper:                       \n\
    movl    0x0c(%esp), %ecx         \n\
    movl    %eax, 0x00(%esp)         \n\
    calll   *%ecx                    \n\
"
);

void ctx_init(ctx_t *ctx, uint8_t *stack_hi, void (*func)(ctx_t *, ctx_t *, void *), void *arg) {
  uint32_t *esp = (uint32_t *) stack_hi;
  esp--;
  *esp = (uint32_t) func;
  esp--;
  *esp = (uint32_t) arg;
  esp--;
  *esp = (uint32_t) ctx;
  esp--;
  ctx->eip = (uint32_t) &__ctx_wrapper;
  ctx->esp = (uint32_t) esp;
  ctx->ebx = 0;
  ctx->esi = 0;
  ctx->edi = 0;
  ctx->ebp = 0;
  ctx->mxcsr = 0x00001f80;
  ctx->cw = 0x0000037f;
}
#elif defined(__amd64__)
typedef struct {
  uint64_t rip;
  uint64_t rsp;
  uint64_t rbx;
  uint64_t rbp;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t mxcsr_cw;
} ctx_t;

ctx_t *ctx_switch(ctx_t *ctx_from, ctx_t *ctx_to);
void ctx_init(ctx_t *ctx, uint8_t *stack_hi, void (*func)(ctx_t *, ctx_t *, void *), void *data);

void __ctx_wrapper();

__asm__(
"\
.globl ctx_switch                    \n\
.type ctx_switch, @function          \n\
.align 16                            \n\
ctx_switch:                          \n\
    popq    %r10                     \n\
    movq    %rdi, %rax               \n\
    movq    %r10, 0x00(%rdi)         \n\
    movq    %rsp, 0x08(%rdi)         \n\
    movq    %rbx, 0x10(%rdi)         \n\
    movq    %rbp, 0x18(%rdi)         \n\
    movq    %r12, 0x20(%rdi)         \n\
    movq    %r13, 0x28(%rdi)         \n\
    movq    %r14, 0x30(%rdi)         \n\
    movq    %r15, 0x38(%rdi)         \n\
    stmxcsr 0x40(%rdi)               \n\
    fnstcw  0x44(%rdi)               \n\
    movq    0x00(%rsi), %r11         \n\
    movq    0x08(%rsi), %rsp         \n\
    movq    0x10(%rsi), %rbx         \n\
    movq    0x18(%rsi), %rbp         \n\
    movq    0x20(%rsi), %r12         \n\
    movq    0x28(%rsi), %r13         \n\
    movq    0x30(%rsi), %r14         \n\
    movq    0x38(%rsi), %r15         \n\
    ldmxcsr 0x40(%rsi)               \n\
    fldcw   0x44(%rsi)               \n\
    jmpq    *%r11                    \n\
"
);

/*
 * Sometimes the compiler would use `jmp` instead of `ret` to save instructions, and when the caller calls `ret`, it
 * would return to the caller of the caller. If some caller called `ctx_switch` with `jmp`, `ctx_switch` would read
 * `0x00(%rsp)`, so we need to make sure that `%rsp` is a valid readable address.
 */
__asm__(
"\
.globl __ctx_wrapper                 \n\
.type __ctx_wrapper, @function       \n\
.align 16                            \n\
__ctx_wrapper:                       \n\
    movq    0x08(%rsp), %r10         \n\
    movq    0x00(%rsp), %rdx         \n\
    leaq    0x10(%rsp), %rsp         \n\
    callq   *%r10                    \n\
"
);

void ctx_init(ctx_t *ctx, uint8_t *stack_hi, void (*func)(ctx_t *, ctx_t *, void *), void *data) {
  uint64_t *rsp = (uint64_t *) stack_hi;
  rsp--;
  *rsp = (uint64_t) func;
  rsp--;
  *rsp = (uint64_t) data;
  ctx->rip = (uint64_t) __ctx_wrapper;
  ctx->rsp = (uint64_t) rsp;
  ctx->rbx = 0;
  ctx->rbp = 0;
  ctx->r12 = 0;
  ctx->r13 = 0;
  ctx->r14 = 0;
  ctx->r15 = 0;
  ctx->mxcsr_cw = 0x0000037f00001f80;
}
#endif

#endif //LIBCTX_H
