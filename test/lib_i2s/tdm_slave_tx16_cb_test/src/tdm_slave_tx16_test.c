// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xs1.h>
#include <stdlib.h>
#include <stdio.h>

#include "xcore/port.h"
#include "xcore/clock.h"
#include "xcore/parallel.h"

#include "i2s.h"
#include "i2s_tdm_slave.h"

port_t p_bclk = XS1_PORT_1A;
port_t p_fsync = XS1_PORT_1C;
port_t p_dout = XS1_PORT_1D;

xclock_t bclk = XS1_CLKBLK_1;

#ifndef TEST_FRAME_COUNT
#define TEST_FRAME_COUNT 100
#endif

DECLARE_JOB(burn, (void));

void burn(void) {
    for(;;);
}

I2S_CALLBACK_ATTR
void i2s_send(void *app_data, size_t n, int32_t *send_data)
{
    static int32_t cnt = 0;

    for(size_t i=0; i<n; i++){
        send_data[i] = (i << 28) | cnt;
    }

    cnt++;
    if (cnt == TEST_FRAME_COUNT) {
        _Exit(0);
    }
}

I2S_CALLBACK_ATTR
i2s_restart_t i2s_restart_check(void *app_data)
{
    return I2S_NO_RESTART;
}

int main(void)
{
    i2s_tdm_ctx_t ctx;
    i2s_callback_group_t i_i2s = {
            .init = NULL,
            .restart_check = (i2s_restart_check_t) i2s_restart_check,
            .receive = NULL,
            .send = (i2s_send_t) i2s_send,
            .app_data = NULL,
    };
printf("start\n");
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
printf("init done\n");
    
    PAR_JOBS(
        PJOB(i2s_tdm_slave_tx_16_thread, (&ctx)),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ())
    );

    return 0;
}
