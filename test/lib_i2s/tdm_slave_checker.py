# Copyright 2023 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
import Pyxsim

class TDMSlaveTX16Checker(Pyxsim.SimThread):

    sample_on_falling = 0
    sample_on_rising = 1

    def __init__(
        self,
        sclk,
        fsync,
        dout,
        setup_strobe_port,
        setup_data_port,
        setup_resp_port,
        sample_edge,
        sclk_frequency
    ):
        self._sclk = sclk
        self._fsync = fsync
        self._dout = dout
        self._setup_strobe_port = setup_strobe_port
        self._setup_data_port = setup_data_port
        self._setup_resp_port = setup_resp_port
        self._sample_edge = sample_edge
        self._sclk_frequency = sclk_frequency

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

            blcks_per_frame = bits_per_word * ch_count

            edge_str = "FALLING" if self._sample_edge==self.sample_on_falling else "RISING"
            print(f"CONFIG: bclk:{self._sclk_frequency} sample_edge: {edge_str} fsynch_len: {fsync_len}")
            clock_half_period = float(1000000000) / float(2 * (self._sclk_frequency/1000)) ## Want freq in Hz
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
            xsi.drive_port_pins(self._sclk, 1 if self._sample_edge == self.sample_on_rising else 0)
            xsi.drive_port_pins(self._fsync, 0)
            self.wait_until(xsi.get_time() + 1000000)

            frame_cnt = 0
            # Start test
            while 1:
                bits_rx = 0
                bclk_val = 0


                # print(f"frame:{frame_cnt}")
                for i in range(0, blcks_per_frame):
                    if i % bits_per_word == 0:
                        word_rx = 0

                    # bclk
                    xsi.drive_port_pins(self._sclk, 0 if self._sample_edge == self.sample_on_rising else 1)

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
                    xsi.drive_port_pins(self._sclk, 1 if self._sample_edge == self.sample_on_rising else 0)

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
                    word_rx |= bit_val << ((i - tx_offset) % bits_per_word)
                    
                    if frame_cnt >= 2: # Ignore init frame as data is 0's while slave syncs up
                        frame_arg = 0
                        bit_arg = 0
                        if tx_offset == 0:
                            frame_arg = frame_cnt-1
                            bit_arg = bits_rx
                        else:
                            if bits_rx < tx_offset:
                                frame_arg = (frame_cnt-2) 
                            else:
                                frame_arg = (frame_cnt-1)

                            if frame_arg == (frame_cnt-1):
                                bit_arg = bits_rx - tx_offset 
                            else:
                                bit_arg = (blcks_per_frame - 1) + (bits_rx - tx_offset)

                        expect_rx = self.calc_expected_bit(frame_arg, bit_arg)
                        # print(f"asked for {frame_arg}:{bit_arg} check bit[{bits_rx}]:{bit_val}:{expect_rx}")

                        # For the first frame, if tx_offset > 0, the first tx_offset bits are don't cares
                        if frame_cnt == 2 and bits_rx > tx_offset:
                            if bit_val != expect_rx:
                                print(f"ERROR: bit[{bits_rx}]:{bit_val}:{expect_rx}")
                    
                    bits_rx += 1
                    time = xsi.get_time()
                    time = time + clock_quarter_period
                    self.wait_until(time)

                    if i % bits_per_word == bits_per_word - 1:
                        print(f"Received word: {frame_cnt} {i // bits_per_word} {hex(word_rx)}")

                frame_cnt += 1

    def calc_expected_bit(self, frame, bit):
        # Each sample in the tdm_slave_tx16_test output is in the format
        # bin AAAAAAAA BBBBBBBB BBBBBBBB BBBBBBBB
        # Where:
        #   AAAAAAAA is the channel id, [0,255]
        #   BBBBBBBB BBBBBBBB BBBBBBBB is frame num starting at 1
        # Additionally, the output data is bitrev, so that it shows up
        # in a more easily recognizable format on the wire in vcd traces
        ret = 0

        ch = bit // 32
        ch_index = bit % 32

        if ch_index >= 24:
            ret = (ch >> (ch_index - 24)) & 0b1
        else:
            ret = (frame >> ch_index) & 0b1

        return ret
