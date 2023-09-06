#include <stdint.h>
#include <math.h>

#include <xs1.h>
#include <platform.h>
#include <xcore/channel_streaming.h>
#include <xcore/port.h>

#include "delta_sigma.h"


const float delta_sigma3_params[6] = {
  -1.0, 1.0, // out_lo, out_hi
  0.79981585, 0.28802756, 0.04386536, // c[0], c[1], c[2]
  0.00016063 // f
};


// void delta_sigma3_thread(
//     streaming_chanend_t c_pcm_in, 
//     port p_pdm_out)
// {

//   const float conversion = ldexpf(1.0, -31);

//   float state[4] = {0,0,0,1.0};

//   const float ds3_params[6] = {
//     -1.0, 1.0, // out_lo, out_hi
//     0.79981585, 0.28802756, 0.04386536, // c[0], c[1], c[2]
//     0.00016063 // f
//   };


//   while(1){
//     float signal_in[32];

//     for(int k = 0; k < 32; k++) {
//       signal_in[k] = s_chan_in_word(c_pcm_in) * conversion;
//     }

//     const uint32_t pdm_chunk = delta_sigma3_float(signal_in, 
//                                                   state, 
//                                                   ds3_params);

//     port_out(p_pdm_out, pdm_chunk);
//   }
// }