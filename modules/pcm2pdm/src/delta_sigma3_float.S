// Copyright (c) 2023, XMOS Ltd, All rights reserved
    
#if defined(__XS3A__)

.text
.issue_mode  dual

/*


uint32_t delta_sigma3_float(
    const int32_t pcm_in[32],
    float state[4],
    const float[6] ds3_params);


*/


#define FUNC_NAME     delta_sigma3_float
#define NSTACKWORDS   8

.globl	FUNC_NAME
.type	FUNC_NAME,@function
.cc_top FUNC_NAME.function,FUNC_NAME


#define input     r0
#define state     r1
#define params    r2
#define res       r3

#define s0        r4
#define s1        r5
#define s2        r6
#define n         r7
#define p         r8
#define t         r9
#define zero      r10
#define d_out     r11

#define d_in      t

#define C_OFFSET    2
#define F_OFFSET    5

.align 16
FUNC_NAME:
  dualentsp NSTACKWORDS
  std r4, r5, sp[0]
  std r6, r7, sp[1]
  std r8, r9, sp[2]

  ldd s1, s0, state[0]
  ldd d_out, s2, state[1]
{ ldc n, 32                   ; stw r10, sp[6]              }
{ ldc zero, 0                 ; ldc res, 0                  }

.L_loop_top:
  { add input, input, 4         ; ldw d_in, input[0]          }
    fadd s2, s2, s1
    fadd s1, s1, s0
  { shl res, res, 1             ; ldw p, params[F_OFFSET]     }
    fmul p, s2, p
    fsub s1, s1, p
    fsub p, d_in, d_out
    fadd s0, s0, p
  {                             ; ldw p, params[C_OFFSET+0]   }
    fmul t, s0, p
  {                             ; ldw p, params[C_OFFSET+1]   }
    fmacc t, t, s1, p
  {                             ; ldw p, params[C_OFFSET+2]   }
    fmacc t, t, s2, p
    fgt p, t, zero
  { sub n, n, 1                 ; ldw d_out, params[p]        }
  { or res, res, p              ; bt n, .L_loop_top           }
  
  std s1, s0, state[0]
  std d_out, s2, state[1]

.L_done:
{ mov r0, res                 ; ldw r10, sp[6]              }
  bitrev r0, r0

  ldd r4, r5, sp[0]
  ldd r6, r7, sp[1]
  ldd r8, r9, sp[2]
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