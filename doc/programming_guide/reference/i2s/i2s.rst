.. include:: ../../../substitutions.rst

#############
|I2S| Library
#############

A software defined library that allows you to control an |I2S| (Inter-IC Sound) bus via xcore ports. |I2S| is a digital data streaming interfaces particularly appropriate for transmission of audio data. TDM is a special case of |I2S| which supports transport of more than two audio channels and is partially included in the library at this time. The components in the library are controlled via C and can either act as |I2S| master, |I2S| slave or TDM slave.

.. note::

    TDM is only currently supported as a TDM16 slave Tx component. Expansion of this library to support master or slave Rx is possible and can be done on request.

|I2S| is a protocol between two devices where one is the *master* and one is the *slave* which determines who drives the clock lines. The protocol is made up of four signals shown in :ref:`i2s_wire_table`.

.. _i2s_wire_table:

.. list-table:: |I2S| data wires
     :class: vertical-borders horizontal-borders

     * - *MCLK*
       - Clock line, driven by external oscillator. This signal is optional.
     * - *BCLK*
       - Bit clock. This is a fixed divide of the *MCLK* and is driven
         by the master.
     * - *LRCLK* (or *WCLK*)
       - Word clock (or word select). This is driven by the master.
     * - *DATA*
       - Data line, driven by one of the slave or master depending on
         the data direction. There may be several data lines in
         differing directions.

All |I2S| functions can be accessed via the ``i2s.h`` header:

.. code-block:: c
   
   #include "i2s.h"

TDM is a protocol between two devices similar to |I2S| where one is the *master* and one is the *slave* which determines who drives the clock lines. The protocol is made up of four signals shown in :ref:`tdm_wire_table`.

.. _tdm_wire_table:

.. list-table:: TDM data wires
     :class: vertical-borders horizontal-borders

     * - *MCLK*
       - Clock line, driven by external oscillator. This signal is optional.
     * - *BCLK*
       - Bit clock. This is a fixed divide of the *MCLK* and is driven
         by the master.
     * - *FSYCNH*
       - Frame synchronization. Toggles at the start of the TDM data frame. This is driven by the master.
     * - *DATA*
       - Data line, driven by one of the slave or master depending on
         the data direction. There may be several data lines in
         differing directions.

Currently supported TDM functions can be accessed via the ``i2s_tdm_slave.h`` header:

.. code-block:: c
   
   #include "i2s_tdm_slave.h"

.. toctree::
   :maxdepth: 2
   :includehidden:

   i2s_common.rst
   i2s_master.rst
   i2s_slave.rst
   i2s_tdm_slave.rst
