// Copyright (c) 2023, XMOS Ltd, All rights reserved
    
#if defined(__XS3A__)

.text
.issue_mode  dual

/*
void vect_to_floats(
    float output[],
    int32_t input[],
    const int exponent,
    const unsigned length);
*/

#define FUNC_NAME     vect_to_floats
#define NSTACKWORDS   4

.globl	FUNC_NAME
.type	FUNC_NAME,@function
.cc_top FUNC_NAME.function,FUNC_NAME

#define output    r0
#define input     r1
#define exp       r2
#define n         r3

#define sign      r4
#define sample    r5
#define tmpA      r6
#define tmpB      r7 

#define zero      r11
  
.align 16
FUNC_NAME:
  dualentsp NSTACKWORDS
  std r5, r4, sp[1]

{ ldc zero, 23                ; sub n, n, 1                 }
{ add exp, exp, zero          ; ldc zero, 0                 }

.L_loop:
  {                             ; ldw sample, input[n]        }
  { lss sign, sample, zero      ;                             }
  {                             ; bf sign, .L_pos             }

  .L_neg:
    neg sample, sample
  .L_pos:
    fmake sample, sign, exp, zero, sample
  
  {                             ; stw sample, output[n]       }
  { sub n, n, 1                 ; bt n, .L_loop               }
  
  
  ldd r5, r4, sp[1]
  retsp NSTACKWORDS


  .cc_bottom FUNC_NAME.function
	.set	FUNC_NAME.nstackwords,NSTACKWORDS;     .globl	FUNC_NAME.nstackwords
	.set	FUNC_NAME.maxcores,1;                  .globl	FUNC_NAME.maxcores
	.set	FUNC_NAME.maxtimers,0;                 .globl	FUNC_NAME.maxtimers
	.set	FUNC_NAME.maxchanends,0;               .globl	FUNC_NAME.maxchanends
.Ltmp1:
	.size	FUNC_NAME, .Ltmp1-FUNC_NAME

#undef NSTACKWORDS


#endif