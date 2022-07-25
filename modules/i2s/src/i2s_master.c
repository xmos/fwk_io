// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xclib.h>
#include <xcore/port.h>
#include <xcore/clock.h>
#include <xcore/assert.h>

#include "i2s.h"

static void i2s_setup_bclk(
        xclock_t bclk,
        /*in*/port_t p_mclk,
        unsigned mclk_bclk_ratio)
{
    clock_enable(bclk);

    clock_set_source_port(bclk, p_mclk);
    clock_set_divide(bclk, mclk_bclk_ratio >> 1);
}

static void i2s_init_ports(
        const /*out buffered*/port_t /*:32*/p_dout[],
        const size_t num_out_ports,
        const /*in buffered*/port_t /*:32*/p_din[],
        const size_t num_in_ports,
        /*out*/port_t p_bclk,
        /*out buffered*/port_t /*:32*/p_lrclk,
        xclock_t bclk
        )
{
    size_t i;

    port_reset(p_bclk);
    port_set_clock(p_bclk, bclk);
    port_set_out_clock(p_bclk);

    port_start_buffered(p_lrclk, 32);
    port_set_clock(p_lrclk, bclk);
    port_out(p_lrclk, 1);

    for (i = 0; i < num_out_ports; i++) {
        port_start_buffered(p_dout[i], 32);
        port_set_clock(p_dout[i], bclk);
        port_out(p_dout[i], 0);
    }

    for (i = 0; i < num_in_ports; i++) {
        port_start_buffered(p_din[i], 32);
        port_set_clock(p_din[i], bclk);
    }
}

static void i2s_deinit_ports(
        const /*out buffered*/port_t /*:32*/p_dout[],
        const size_t num_out,
        const /*in buffered*/port_t /*:32*/p_din[],
        const size_t num_in,
        /*out*/port_t p_bclk,
        /*out buffered*/port_t /*:32*/p_lrclk
        )
{
    size_t i;

    port_disable(p_bclk);
    port_disable(p_lrclk);

    for (i = 0; i < num_out; i++) {
        port_disable(p_dout[i]);
    }

    for (i = 0; i < num_in; i++) {
        port_disable(p_din[i]);
    }
}

