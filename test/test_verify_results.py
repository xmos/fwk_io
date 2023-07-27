#!/usr/bin/env python
# Copyright 2021-2023 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
import sys
import pytest
import xml.etree.ElementTree as ET


@pytest.fixture(scope="session")
def lib_name(pytestconfig):
    lib_name = pytestconfig.getoption("lib_name").split()
    return lib_name

def test_results(lib_name):
    lib = lib_name[0]
    result_fname = f"test_results_{lib}.xml"
    tree = ET.parse(result_fname)
    root = tree.getroot()
    tsuite = root.find("testsuite")

    acceptable_outcomes = ("skipped",)
    

    # There should at least be some tests in here. Assert that this is the case.
    assert len(tsuite) > 0

    for testcase in tsuite.iter("testcase"):

        # Test passed if there are no children
        if len(testcase) == 0:
            if "name" in testcase.attrib:
                print(testcase.attrib['name'], "passed")
            continue
        # Otherwise, test was either skipped, errored, or failed. The testcase
        # will have a child with a relevant tag - skipped, error, or failure.
        # If the tag is acceptable then carry on, or otherwise assert failure.
        else:
            for child in testcase:
                assert (
                    child.tag in acceptable_outcomes
                ), f"A test reports as {child.tag}, which is not accepted."
            print(testcase.attrib['name'], child.tag )
