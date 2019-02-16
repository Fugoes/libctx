#ifndef LIBCTX_H
#define LIBCTX_H

#include <stdint.h>

#ifdef __amd64__
typedef void (*ctx_func)(void *ctx, void *data);
struct ctx_rval {
  void *ctx;
  void *data;
};

void *ctx_make(uint8_t *stack, ctx_func func);
struct ctx_rval ctx_jump(void *ctx, void *data);


void __ctx_init();

//   0 1 2 3 4 5 6 7 8 9 a b c d e f
// 0 [     rbx      ][     rbp      ]
// 1 [     r12      ][     r13      ]
// 2 [     r14      ][     r15      ]
// 3 [mxcsr ][  cw  ][     rip      ]

__asm__(
"\
.globl ctx_jump                                                               \n\
.type ctx_jump, @function                                                     \n\
.align 16                                                                     \n\
ctx_jump:                                                                     \n\
    leaq    -0x38(%rsp), %rsp                                                 \n\
    movq    %rsp, %rax                                                        \n\
    movq    %rsi, %rdx                                                        \n\
    movq    %rbx, 0x00(%rsp)                                                  \n\
    movq    %rbp, 0x08(%rsp)                                                  \n\
    movq    %r12, 0x10(%rsp)                                                  \n\
    movq    %r13, 0x18(%rsp)                                                  \n\
    movq    %r14, 0x20(%rsp)                                                  \n\
    movq    %r15, 0x28(%rsp)                                                  \n\
    stmxcsr 0x30(%rsp)                                                        \n\
    fnstcw  0x34(%rsp)                                                        \n\
    movq    %rdi, %rsp                                                        \n\
    movq    0x00(%rsp), %rbx                                                  \n\
    movq    0x08(%rsp), %rbp                                                  \n\
    movq    0x10(%rsp), %r12                                                  \n\
    movq    0x18(%rsp), %r13                                                  \n\
    movq    0x20(%rsp), %r14                                                  \n\
    movq    0x28(%rsp), %r15                                                  \n\
    ldmxcsr 0x30(%rsp)                                                        \n\
    fldcw   0x34(%rsp)                                                        \n\
    movq    0x38(%rsp), %r10                                                  \n\
    leaq    0x40(%rsp), %rsp                                                  \n\
    jmpq    *%r10                                                             \n\
"
);
__asm__(
"\
.globl __ctx_init                                                             \n\
.type __ctx_init, @function                                                   \n\
.align 16                                                                     \n\
__ctx_init:                                                                   \n\
    movq    (%rsp), %r10                                                      \n\
    movq    %rax, %rdi                                                        \n\
    movq    %rdx, %rsi                                                        \n\
    callq   *%r10                                                             \n\
"
);

void *ctx_make(uint8_t *stack, ctx_func func) {
  uint64_t *s = (uint64_t *) stack;

  s--;
  *s = 0;
  s--;
  *s = (uint64_t) func;

  s--;
  *s = (uint64_t) &__ctx_init;
  s--;
  *s = 0;
  s--;
  *s = 0;
  s--;
  *s = 0;
  s--;
  *s = 0;
  s--;
  *s = 0;
  s--;
  *s = 0;
  s--;
  *s = 0;

  return (void *) s;
}
#endif //__amd64__

#endif //LIBCTX_H
