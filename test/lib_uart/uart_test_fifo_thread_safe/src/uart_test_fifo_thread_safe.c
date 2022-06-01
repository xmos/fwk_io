// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <xcore/parallel.h>
#include <xcore/assert.h>
#include <xcore/hwtimer.h>

#include "uart_util.h"
#define SETSR(c) asm volatile("setsr %0" : : "n"(c));


const unsigned num_test_bytes = 2000; //How many bytes to push/pop
const size_t buff_size = 17; //Something odd and small enough to trigger lots of over/undeflows
volatile int test_running = 1;


uint32_t random = 0x80085; //Initial seed
void pseudo_rand_uint32(uint32_t *r){
    #define CRC_POLY (0xEB31D82E)
    asm volatile("crc32 %0, %2, %3" : "=r" (*r) : "0" (*r), "r" (-1), "r" (CRC_POLY));
}


unsigned cycle_stats[2][2] = {{0xffffffff, 0x0}, {0xffffffff, 0x0}};
unsigned num_errs[2] = {0};

__attribute__((always_inline))
inline void update_min_max_time(unsigned *cycle_stats_ptr, unsigned this_time){
    cycle_stats_ptr[0] = this_time < cycle_stats_ptr[0] ? this_time : cycle_stats_ptr[0]; 
    cycle_stats_ptr[1] = this_time > cycle_stats_ptr[1] ? this_time : cycle_stats_ptr[1]; 
}

void delay_random(void){
    hwtimer_t tmr = hwtimer_alloc();
    pseudo_rand_uint32(&random);

    uint32_t random_short = random & 0x3ff;
    hwtimer_delay(tmr, random_short);
    hwtimer_free(tmr);
}


DECLARE_JOB(producer, (uart_buffer_t*));
void producer(uart_buffer_t *fifo){
    printf("producer start, %u pushes into FIFO of size: %u\n", num_test_bytes, buff_size);

    for(unsigned i = 0; i< num_test_bytes; i++){
        uint8_t byte = (uint8_t)i;

        uart_buffer_error_t err;
        uint32_t t0, t1;
        do{
            delay_random();
            t0 = get_reference_time();
            err = push_byte_into_buffer(fifo, byte);
            if(err){
                num_errs[0] += 1;
            }
        } while (err != UART_BUFFER_OK);
        t1 = get_reference_time();
        // printf("+");
        update_min_max_time(cycle_stats[0], t1 - t0);
    }
    printf("producer finished\n");
}


DECLARE_JOB(consumer, (uart_buffer_t*));
void consumer(uart_buffer_t *fifo){
    printf("consumer start, %u pops from FIFO of size: %u\n", num_test_bytes, buff_size);

    for(unsigned i = 0; i< num_test_bytes; i++){

        uint8_t dut = 0;
        uint8_t ref = (uint8_t)i;

        uart_buffer_error_t err;
        uint32_t t0, t1;
        do{
            delay_random();
            t0 = get_reference_time();
            err = pop_byte_from_buffer(fifo, &dut);
            if(err){
                num_errs[1] += 1;
            }
        } while (err != UART_BUFFER_OK);
        t1 = get_reference_time();
        // printf("-");
        update_min_max_time(cycle_stats[1], t1 - t0);

        if(dut != ref){
            printf("Error at pop %u, expected: %u got: %u\n", num_test_bytes, ref, dut);
            exit(1);
        }
    }
    printf("consumer finished\n");
    test_running = 0;
}

DECLARE_JOB(burn, (void));
void burn(void) {
    SETSR(XS1_SR_QUEUE_MASK | XS1_SR_FAST_MASK);
    while(test_running);
}

int main(void) {
    uart_buffer_t fifo;
    uint8_t buff_storage[buff_size];
    init_buffer(&fifo, buff_storage, buff_size);

PAR_JOBS(PJOB(producer, (&fifo)),
        PJOB(consumer, (&fifo)),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ())
    );


    printf("Interesting stats - min_push: %u, max_push: %u, num_full_errs: %u\n", cycle_stats[0][0], cycle_stats[0][1], num_errs[0]);
    printf("Interesting stats - min_pop: %u, max_pop: %u, num_empty_errs: %u\n", cycle_stats[1][0], cycle_stats[1][1], num_errs[1]);

    if(num_errs[0] < 10 || num_errs[1] < 10){
        printf("Error: Test didn't full exercise hitting empty / full cases..\n");
        exit(1);
    }

    printf("PASS\n")
    return 0;
}
