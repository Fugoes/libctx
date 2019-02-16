#ifndef LIBCTX_H
#define LIBCTX_H

#include <stdint.h>

#if defined(__i386__)
typedef uint32_t ctx_t;
typedef void (*ctx_func_t)(ctx_t ctx, void *data);
typedef struct {
  ctx_t ctx;
  void *data;
} ctx_rval_t;

ctx_t ctx_make(uint8_t *stack, ctx_func_t func);
ctx_rval_t ctx_jump(ctx_t ctx, void *data);

void __ctx_init();

//   0 1 2 3 4 5 6 7 8 9 a b c d e f
// 0 [ ebx  ][ esi  ][ edi  ][ ebp  ]
// 1 [mxcsr ][  cw  ][ rip  ][hidden]
// 2 [ arg1 ][ arg2 ]

__asm__(
"\
.globl ctx_jump                                                               \n\
.align 16                                                                     \n\
.type ctx_jump, @function                                                     \n\
ctx_jump:                                                                     \n\
    leal    -0x18(%esp), %esp                                                 \n\
    movl    %esp, %ecx                                                        \n\
    movl    0x24(%esp), %edx                                                  \n\
    movl    %ebx, 0x00(%esp)                                                  \n\
    movl    %esi, 0x04(%esp)                                                  \n\
    movl    %edi, 0x08(%esp)                                                  \n\
    movl    %ebp, 0x0c(%esp)                                                  \n\
    stmxcsr 0x10(%esp)                                                        \n\
    fnstcw  0x14(%esp)                                                        \n\
    movl    0x20(%esp), %esp                                                  \n\
    movl    0x1c(%esp), %eax                                                  \n\
    movl    %ecx, 0x00(%eax)                                                  \n\
    movl    %edx, 0x04(%eax)                                                  \n\
    movl    0x18(%esp), %ecx                                                  \n\
    movl    0x00(%esp), %ebx                                                  \n\
    movl    0x04(%esp), %esi                                                  \n\
    movl    0x08(%esp), %edi                                                  \n\
    movl    0x0c(%esp), %ebp                                                  \n\
    ldmxcsr 0x10(%esp)                                                        \n\
    fldcw   0x14(%esp)                                                        \n\
    leal    0x20(%esp), %esp                                                  \n\
    jmpl    *%ecx                                                             \n\
"
);

__asm__(
"\
.globl __ctx_init                                                             \n\
.align 16                                                                     \n\
.type __ctx_init, @function                                                   \n\
__ctx_init:                                                                   \n\
    leal    -0x04(%esp), %esp                                                 \n\
    movl    0x10(%esp), %ecx                                                  \n\
    movl    0x0c(%esp), %eax                                                  \n\
    movl    0x08(%esp), %edx                                                  \n\
    movl    %eax, 0x04(%esp)                                                  \n\
    movl    %edx, 0x00(%esp)                                                  \n\
    calll   *%ecx                                                             \n\
"
);

ctx_t ctx_make(uint8_t *stack, ctx_func_t func) {
  uint32_t *s = (uint32_t *) stack;

  s--;
  *s = 1;
  s--;
  *s = 2;
  s--;
  *s = 3;
  s--;
  *s = (uint32_t) func;
  s--;
  *s = 4;
  s--;
  *s = 5;

  uint32_t *p = s;

  s--;
  *s = 6;
  s--;
  *s = (uint32_t) p;

  s--;
  *s = (uint32_t) &__ctx_init;
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

  return (ctx_t) s;
}
#elif defined(__amd64__)
typedef uint64_t ctx_t;
typedef void (*ctx_func_t)(ctx_t ctx, void *data);
typedef struct {
  ctx_t ctx;
  void *data;
} ctx_rval_t;

ctx_t ctx_make(uint8_t *stack, ctx_func_t func);
ctx_rval_t ctx_jump(ctx_t ctx, void *data);

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

ctx_t ctx_make(uint8_t *stack, ctx_func_t func) {
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

  return (ctx_t) s;
}
#endif

#endif //LIBCTX_H