static i2s_restart_t i2s_ratio_n_1b(
        const i2s_callback_group_t *const i2s_cbg,
        const port_t p_dout[],
        const size_t num_out,
        const port_t p_din[],
        const size_t num_in,
        const port_t p_bclk,
        const xclock_t bclk,
        const port_t p_lrclk,
        const i2s_mode_t mode)
{
    size_t i;
    size_t idx;
    int offset;

    /* two samples per data line, left and right */
    int32_t in_samps[I2S_CHANS_PER_FRAME * I2S_MAX_DATALINES];
    int32_t out_samps[I2S_CHANS_PER_FRAME * I2S_MAX_DATALINES];

    xassert(num_in <= I2S_MAX_DATALINES);
    xassert(num_out <= I2S_MAX_DATALINES);

    unsigned lr_mask = 0;

    for (i = 0; i < num_out; i++) {
        port_clear_buffer(p_dout[i]);
    }
    for (i = 0; i < num_in; i++) {
        port_clear_buffer(p_din[i]);
    }
    port_clear_buffer(p_lrclk);

    if (num_out > 0) {
        i2s_cbg->send(i2s_cbg->app_data, num_out << 1, out_samps);
    }

    //Start outputting evens (0,2,4..) data at correct point relative to the clock
    if (mode == I2S_MODE_I2S) {
        offset = 1;
    } else {
        offset = 0;
    }

//#pragma unroll(I2S_MAX_DATALINES)
    for (i = 0, idx = 0; i < num_out; i++, idx += I2S_CHANS_PER_FRAME) {
        port_set_trigger_time(p_dout[i], 1 + offset);
        port_out(p_dout[i], bitrev(out_samps[idx]));
    }

    port_set_trigger_time(p_lrclk, 1);
    port_out(p_lrclk, lr_mask);

    clock_start(bclk);

    //And pre-load the odds (1,3,5..)
//#pragma unroll(I2S_MAX_DATALINES)
    for (i = 0, idx = 1; i < num_out; i++, idx += I2S_CHANS_PER_FRAME) {
        port_out(p_dout[i], bitrev(out_samps[idx]));
    }

    lr_mask = ~lr_mask;
    port_out(p_lrclk, lr_mask);

    for (i = 0; i < num_in; i++) {
        port_set_trigger_time(p_din[i], 32 + offset);
    }

    for (;;) {
        // Check for restart
        i2s_restart_t restart = i2s_cbg->restart_check(i2s_cbg->app_data);

        if (restart == I2S_NO_RESTART) {
            if (num_out > 0) {
                i2s_cbg->send(i2s_cbg->app_data, num_out << 1, out_samps);
            }

            //Output i2s evens (0,2,4..)
//#pragma unroll(I2S_MAX_DATALINES)
            for (i = 0, idx = 0; i < num_out; i++, idx += I2S_CHANS_PER_FRAME) {
                port_out(p_dout[i], bitrev(out_samps[idx]));
            }
        }

        //Input i2s evens (0,2,4..)
//#pragma unroll(I2S_MAX_DATALINES)
        for (i = 0, idx = 0; i < num_in; i++, idx += I2S_CHANS_PER_FRAME) {
            int32_t data;
            data = port_in(p_din[i]);
            in_samps[idx] = bitrev(data);
        }

        lr_mask = ~lr_mask;
        port_out(p_lrclk, lr_mask);

        if (restart == I2S_NO_RESTART) {
            //Output i2s odds (1,3,5..)
//#pragma unroll(I2S_MAX_DATALINES)
            for (i = 0, idx = 1; i < num_out; i++, idx += I2S_CHANS_PER_FRAME) {
                port_out(p_dout[i], bitrev(out_samps[idx]));
            }

            lr_mask = ~lr_mask;
            port_out(p_lrclk, lr_mask);
        }

        //Input i2s odds (1,3,5..)
//#pragma unroll(I2S_MAX_DATALINES)
        for (i = 0, idx = 1; i < num_in; i++, idx += I2S_CHANS_PER_FRAME) {
            int32_t data;
            data = port_in(p_din[i]);
            in_samps[idx] = bitrev(data);
        }

        if (num_in > 0) {
            i2s_cbg->receive(i2s_cbg->app_data, num_in << 1, in_samps);
        }

        if (restart != I2S_NO_RESTART) {
            if (num_in == 0) {
                // Prevent the clock from being stopped before the last word
                // has been sent if there are no RX ports.
                asm volatile("syncr res[%0]" : : "r" (p_dout[0]));
            }
            clock_stop(bclk);
            return restart;
        }
    }
    return I2S_RESTART;
}


static i2s_restart_t i2s_ratio_n_4b(
        const i2s_callback_group_t *const i2s_cbg,
        const port_t p_dout[],
        const size_t num_out,
        const port_t p_din[],
        const size_t num_in,
        const port_t p_bclk,
        const xclock_t bclk,
        const port_t p_lrclk,
        const i2s_mode_t mode)
{
    /*
     * Implementation currently only works for
     * a single 4b port; we've already asserted that this is the case.
     * If it stops being the case, go back to using p_d[in/out] everywhere.
     */
    port_t port_din = p_din ? p_din[0] : 0;
    port_t port_dout = p_dout ? p_dout[0] : 0;

    const int offset = (mode == I2S_MODE_I2S) ? 1 : 0;

    /* 
     * two channels per data line, left and right 
     * This should be (num_[in/out] << 1), but that's not const.
     */
    int32_t in_samps[I2S_CHANS_PER_FRAME * I2S_MAX_DATALINES];
    int32_t out_samps[I2S_CHANS_PER_FRAME * I2S_MAX_DATALINES];

    xassert(num_in <= I2S_MAX_DATALINES);
    xassert(num_out <= I2S_MAX_DATALINES);

    port_clear_buffer(port_din);
    port_clear_buffer(port_dout);
    port_clear_buffer(p_lrclk);

    if (num_out)
    {
        i2s_cbg->send(i2s_cbg->app_data, num_out << 1, out_samps);
    }

    port_set_trigger_time(p_lrclk, 1);
    port_out(p_lrclk, 0);

    if (port_din)
    {
        asm volatile ("setpt res[%0], %1"
                     : 
                     :"r"(port_din), "r"(8 + offset));
    }
    if (port_dout)
    {
        asm volatile ("setpt res[%0], %1"
                     : 
                     :"r"(port_dout), "r"(1 + offset));
    }
    i2s_master_4b_setup(out_samps, in_samps, port_dout, port_din, bclk, p_lrclk);

    while (1)
    {
        // Check for restart
        i2s_restart_t restart = i2s_cbg->restart_check(i2s_cbg->app_data);

        if (num_out)
        {
            i2s_cbg->send(i2s_cbg->app_data, num_out << 1, out_samps);
        }
        i2s_master_4b_loop_part_1(out_samps, in_samps, port_dout, port_din, p_lrclk);

        if (num_in)
        {
            i2s_i.receive(num_in << 1, in_samps);
        }

        if (restart == I2S_NO_RESTART)
        {
            i2s_master_4b_loop_part_2(out_samps, in_samps, port_dout, port_din, p_lrclk);
        }
        else
        {
            if (!num_in)
            {
                /*
                 * Prevent the clock from being stopped before the last word
                 * has been sent if there are no RX ports
                 */
                port_sync(port_dout);
            }
            clock_stop(bclk);
            return restart;
        }
    }
    return I2S_RESTART;
}

