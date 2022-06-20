.. include:: ../substitutions.rst

############
UART Library
############

A software defined UART (universal asynchronous receiver transmitter) library allows you to connect to other UART enabled devices in your system. UART is a single wire per direction communications interface allowing either half or full duplex communication. The components in the library are controlled via C and can act as UART transmitter and/or receiver peripheral.

Various options are available including baud rate (individually settable per direction), number of data bits (between 5 and 8), parity (EVEN, ODD or NONE) and number of stop bits (1 or 2). The UART does not support flow control signals. Only a single 1b port per UART direction is needed.

The UART receive supports standard error detection including START, PARITY and FRAMING errors. A callback mechanism is included to notify the user of these conditions. 

The UART may be used in blocking mode, where the call to Tx/Rx does not return until the stop bit is complete or in ISR mode where the UART Rx and/or Tx operates in background mode using a FIFO and callbacks to manage dataflow and error conditions. In ISR (buffered) mode additional callbacks are supported indicating UNDERRUN when the Tx buffer is empty and OVERRUN when the Rx buffer is full.


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
