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
#ifndef TEST_NUM_UARTS
#define TEST_NUM_UARTS          2
#endif
#define TEST_BAUD               115200
#include "uart_test_common.h"

#define NUMBER_TEST_BYTES  16

uint8_t tx_data[NUMBER_TEST_BYTES] = {0};

port_t p_uart_tx = XS1_PORT_4A;
port_t p_uart_rx = XS1_PORT_4B;

volatile unsigned bytes_received[TEST_NUM_UARTS] = {0};
volatile unsigned task_finished[TEST_NUM_UARTS] = {0};

HIL_UART_RX_CALLBACK_ATTR void rx_error_callback(void *app_data){
    uart_rx_t *uart = (uart_rx_t *)app_data;
    uart_callback_code_t callback_info = uart->cb_code;

    switch(callback_info){
        case UART_START_BIT_ERROR:
            printstrln("UART_START_BIT_ERROR");
            break;
        case UART_PARITY_ERROR:
            printstrln("UART_PARITY_ERROR");
            break;
        case UART_FRAMING_ERROR:
            printstrln("UART_FRAMING_ERROR");
            break;
        case UART_OVERRUN_ERROR:
            printstrln("UART_OVERRUN_ERROR");
            break;
        case UART_UNDERRUN_ERROR:
            printstrln("UART_UNDERRUN_ERROR");
            break;
        default:
            printstr("Unexpected callback code: ");
            printintln(callback_info);
    }
    // exit(-1);
}


DECLARE_JOB(tx_task, (unsigned));
void tx_task(unsigned task_num){

    uart_tx_t uart;
    hwtimer_t tmr = hwtimer_alloc();
    lock_t lock = lock_alloc();
    
    uart_tx_blocking_init(&uart, p_uart_tx, TEST_BAUD, TEST_DATA_BITS, TEST_PARITY, TEST_STOP_BITS, tmr, lock, 0);

    printf("TX UART setting - bit: %d baud: %d bits: %d parity: %d stop: %d\n", task_num, TEST_BAUD, TEST_DATA_BITS, TEST_PARITY, TEST_STOP_BITS);


    for(int i = 0; i < NUMBER_TEST_BYTES; i++){
        uart_tx(&uart, tx_data[i]);
    }

    uart_tx_deinit(&uart);

    hwtimer_free(tmr);
    lock_free(lock);
}


DECLARE_JOB(rx_task, (unsigned));
void rx_task(unsigned task_num){

    uart_rx_t uart;
    hwtimer_t tmr = hwtimer_alloc();
    lock_t lock = lock_alloc();

    uint8_t test_rx[NUMBER_TEST_BYTES] = {0};

    printf("RX UART setting - bit: %d baud: %d bits: %d parity: %d stop: %d\n", task_num, TEST_BAUD, TEST_DATA_BITS, TEST_PARITY, TEST_STOP_BITS);

    uart_rx_blocking_init(  &uart, p_uart_rx, TEST_BAUD, TEST_DATA_BITS, TEST_PARITY, TEST_STOP_BITS, tmr,
                            rx_error_callback, &uart, lock, task_num);

    for(int i = 0; i < NUMBER_TEST_BYTES; i++){
        test_rx[i] = uart_rx(&uart);
    }

    unsigned test_passed = 1;
    for(int i = 0; i < NUMBER_TEST_BYTES; i++){
        if(test_rx[i] != tx_data[i]){
            printf("ERROR - UART: %d expected: 0x%x got: 0x%x\n", task_num, tx_data[i], test_rx[i]);
            test_passed = 0;
        }
    }

    if(test_passed){
        printf("RX UART: %d PASSED\n", task_num);
    }

    uart_rx_deinit(&uart);
    hwtimer_free(tmr);
    lock_free(lock);

    task_finished[task_num] = 1;
}

DECLARE_JOB(burn, (void));
void burn(void) {
    SETSR(XS1_SR_QUEUE_MASK | XS1_SR_FAST_MASK);
    unsigned all_finished = 0;
    while(!all_finished){ //Exit when all finished
        all_finished = 1;
        for(int i = 0; i < TEST_NUM_UARTS; i++){
            if(task_finished[i] = 0){
                all_finished = 0;
            }
        }
    }//while !allfinished
}

uint32_t random = 0x80085; //Initial seed
void pseudo_rand_uint32(uint32_t *r){
    #define CRC_POLY (0xEB31D82E)
    asm volatile("crc32 %0, %2, %3" : "=r" (*r) : "0" (*r), "r" (-1), "r" (CRC_POLY));
}

void make_test_vect(void){
    printf("Generating %d random bytes to transmit\n", NUMBER_TEST_BYTES);
    for(int i = 0; i < NUMBER_TEST_BYTES; i++){
        pseudo_rand_uint32(&random);
        tx_data[i] = (uint8_t)random;
    }
}



int main(void) {
    make_test_vect();
    PAR_JOBS (
        PJOB(rx_task, (0)),
        PJOB(tx_task, (0)),
#if (TEST_NUM_UARTS > 1)
        PJOB(rx_task, (1)),
        PJOB(tx_task, (1)),
#else
        PJOB(burn, ()),
        PJOB(burn, ()),
#endif
#if (TEST_NUM_UARTS > 2)
        PJOB(rx_task, (2)),
        PJOB(tx_task, (2)),
#else
        PJOB(burn, ()),
        PJOB(burn, ()),
#endif
#if (TEST_NUM_UARTS > 3)
        PJOB(rx_task, (3)),
        PJOB(tx_task, (3))
#else
        PJOB(burn, ()),
        PJOB(burn, ())
#endif
    );

    printf("Exit..\n");
    return 0;
}