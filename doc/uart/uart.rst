.. include:: ../substitutions.rst

############
UART Library
############

This library provide a software defined UART (universal asynchronous receiver transmitter) allowing you to communicate with other UART enabled devices in your system. A UART is a single wire per direction communications interface allowing either half or full duplex communication. The components in this library are controlled via C and behave as a UART transmitter and/or receiver peripheral.

Various configuration options are available including baud rate (individually settable per direction), number of data bits (between 5 and 8), parity (EVEN, ODD or NONE) and number of stop bits (1 or 2). The UART does not support flow control signals. Only a single 1b IO port per UART direction is needed.

The Tx UART supports up to 1152000 baud unbuffered and 576000 baud buffered with a 75MHz logical core. The Rx UART supports up to 700000 baud unbuffered and 422400 baud buffered with a 75MHz logical core. Proportionally higher rates are achievable using a higher logical core MHz.

The UART receive supports standard error detection including START, PARITY and FRAMING errors. A callback mechanism is included to notify the user of these conditions. 

The UART may be used in blocking mode, where the call to Tx/Rx does not return until the stop bit is complete. It may also be used in ISR/buffered mode where the UART Rx and/or Tx operates in background mode using a FIFO and callbacks to manage data-flow and error conditions. Cycles are stolen from the logical core which setup the interrupt. In ISR/buffered mode additional callbacks are supported indicating the UNDERRUN condition when the Tx buffer is empty and OVERRUN when the Rx buffer is full.


.. _uart_wire_table:

.. list-table:: UART data wires
     :class: vertical-borders horizontal-borders

     * - *Tx*
       - Transmit line controlled by UART Tx
     * - *Rx*
       - Receive line controlled by UART Rx


All UART functions can be accessed via the ``uart.h`` header:

.. code-block:: c
   
   #include <uart.h>

.. toctree::
   :maxdepth: 2
   :includehidden:

   uart_tx.rst
   uart_rx.rst
