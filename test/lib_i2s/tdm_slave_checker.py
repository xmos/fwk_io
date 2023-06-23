# Copyright 2023 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
import Pyxsim

class TDMSlaveTX16Checker(Pyxsim.SimThread):

    def __init__(self, sclk, fsync, dout, tx_offset):
        self._sclk = sclk
        self._fsync = fsync
        self._dout = dout
        self._tx_offset = tx_offset

    def run(self):
        xsi = self.xsi
        print("TDM Slave TX 16 Checker Started")

        while True:
            time = xsi.get_time()
            
            frame_count = 0
            bit_count = 0
            word_count = 0
            bits_per_word = 32
            ch_count = 16
            fsync_len = 1

            sclk_frequency = 49152000

            xsi.drive_port_pins(self._sclk, 1)
            xsi.drive_port_pins(self._fsync, 0)

            print(f"CONFIG: bclk:{sclk_frequency}")
            clock_half_period = float(1000000000000) / float(2 * sclk_frequency)

            # Give app time to start, TODO replace with sync signal
            time = time + (clock_half_period * 1000) # arbitary delay
            self.wait_until(time)

            blcks_per_frame = bits_per_word * ch_count

            # Start test
            while 1:
                bits_rx = 0
                bclk_val = 0
                for i in range(0, blcks_per_frame):

                    # bclk
                    xsi.drive_port_pins(self._sclk, 0)

                    # fsync
                    if bits_rx % blcks_per_frame == 0:
                        xsi.drive_port_pins(self._fsync, 1)
                    if bits_rx % blcks_per_frame == fsync_len:
                        xsi.drive_port_pins(self._fsync, 0)

                    # half clock update
                    time = xsi.get_time()
                    time = time + clock_half_period
                    self.wait_until(time)

                    # bclk
                    xsi.drive_port_pins(self._sclk, 1)

                    # fsync (unchanged)
                    if bits_rx % blcks_per_frame == 0:
                        xsi.drive_port_pins(self._fsync, 1)
                    if bits_rx % blcks_per_frame == fsync_len:
                        xsi.drive_port_pins(self._fsync, 0)

                    # full clock update
                    time = xsi.get_time()
                    time = time + clock_half_period
                    self.wait_until(time)

                    # sample
                    bit_val = xsi.sample_port_pins(self._dout)
                    print(f"bit[{bits_rx}]:{bit_val}")

                    bits_rx += 1

