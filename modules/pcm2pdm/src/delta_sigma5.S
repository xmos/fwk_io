// Copyright (c) 2023, XMOS Ltd, All rights reserved
    
#if defined(__XS3A__)

.text
.issue_mode  dual

/*
  5th order delta-sigma modulator implementation.

  This function takes in a vector of 32 PCM samples as int32_t and outputs a
  single uint32_t containing the next 32 PDM samples.

  Note: pcm_in[] must be word-aligned.

  The matrix should look like this:
    M = np.array([
    [   1,      0,     0,      0,     0,   1,  -1 ],
    [   1, 1-f[0], -f[0],      0,     0,   0,   0 ],
    [   0,      1,     1,      0,     0,   0,   0 ],
    [   0,      0,     1, 1-f[1], -f[1],   0,   0 ],
    [   0,      0,     0,      1,     1,   0,   0 ],
  ])

  where f[0] and f[1] are the first and second feedback coefficients.

  However, the matrix is stored with the rows in reverse order, and with an 
  8th column of 0's to make it more VPU friendly.

  typedef struct {
    // state vector. s[0:5] are the integrator states. s[5] is the previous
    // output, and s[6] will hold the current input. s[7] is zero (for VPU)
    int32_t s[8];
    // 5x8 matrix of coefficients. The last column should be all 0's
    // rows are in reverse order
    int32_t matrix[5][8];
    // coefficients used in inner product.
    int32_t c[8];
  } delta_sigma5_context_t;

  uint32_t delta_sigma5(
      const int32_t pcm_in[],
      delta_sigma5_context_t *ctx);

*/

#define FUNC_NAME     delta_sigma5
#define NSTACKWORDS   14

.globl	FUNC_NAME
.type	FUNC_NAME,@function
.cc_top FUNC_NAME.function,FUNC_NAME

// This will store 32 bytes, each corresponding to one output bit.
// They will get converted to a single uint32_t at the end.
#define STK_VEC_OUT     (NSTACKWORDS-8)

#define input     r0
#define s         r1
#define ctx       r2
#define _32       r3

#define tmp       r4
#define output    r5
#define n         r6
#define s5        r7
#define s5_mask   r8
#define tmp2      r9  


.align 16
FUNC_NAME:
  dualentsp NSTACKWORDS
  std r4, r5, sp[0]
  std r6, r7, sp[1]
  std r8, r9, sp[2]

{ ldc r11, 0                  ; ldc _32, 32                 }
{ add ctx, s, _32             ; vsetc r11                   }
{ mkmsk s5_mask, 4            ; ldaw output, sp[STK_VEC_OUT]}
  ldaw s5, s[5]
{ mov n, _32                  ; ldw tmp, input[0]           }
{                             ; stw tmp, s[6]               }
{                             ; bu .L_loop_top              }

.align 16
.L_loop_top:
  // Do the matrix multiply
  { sub n, n, 1                 ; vclrdr                      }
  {                             ; vldc s[0]                   }
  { add tmp, ctx, _32           ; vlmaccr ctx[0]              }
  { add tmp, tmp, _32           ; vlmaccr tmp[0]              }
  { add tmp, tmp, _32           ; vlmaccr tmp[0]              }
  { add tmp, tmp, _32           ; vlmaccr tmp[0]              }
  { add tmp, tmp, _32           ; vlmaccr tmp[0]              }

  // Write s back to ctx  (last 3 elements will be 0)
  {                             ; vstr s[0]                   }

  /////////// FNOP //////////

  // inner product
  {                             ; vclrdr                      }
  {                             ; vldc s[0]                   }
  {                             ; vlmaccr tmp[0]              }


  // astew: Instead of doing VSIGN here, writing the word to memory, loading it
  //        back and writing it again and all that, I think maybe this can be
  //        simplified to do a VPOS, followed by a VEQDR, which, assuming vD is
  //        known to be all zeros, will put 1 in r10 iff the input was negative.
  //        But that assumes vD is known to be all zeros... but the VLMACCR 
  //        could have resulted in a negative number.
  //        Could work around that by loading 0's into vD explicitly.
  //            VLDD zeros[0]
  //            VPOS
  //            { shl output, output, 1;  VEQDR }
  //            { shl r10, r10, 29 ;  or output, output r10 }
  //              stw r10, s[5]
  //        If this works it could save 1 instruction and 1 FNOP.

  // take the sign of the inner product, and that is our next output
  {                             ; vsign                       }

  // write that output back to s[5]
    vstrpv s5[0], s5_mask
  
  // write one byte to output buffer and
  // move next input into s[6]
  {                             ; ldw tmp, s5[0]              }
  { shr tmp, tmp, 24            ; ldw tmp2, input[0]          }
    st8 tmp, output[n]

  /////////// FNOP //////////

  { add input, input, 4         ; stw tmp2, s[6]              }
  
  // loop
  { mov r11, output             ; bt n, .L_loop_top           }
  
{ shl r11, _32, 4             ; vldr r11[0]                 }
{ mov tmp, output             ; vsetc r11                   }
{                             ; vdepth1                     }
{                             ; vstr tmp[0]                 }
{                             ; ldw r0, tmp[0]              }

.L_done:
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