static void i2s_master_1b(
        const i2s_callback_group_t *const i2s_cbg,
        const port_t p_dout[],
        const size_t num_out_ports,
        const size_t num_out,
        const port_t p_din[],
        const size_t num_in_ports,
        const size_t num_in,
        const size_t num_data_bits,
        const port_t p_bclk,
        const port_t p_lrclk,
        const port_t p_mclk,
        const xclock_t bclk)
{
    for (;;) {
        i2s_config_t config;
        i2s_cbg->init(i2s_cbg->app_data, &config);

        i2s_setup_bclk(bclk, p_mclk, config.mclk_bclk_ratio);

        //This ensures that the port time on all the ports is at 0
        i2s_init_ports(p_dout, num_out_ports, p_din, num_in_ports, p_bclk, p_lrclk, bclk);

        i2s_restart_t restart = i2s_ratio_n_1b(i2s_cbg, p_dout, num_out, p_din,
                                             num_in,
                                             p_bclk, bclk, p_lrclk,
                                             config.mode);

        if (restart == I2S_SHUTDOWN) {
            i2s_deinit_ports(p_dout, num_out_ports, p_din, num_in_ports, p_bclk, p_lrclk);
            clock_disable(bclk);
            return;
        }
    }
}

static void i2s_master_4b(
        const i2s_callback_group_t *const i2s_cbg,
        const port_t p_dout[],
        const size_t num_out_ports,
        const size_t num_out,
        const port_t p_din[],
        const size_t num_in_ports,
        const size_t num_in,
        const size_t num_data_bits,
        const port_t p_bclk,
        const port_t p_lrclk,
        const port_t p_mclk,
        const xclock_t bclk)
{
    for (;;) {
        i2s_config_t config;
        i2s_cbg->init(i2s_cbg->app_data, &config);

        i2s_setup_bclk(bclk, p_mclk, config.mclk_bclk_ratio);

        //This ensures that the port time on all the ports is at 0
        i2s_init_ports(p_dout, num_out_ports, p_din, num_in_ports, p_bclk, p_lrclk, bclk);

        i2s_restart_t restart = i2s_ratio_n_4b(i2s_cbg, p_dout, num_out, p_din,
                                             num_in,
                                             p_bclk, bclk, p_lrclk,
                                             config.mode);

        if (restart == I2S_SHUTDOWN) {
            i2s_deinit_ports(p_dout, num_out_ports, p_din, num_in_ports, p_bclk, p_lrclk);
            clock_disable(bclk);
            return;
        }
    }
}

static void i2s_master_external_clock_1b(
        const i2s_callback_group_t *const i2s_cbg,
        const port_t p_dout[],
        const size_t num_out_ports,
        const size_t num_out,
        const port_t p_din[],
        const size_t num_in_ports,
        const size_t num_in,
        const size_t num_data_bits,
        const port_t p_bclk,
        const port_t p_lrclk,
        const xclock_t bclk)
{
    while (1) {
        i2s_config_t config;
        i2s_cbg->init(i2s_cbg->app_data, &config);

        //This ensures that the port time on all the ports is at 0
        i2s_init_ports(p_dout, num_out, p_din, num_in, p_bclk, p_lrclk, bclk);

        i2s_restart_t restart = i2s_ratio_n(i2s_cbg, p_dout, num_out, p_din,
                                            num_in,
                                            p_bclk, bclk, p_lrclk,
                                            config.mode);

        if (restart == I2S_SHUTDOWN) {
            i2s_deinit_ports(p_dout, num_out, p_din, num_in, p_bclk, p_lrclk);
            return;
        }
    }
}

