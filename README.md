# IO Framework Repository

This repository is a collection of C/C++ peripheral IO libraries used to develop for xcore:

Supported peripherals include:

- UART
- I2C
- I2S
- SPI
- PDM microphones
- USB

## Build Status

Build Type       |    Status     |
-----------      | --------------|
CI (Linux)       | ![CI](https://github.com/xmos/fwk_io/actions/workflows/ci.yml/badge.svg?branch=develop&event=push) |
Docs             | ![CI](https://github.com/xmos/fwk_io/actions/workflows/docs.yml/badge.svg?branch=develop&event=push) |

## Cloning

Some dependent components are included as git submodules. These can be obtained by cloning this repository with the following command:

    $ git clone --recurse-submodules https://github.com/xmos/fwk_io.git

## Testing

Information on running tests can be found in the tests [README](https://github.com/xmos/fwk_io/blob/develop/test/README.rst).

## Documentation

Information on building the documentation can be found in the docs [README](https://github.com/xmos/fwk_io/blob/develop/doc/README.rst).

## License

This Software is subject to the terms of the [XMOS Public Licence: Version 1](https://github.com/xmos/fwk_io/blob/develop/LICENSE.rst)
