// Copyright (c) 2023, XMOS Ltd, All rights reserved
    
#if defined(__XS3A__)

.text
.issue_mode  dual

/*


void upsample_x8(
    int32_t sample_out[8],
    const int32_t sample_in,
    const int32_t filter[8][32],
    int32_t state[32]);

*/


#define FUNC_NAME     upsample_x8
#define NSTACKWORDS   0

.globl	FUNC_NAME
.type	FUNC_NAME,@function
.cc_top FUNC_NAME.function,FUNC_NAME

#define smp_out   r0
#define smp_in    r1
#define coef      r2
#define state     r3

#define _32       smp_in

.align 16
FUNC_NAME:
  dualentsp NSTACKWORDS

{ ldc r11, 0                  ; stw smp_in, state[0]        }
{ ldc _32, 32                 ; vsetc r11                   }

{                             ; vclrdr                      }
{ add state, state, _32       ; vldc state[0]               }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }

{ add state, state, _32       ; vldc state[0]               }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }

{ add state, state, _32       ; vldc state[0]               }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }

{                             ; vldc state[0]               }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{ add coef, coef, _32         ; vlmaccr coef[0]             }
{                             ; vlmaccr coef[0]             }

{ sub r11, state, 4           ; vstr smp_out[0]             }

// Shift the filter state over one word
{ sub r11, r11, _32           ; vldr r11[0]                 }
{ sub state, state, _32       ; vstr state[0]               }
{ sub r11, r11, _32           ; vldr r11[0]                 }
{ sub state, state, _32       ; vstr state[0]               }
{ sub r11, r11, _32           ; vldr r11[0]                 }
{ sub state, state, _32       ; vstr state[0]               }
{ sub r11, r11, _32           ; vldr r11[0]                 }
{ sub state, state, _32       ; vstr state[0]               }

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