static void i2s_master_external_clock_4b(
        const i2s_callback_group_t *const i2s_cbg,
        const port_t p_dout[],
        const size_t num_out_ports,
        const size_t num_out,
        const port_t p_din[],
        const size_t num_in_ports,
        const size_t num_in,
        const size_t num_data_bits,
        const port_t p_bclk,
        const port_t p_lrclk,
        const xclock_t bclk)
{
    while (1) {
        i2s_config_t config;
        i2s_cbg->init(i2s_cbg->app_data, &config);

        //This ensures that the port time on all the ports is at 0
        i2s_init_ports(p_dout, num_out, p_din, num_in, p_bclk, p_lrclk, bclk);

        i2s_restart_t restart = i2s_ratio_n(i2s_cbg, p_dout, num_out, p_din,
                                            num_in,
                                            p_bclk, bclk, p_lrclk,
                                            config.mode);

        if (restart == I2S_SHUTDOWN) {
            i2s_deinit_ports(p_dout, num_out, p_din, num_in, p_bclk, p_lrclk);
            return;
        }
    }
}

void i2s_master(
        const i2s_callback_group_t *const i2s_cbg,
        const size_t io_port_size,
        const size_t num_data_bits,
        const port_t p_dout[],
        const size_t num_out_ports,
        const size_t num_out,
        const port_t p_din[],
        const size_t num_in_ports,
        const size_t num_in,
        const port_t p_bclk,
        const port_t p_lrclk,
        const port_t p_mclk,
        const xclock_t bclk)
{
    xassert(num_data_bits == 32);
    xassert(io_port_size == 4 || io_port_size == 1);
    xassert(num_out || num_in);
    xassert(p_dout || p_din);
    xassert((io_port_size * num_out_ports) >= num_out);
    xassert((io_port_size * num_in_ports) >= num_in);

    if (io_port_size == 4)
    {
        xassert(num_in_ports <= 1);
        xassert(num_out_ports <= 1);
        i2s_master_4b(i2s_cbg,
                        p_dout,
                        num_out_ports,
                        num_out,
                        p_din,
                        num_in_ports,
                        num_in,
                        num_data_bits,
                        p_bclk,
                        p_lrclk,
                        p_mclk,
                        bclk);
    }
    else if (io_port_size == 1)
    {
        i2s_master_1b(i2s_cbg,
                        p_dout,
                        num_out_ports,
                        num_out,
                        p_din,
                        num_in_ports,
                        num_in,
                        num_data_bits,
                        p_bclk,
                        p_lrclk,
                        p_mclk,
                        bclk);
    }
}

void i2s_master_external_clock(
        const i2s_callback_group_t *const i2s_cbg,
        const size_t io_port_size,
        const size_t num_data_bits,
        const port_t p_dout[],
        const size_t num_out_ports,
        const size_t num_out,
        const port_t p_din[],
        const size_t num_in_ports,
        const size_t num_in,
        const port_t p_bclk,
        const port_t p_lrclk,
        const xclock_t bclk)
{
    xassert(num_data_bits == 32);
    xassert(io_port_size == 4 || io_port_size == 1);
    xassert(num_out || num_in);
    xassert(p_dout || p_din);
    xassert((io_port_size * num_out_ports) >= num_out);
    xassert((io_port_size * num_in_ports) >= num_in);

    if (io_port_size == 4)
    {
        i2s_master_external_clock_4b(i2s_cbg,
                                        p_dout,
                                        num_out_ports,
                                        num_out,
                                        p_din,
                                        num_in_ports,
                                        num_in,
                                        num_data_bits,
                                        p_bclk,
                                        p_lrclk,
                                        bclk);
    }
    else if (io_port_size == 1)
    {
        i2s_master_external_clock_1b(i2s_cbg,
                                        p_dout,
                                        num_out_ports,
                                        num_out,
                                        p_din,
                                        num_in_ports,
                                        num_in,
                                        num_data_bits,
                                        p_bclk,
                                        p_lrclk,
                                        bclk);
    }
}
