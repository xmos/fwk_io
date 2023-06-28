# Copyright 2023 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
import Pyxsim

class TDMSlaveTX16Checker(Pyxsim.SimThread):

    def __init__(
        self,
        sclk,
        fsync,
        dout,
        tx_offset,
        setup_strobe_port,
        setup_data_port,
        setup_resp_port
    ):
        self._sclk = sclk
        self._fsync = fsync
        self._dout = dout
        self._tx_offset = tx_offset
        self._setup_strobe_port = setup_strobe_port
        self._setup_data_port = setup_data_port
        self._setup_resp_port = setup_resp_port

    def get_setup_data(self, 
                       xsi: Pyxsim.pyxsim.Xsi, 
                       setup_strobe_port: str, 
                       setup_data_port: str) -> int:
        self.wait_for_port_pins_change([setup_strobe_port])
        self.wait_for_port_pins_change([setup_strobe_port])
        return xsi.sample_port_pins(setup_data_port)

    def run(self):
        xsi = self.xsi
        print("TDM Slave TX 16 Checker Started")

        while True:            
            frame_count = 0
            bit_count = 0
            word_count = 0
            bits_per_word = 32
            ch_count = 16
            fsync_len = 1

            sclk_frequency = 49152000
            blcks_per_frame = bits_per_word * ch_count

            print(f"CONFIG: bclk:{sclk_frequency}")
            clock_half_period = float(1000000000000) / float(2 * (sclk_frequency/1000)) ## Want freq in khz
            clock_quarter_period = clock_half_period / 2

            #first do the setup rx
            strobe_val = xsi.sample_port_pins(self._setup_strobe_port)
            if strobe_val == 1:
                xsi.drive_port_pins(self._sclk, 1)
                xsi.drive_port_pins(self._fsync, 0)
                self.wait_for_port_pins_change([self._setup_strobe_port])

            tx_offset = self.get_setup_data(xsi, self._setup_strobe_port, self._setup_data_port)
            
            print(f"Got Settings:tx_offset {tx_offset}")

            # drive initial values while slave starts up for the first time
            xsi.drive_port_pins(self._sclk, 1)
            xsi.drive_port_pins(self._fsync, 0)
            self.wait_until(xsi.get_time() + 10000000)

            frame_cnt = 0
            # Start test
            while 1:
                bits_rx = 0
                bclk_val = 0

                # print(f"frame:{frame_cnt}")
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
                    time = time + clock_quarter_period
                    self.wait_until(time)

                    # sample
                    bit_val = xsi.sample_port_pins(self._dout)
                    
                    if frame_cnt >= 2:
                        if tx_offset == 0:
                            frame_arg = frame_cnt-1
                            bit_arg = bits_rx
                        else:
                            frame_arg = (frame_cnt-1) if bits_rx <= tx_offset else (frame_cnt-2)
                            bit_arg = bits_rx + tx_offset if bits_rx >= tx_offset else blcks_per_frame - (tx_offset - bits_rx)

                        expect_rx = self.calc_expected_bit(frame_arg, bit_arg)

                        if bit_val != expect_rx:
                            print(f"bit[{bits_rx}]:{bit_val}:{expect_rx}")
                    
                    bits_rx += 1
                    time = xsi.get_time()
                    time = time + clock_quarter_period
                    self.wait_until(time)

                frame_cnt += 1

    def calc_expected_bit(self, frame, bit):
        # Each sample in the tdm_slave_tx16_test output is in the format
        # bin AAAABBBB BBBBBBBB BBBBBBBB BBBBBBBB
        # Where:
        #   AAAA is the channel id, [0,15]
        #   BBBB BBBBBBBB BBBBBBBB BBBBBBBB is frame num starting at 1
        # Additionally, the output data is bitrev, so that it shows up
        # in a more easily recognizable format on the wire in vcd traces
        ret = 0

        ch = bit // 32
        ch_index = bit % 32

        if ch_index >= 28:
            ret = (ch >> (ch_index - 28)) & 0b1
        else:
            ret = (frame >> ch_index) & 0b1

        return ret
