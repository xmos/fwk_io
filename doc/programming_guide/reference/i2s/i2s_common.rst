
****************
|I2S| Common API
****************

|I2S| Instances
===============

The macro I2S_DATA_WIDTH may be set as a compile flag (e.g. 
-DI2S_DATA_WIDTH=16) to alter the number of bits per word for both the |I2S| 
Master and |I2S| Slave components; this defaults to 32 bits per word. This 
value may be set to any value between 1 and 32. Correct operation of the |I2S| 
components has only currently been verified at 16 and 32 bits per word.

The following structures and functions are used by an |I2S| master or slave instance.

.. doxygengroup:: hil_i2s_core
   :content-only:

TDM Instances
=============

The following structures and functions are used by an TDM master or slave instance.

.. doxygengroup:: hil_i2s_tdm_core
   :content-only: