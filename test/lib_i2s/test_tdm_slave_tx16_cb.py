# Copyright 2023 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
from tdm_checker import TDMSlaveTX16Checker
from pathlib import Path
import Pyxsim
import pytest

tx_offset_args = {"0": (0),
                  "1": (1),
                  "2": (2)}

@pytest.mark.parametrize(("tx_offset"), tx_offset_args.values(), ids=tx_offset_args.keys())
def test_tdm_slavetx16_cb(capfd, request, nightly, tx_offset):
    testlevel = '0' if nightly else '1'
    id_string = f"{tx_offset}"
    id_string += "_smoke" if testlevel == '1' else ""

    cwd = Path(request.fspath).parent
    binary = f'{cwd}/tdm_slave_tx16_cb_test/bin/test_hil_i2s_tdm_tx16_slave_test_{id_string}.xe'

    checker = TDMSlaveTX16Checker(
        "tile[0]:XS1_PORT_1A",
        "tile[0]:XS1_PORT_1C",
        "tile[0]:XS1_PORT_1D",
        tx_offset)

    tester = Pyxsim.testers.AssertiveComparisonTester(
        f'{cwd}/expected/tdm_slave_tx16_cb_test.expect',
        regexp = True,
        ordered = True,
        suppress_multidrive_messages=True,
        ignore=["CONFIG:.*"]
    )
    # with capfd.disabled():
    Pyxsim.run_on_simulator(
        binary,
        tester=tester,
        simthreads=[checker],
        build_env = {},
        simargs=[],
        capfd=capfd
    )
