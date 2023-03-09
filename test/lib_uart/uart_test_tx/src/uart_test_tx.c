// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xs1.h>
#include <stdio.h>
#include <print.h>
#include <string.h>
#include <stdlib.h>
#include <xcore/parallel.h>
#include <xcore/port.h>
#include <xcore/hwtimer.h>
#include <xcore/triggerable.h>
#include <xcore/interrupt.h>
#include <xcore/interrupt_wrappers.h>
#include "uart.h"

#include "uart_test_common.h"

volatile unsigned tx_empty = 0;

HIL_UART_TX_CALLBACK_ATTR void tx_callback(void *app_data){
        tx_empty = 1;
}

port_t p_uart_tx = XS1_PORT_1A;


DEFINE_INTERRUPT_PERMITTED(UART_TX_INTERRUPTABLE_FUNCTIONS, void, test, void){
    uint8_t tx_data[] = {0xff, 0x00, 0x08, 0x55};

    uart_tx_t uart;
    hwtimer_t tmr = hwtimer_alloc();
    // printf("UART setting: %d %d %d %d\n", TEST_BAUD, TEST_DATA_BITS, TEST_PARITY, TEST_STOP_BITS);

#if TEST_BUFFER
    uint8_t buffer[64 + 1] = {0};
    uart_tx_init(&uart, p_uart_tx, TEST_BAUD, TEST_DATA_BITS, TEST_PARITY, TEST_STOP_BITS, tmr, buffer, sizeof(buffer), tx_callback, &uart);
#else
    uart_tx_blocking_init(&uart, p_uart_tx, TEST_BAUD, TEST_DATA_BITS, TEST_PARITY, TEST_STOP_BITS, tmr);
#endif

    for(int i = 0; i < sizeof(tx_data); i++){
        uart_tx(&uart, tx_data[i]);
    }

#if TEST_BUFFER
    while(!tx_empty);
#endif

    uart_tx_deinit(&uart);

    hwtimer_free(tmr);
    exit(0);
}

DECLARE_JOB(burn, (void));

void burn(void) {
    for(;;);
}

int main(void) {
    PAR_JOBS (
        PJOB(INTERRUPT_PERMITTED(test), ()),
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
