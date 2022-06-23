.. include:: ../substitutions.rst

*******
UART Tx
*******

UART Tx Usage
=============

The following code snippet demonstrates the basic blocking usage of an UART Tx device.

.. code-block:: c

   #include <xs1.h>
   #include "uart.h"

   uart_tx_t uart;

   port_t p_uart_tx = XS1_PORT_1A;
   hwtimer_t tmr = hwtimer_alloc();

   uint8_t tx_data[4] = {0x01, 0x02, 0x04, 0x08};

   // Initialize the UART Tx
   uart_tx_blocking_init(&uart, p_uart_tx, 115200, 8, UART_PARITY_NONE, 1, tmr);

   // Transfer some data
   for(int i = 0; i < sizeof(tx_data); i++){
      uart_tx(&uart, tx_data[i]);
   }


UART Tx Usage ISR/Buffered
==========================

The following code snippet demonstrates the usage of an UART Tx device used in ISR/Buffered mode:

.. code-block:: c

  #include <xs1.h>
  #include "uart.h"


  HIL_UART_TX_CALLBACK_ATTR void tx_empty_callback(void *app_data){
        int *tx_empty = (int *)app_data;
        *tx_empty = 1;
  }

  void uart_tx(void){

      uart_tx_t uart;
      port_t p_uart_tx = XS1_PORT_1A;
      hwtimer_t tmr = hwtimer_alloc();
      uint8_t buffer[64 + 1] = {0}; // Note buffer size plus one

      uint8_t tx_data[4] = {0x01, 0x02, 0x04, 0x08};
      volatile int tx_empty = 0;

      // Initialize the UART Tx
      uart_tx_init(&uart, p_uart_tx, 115200, 8, UART_PARITY_NONE, 1, tmr, buffer, sizeof(buffer), tx_empty_callback, &tx_empty);

      // Transfer some data
      for(int i = 0; i < sizeof(tx_data); i++){
         uart_tx(&uart, tx_data[i]);
      }

      // Wait for it to complete
      while(!tx_empty);


UART Tx API
===========

The following structures and functions are used to initialize and start an UART Tx instance.

.. doxygengroup:: hil_uart_tx
   :content-only:
