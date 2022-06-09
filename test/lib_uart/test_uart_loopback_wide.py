#!/usr/bin/env python
# Copyright 2022 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

from pathlib import Path
import Pyxsim as px
import pytest

num_args = {        "ONE"   : 1,
                    "TWO"   : 2,
                    "THREE" : 3,
                    "FOUR"  : 4
                    }

speed_args = {
              "57600 baud": 57600,
              "115200 baud": 115200,
              "230400 baud": 230400,
              }

# num_args = {   "ONE" : 1, "TWO" : 2, "THREE" : 3 }
# num_args = {   "ONE" : 1}
# num_args = {   "TWO" : 2}
# num_args = {   "THREE" : 3}
num_args = {   "FOUR" : 4}
# speed_args = {"115200 baud": 115200}
speed_args = {"57600 baud": 57600}
# speed_args = {"115200 baud": 115200}
# speed_args = {"9600 baud": 9600}

@pytest.mark.parametrize("num_args", num_args.values(), ids=num_args.keys())
@pytest.mark.parametrize("baud", speed_args.values(), ids=speed_args.keys())
def test_loopback_wide(request, capfd, num_args, baud):


    # if buffered and baud >= 921600:
    #     pytest.skip(f"Skipping {buffer_key} at {baud} baud")

    cwd = Path(request.fspath).parent
    binary = f"{cwd}/uart_test_loopback_wide/bin/test_hil_uart_loopback_wide_test_{num_args}_{baud}.xe"
    assert Path(binary).exists()

    tester = px.testers.PytestComparisonTester(f'{cwd}/expected/test_uart_loopback_wide_{num_args}.expect',
                                        regexp = False,
                                        ordered = False, #Important because tasks may complete in any order
                                        ignore = ["Exit.*", "Generating"])

    simargs = ["--plugin", "LoopbackPort.dll", "-port tile[0] XS1_PORT_4A 4 0 -port tile[0] XS1_PORT_4B 4 0"]#for speed when not debugging
    simargs_dbg = ["--trace-to", "trace.txt", "--vcd-tracing", "-tile tile[0] -ports -ports-detailed -cores -instructions -o trace.vcd",
                "--plugin", "LoopbackPort.dll", "-port tile[0] XS1_PORT_4A 4 0 -port tile[0] XS1_PORT_4B 4 0"]
    px.run_with_pyxsim(binary, simthreads = [], simargs = simargs_dbg)
    capture = capfd.readouterr().out[:-1] #Tester appends an extra line feed which we don't need

    tester.run(capture)