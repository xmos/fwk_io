.. include:: ../../../substitutions.rst

*********
TDM Slave
*********

TDM Slave Tx Usage
==================

The following code snippet demonstrates the basic usage of a TDM slave Tx device.

.. code-block:: c

   #include <xs1.h>
   #include "i2s_tdm_slave.h"

   // Setup ports and clocks
   port_t p_bclk = XS1_PORT_1A;
   port_t p_fsync = XS1_PORT_1B;
   port_t p_dout = XS1_PORT_1C;

   xclock_t clk_bclk = XS1_CLKBLK_1;

   // Setup callbacks
   // NOTE: See API or sln_voice examples for more on using the callbacks
   i2s_tdm_ctx_t ctx;
   i2s_callback_group_t i_i2s = {
           .init = (i2s_init_t) i2s_init,
           .restart_check = (i2s_restart_check_t) i2s_restart_check,
           .receive = NULL,
           .send = (i2s_send_t) i2s_send,
           .app_data = NULL,
   };

   // Initialize the TDM slave
   i2s_tdm_slave_tx_16_init(
           &ctx,
           &i_i2s,
           p_dout,
           p_fsync,
           p_bclk,
           clk_bclk,
           0,
           I2S_SLAVE_SAMPLE_ON_BCLK_FALLING,
           NULL);

   // Start the slave device in this thread
   // NOTE: You may wish to launch the slave device in a different thread.  
   //       See the XTC Tools documentation reference for lib_xcore.
   i2s_tdm_slave_tx_16_thread(&ctx);

TDM Slave Tx API
================

The following structures and functions are used to initialize and start a TDM slave Tx instance.

.. doxygengroup:: hil_i2s_tdm_slave_tx16
   :content-only:

