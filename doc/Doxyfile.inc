# This file provides overrides to the Doxyfile configuration

PROJECT_NAME = XCORE IO Framework
PROJECT_BRIEF = "XCORE IO Framework"

PREDEFINED = __DOXYGEN__=1 
PREDEFINED += DWORD_ALIGNED=
PREDEFINED += __attribute__((weak))=
PREDEFINED += C_API= MA_C_API= C_API_START= C_API_END= EXTERN_C=

# library APIs
INPUT += ../modules/uart/api
INPUT += ../modules/i2c/api
INPUT += ../modules/i2s/api
INPUT += ../modules/qspi_io/api
INPUT += ../modules/spi/api
INPUT += ../modules/mic_array/lib_mic_array/api ../modules/mic_array/lib_mic_array/etc/vanilla

USE_MATHJAX = YES
MATHJAX_FORMAT = HTML-CSS
MATHJAX_RELPATH = https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/

ALIASES += math{1}="@f$\1@f$"
ALIASES += vector{1}="@f$\bar \1@f$"
ALIASES += operation{1}="@par Operation Performed:^^@f{align*}{ \1 @f}"
ALIASES += "beginrst=^^\verbatim embed:rst^^"
ALIASES += "endrst=\endverbatim"
