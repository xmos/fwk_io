#pragma once

#include <stdint.h>

#include "xccompat.h"

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

extern
const int32_t upsample_x8_coef[256];

/**
 * 
 * Timing: Last measured at 560 ns at 600 MHz
 */
void upsample_x8(
    int32_t sample_out[8],
    const int32_t sample_in,
    const int32_t filter[256],
    int32_t state[32]);


extern
const int32_t upsample_x24_coef[120];


/**
 * 
 * Timing: Last measured at 410 ns at 600 MHz
 */
void upsample_x24(
    int32_t sample_out[24],
    const int32_t sample_in,
    const int32_t filter[120],
    int32_t state[8]);


// If upsample_x8 takes 560 ns, upsample_x24 takes 410 ns, and 
// delta_sigma3_float takes 4720 ns..

/*

  Each 16 kHz sample received gets one upsample_x8(), producing 8 samples
  at 128 kHz. Those 8 samples get one upsample_x24() each, producing a total of
  192 samples at 3.072 MHz. Every 32 of the 3.072 MHz samples gets one call to
  delta_sigma3_float(), which means 6 calls.

  There are 62500 nanoseconds between 16 kHz samples...

      1 * 560 
    + 8 * 410
    + 6 * 4720
    ==========
         32160 ns

  Shit... unless I did this math wrong, we should be able to do all of this
  in a single thread. 
*/


#if defined(__XC__) || defined(__cplusplus)
}
#endif