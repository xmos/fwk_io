#pragma once

#include <stdint.h>

#include "xccompat.h"

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

#define DELTA_SIGMA3_INITIAL_STATE    {0.0f, 0.0f, 0.0f, 1.0f};

extern
const float delta_sigma3_params[6];

/**
 * 
 * Timing: Last measured at 4720 ns at 600 MHz
 */
uint32_t delta_sigma3_float(
    const float pcm_in[32],
    float state[4],
    const float ds3_params[6]);


void vect_to_floats(
    float output[],
    int32_t input[],
    const int exponent,
    const unsigned length);

void chan_in_to_floats(
    float sample_out[32],
    chanend c_samples,
    const int exponent);



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



#if defined(__XC__) || defined(__cplusplus)
}
#endif