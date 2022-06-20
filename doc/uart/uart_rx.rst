.. include:: ../substitutions.rst

*******
UART Rx
*******

UART Rx Usage
=============

The following code snippet demonstrates the basic usage of an UART Rx device where the function call to Rx returns after the stop bit has been sampled. The function blocks until a complete byte has been received.

.. code-block:: c

   #include <xs1.h>
   #include <print.h>
   #include "uart.h"


   HIL_UART_RX_CALLBACK_ATTR void rx_error_callback(uart_callback_code_t callback_code, void *app_data){
       switch(callback_code){
           case UART_START_BIT_ERROR:
               printstrln("UART_START_BIT_ERROR");
               break;
           case UART_PARITY_ERROR:
               printstrln("UART_PARITY_ERROR");
               break;
           case UART_FRAMING_ERROR:
               printstrln("UART_FRAMING_ERROR");
               test_abort = 1;
               break;
           case UART_OVERRUN_ERROR:
               printstrln("UART_OVERRUN_ERROR");
               break;
           case UART_UNDERRUN_ERROR:
               printstrln("UART_UNDERRUN_ERROR");
               break;
           default:
               printstr("Unexpected callback code: ");
               printintln(callback_code);
       }
   }

   void uart_rx(void){

       uart_rx_t uart;

       port_t p_uart_rx = XS1_PORT_1B;
       hwtimer_t tmr = hwtimer_alloc();

       char test_rx[16];

       // Initialize the UART Rx
       uart_rx_blocking_init(  &uart, p_uart_rx, 115200, 8, UART_PARITY_NONE, 1, tmr,
                               rx_error_callback, &uart);

       // Receive some data
       for(int i = 0; i < sizeof(rx_data); i++){
          test_rx[i] = uart_rx(&uart);
       }


UART Rx Usage ISR/Buffered
==========================

The following code snippet demonstrates the usage of an UART Rx device used in ISR/Buffered mode:

.. code-block:: c

   #include <xs1.h>
   #include <print.h>
   #include "uart.h"


   HIL_UART_RX_CALLBACK_ATTR void rx_error_callback(uart_callback_code_t callback_code, void *app_data){
       switch(callback_code){
           case UART_START_BIT_ERROR:
               printstrln("UART_START_BIT_ERROR");
               break;
           case UART_PARITY_ERROR:
               printstrln("UART_PARITY_ERROR");
               break;
           case UART_FRAMING_ERROR:
               printstrln("UART_FRAMING_ERROR");
               test_abort = 1;
               break;
           case UART_OVERRUN_ERROR:
               printstrln("UART_OVERRUN_ERROR");
               break;
           case UART_UNDERRUN_ERROR:
               printstrln("UART_UNDERRUN_ERROR");
               break;
           default:
               printstr("Unexpected callback code: ");
               printintln(callback_code);
       }
   }


  HIL_UART_RX_CALLBACK_ATTR void rx_callback(void *app_data){
        unsigned *bytes_received = (unsigned *)app_data;
        *bytes_received += 1;
  }

  void uart_rx(void){

      uart_rx_t uart;
      port_t p_uart_rx = XS1_PORT_1A;
      hwtimer_t tmr = hwtimer_alloc();
      uint8_t buffer[64 + 1] = {0}; // Note buffer size plus one

      volatile unsigned bytes_received = 0;

      // Initialize the UART Rx
      uart_rx_init(&uart, p_uart_rx, 115200, 8, UART_PARITY_NONE, 1, tmr, 
                   buffer, sizeof(buffer), rx_callback, &bytes_received);

      // Wait for 16b of data
      while(bytes_received < 15);

      // Get the data
      uint8_t test_rx[NUM_RX_WORDS];
      for(int i = 0; i < 16; i++){
          test_rx[i] = uart_rx(&uart);
      }


UART Rx API
===========

The following structures and functions are used to initialize and start an UART Rx instance.

.. doxygengroup:: hil_uart_rx
   :content-only:
