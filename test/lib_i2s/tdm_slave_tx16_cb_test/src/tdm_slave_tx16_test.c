// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xs1.h>
#include <xclib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xcore/port.h"
#include "xcore/clock.h"
#include "xcore/parallel.h"

#include "i2s.h"
#include "i2s_tdm_slave.h"

port_t p_bclk = XS1_PORT_1A;
port_t p_fsync = XS1_PORT_1C;
port_t p_dout = XS1_PORT_1D;

xclock_t bclk = XS1_CLKBLK_1;

port_t setup_strobe_port = XS1_PORT_1E;
port_t setup_data_port = XS1_PORT_16B;
port_t setup_resp_port = XS1_PORT_1F;

#ifndef TEST_FRAME_COUNT
#define TEST_FRAME_COUNT 20
#endif
#ifndef TEST_NUM_CH
#define TEST_NUM_CH 16
#endif

int32_t test_data[TEST_FRAME_COUNT][TEST_NUM_CH] = {{0}};
volatile int32_t cnt = 0;


DECLARE_JOB(burn, (void));

void burn(void) {
    for(;;);
}

static void send_data_to_tester(
        port_t setup_strobe_port,
        port_t setup_data_port,
        unsigned data){
    port_out(setup_data_port, data);
    asm volatile("syncr res[%0]" : : "r" (setup_data_port));
    port_out(setup_strobe_port, 1);
    port_out(setup_strobe_port, 0);
    asm volatile("syncr res[%0]" : : "r" (setup_data_port));
}

static void broadcast_settings(
        port_t setup_strobe_port,
        port_t setup_data_port)
{
    port_out(setup_strobe_port, 0);

    send_data_to_tester(setup_strobe_port, setup_data_port, TX_OFFSET);
}

static uint32_t request_response(
        port_t setup_strobe_port,
        port_t setup_resp_port)
{
    port_enable(setup_resp_port);
    port_out(setup_strobe_port, 1);
    port_out(setup_strobe_port, 0);
    uint32_t tmp = port_in(setup_resp_port);
    return tmp;
}


I2S_CALLBACK_ATTR
void i2s_init(void *app_data, i2s_config_t *i2s_config)
{
    printf("i2s_init\n");
    (void) app_data;
    (void) i2s_config;

    if (cnt > 0) {
        printf("Restart likely due to fsynch error at frame count: %ld\n", cnt);
        _Exit(1);
    }


    /* Initialize test data */
    for (int i=1; i<=TEST_FRAME_COUNT; i++) {
        for (int j=0; j<TEST_NUM_CH; j++) {
            /* bit rev and start with 1 to make it easier to see on the wire */
            test_data[i-1][j] = bitrev((j << 24) | i);
        }
    }
    
    broadcast_settings(setup_strobe_port, setup_data_port);
}

I2S_CALLBACK_ATTR
void i2s_send(void *app_data, size_t n, int32_t *send_data)
{
    memcpy(send_data, test_data[cnt], n * sizeof(int32_t));
}

I2S_CALLBACK_ATTR
i2s_restart_t i2s_restart_check(void *app_data)
{
     cnt++;

     if (cnt == TEST_FRAME_COUNT) {
        port_sync(p_dout); // Wait for the port to empty so we get the whole frame before quitting
        _Exit(1);

     }


    return I2S_NO_RESTART;
}

int main(void)
{
    i2s_tdm_ctx_t ctx;
    i2s_callback_group_t i_i2s = {
            .init = (i2s_init_t) i2s_init,
            .restart_check = (i2s_restart_check_t) i2s_restart_check,
            .receive = NULL,
            .send = (i2s_send_t) i2s_send,
            .app_data = NULL,
    };

    i2s_tdm_slave_tx_16_init(
        &ctx,
        &i_i2s,
        p_dout,
        p_fsync,
        p_bclk,
        bclk,
        TX_OFFSET,
        I2S_SLAVE_SAMPLE_ON_BCLK_RISING,
        NULL);

    port_enable(setup_strobe_port);
    port_enable(setup_data_port);
    port_enable(setup_resp_port);
    
    PAR_JOBS(
        PJOB(i2s_tdm_slave_tx_16_thread, (&ctx)),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ())
    );

    return 0;
}
