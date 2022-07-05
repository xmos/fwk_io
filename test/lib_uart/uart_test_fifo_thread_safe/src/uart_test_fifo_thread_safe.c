// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <xcore/parallel.h>
#include <xcore/assert.h>
#include <xcore/hwtimer.h>
#include <xcore/hwtimer.h>
#include <xcore/channel.h>

#include "uart_util.h"
#define SETSR(c) asm volatile("setsr %0" : : "n"(c));


const unsigned num_test_bytes = 4000; //How many bytes to push/pop
const size_t buff_size = 17; //Something odd and small enough to trigger lots of over/undeflows
volatile int test_running = 1;


uint32_t random = 0x80085; //Initial seed
void pseudo_rand_uint32(uint32_t *r){
    #define CRC_POLY (0xEB31D82E)
    asm volatile("crc32 %0, %2, %3" : "=r" (*r) : "0" (*r), "r" (-1), "r" (CRC_POLY));
}


unsigned cycle_stats[2][2] = {{0xffffffff, 0x0}, {0xffffffff, 0x0}};
unsigned num_errs[2] = {0};

//Delay ranges
unsigned p_lower = 0;
unsigned p_upper = 1000;
unsigned c_lower = 0;
unsigned c_upper = 1000;



__attribute__((always_inline))
inline void update_min_max_time(unsigned *cycle_stats_ptr, unsigned this_time){
    cycle_stats_ptr[0] = this_time < cycle_stats_ptr[0] ? this_time : cycle_stats_ptr[0]; 
    cycle_stats_ptr[1] = this_time > cycle_stats_ptr[1] ? this_time : cycle_stats_ptr[1]; 
}

void delay_random(unsigned lower, unsigned upper){
    hwtimer_t tmr = hwtimer_alloc();
    pseudo_rand_uint32(&random);

    uint32_t random_period = lower + (random % (upper - lower));
    hwtimer_delay(tmr, random_period);
    hwtimer_free(tmr);
}

char *scenarios[3] = {"Balanced rate", "Over producing", "Over consuming"};

DECLARE_JOB(producer, (uart_buffer_t*, chanend_t));
void producer(uart_buffer_t *fifo, chanend_t sync){

    for(unsigned scenario = 0; scenario < sizeof(scenarios) / sizeof(scenarios[0]); scenario ++){
        printf("producer start, %u pushes into FIFO of size: %u\n", num_test_bytes, buff_size);

        for(unsigned i = 0; i< num_test_bytes; i++){
            uint8_t byte = (uint8_t)i;

            uart_buffer_error_t err;
            uint32_t t0, t1;
            do{
                delay_random(p_lower, p_upper);
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
        printf("producer finished secanrio: %d %s\n", scenario, scenarios[scenario]);
        printf("Interesting stats - min_push: %u, max_push: %u, num_full_errs: %u\n", cycle_stats[0][0], cycle_stats[0][1], num_errs[0]);


        chan_out_word(sync, 0);
        if (scenario == 0){
            if(num_errs[0] < 10 || num_errs[1] < 10){
                printf("Error: Test didn't full exercise hitting empty / full cases..\n");
                exit(1);
            }
        }
        if (scenario == 1){
            if(num_errs[1] > 0){
                printf("Error: Test hit empty cases..\n");
                exit(1);
            }
        }
        if (scenario == 2){
            if(num_errs[0] > 0){
                printf("Error: Test hit full cases..\n");
                exit(1);
            }
        }

        if(scenario == 0){ //next is over-push
            p_lower = 0;
            p_upper = 500;
            c_lower = 500;
            c_upper = 1000;
        }
        if(scenario == 1){ // next is over consume
            p_lower = 500;
            p_upper = 1000;
            c_lower = 0;
            c_upper = 500;
        }
        chan_out_word(sync, 0);

        cycle_stats[0][0] = 0xffffffff;
        cycle_stats[0][1] = 0;
        num_errs[0] = 0;

        chan_out_word(sync, 0);

    }
}


DECLARE_JOB(consumer, (uart_buffer_t*, chanend_t));
void consumer(uart_buffer_t *fifo, chanend_t sync){

    for(unsigned scenario = 0; scenario < sizeof(scenarios) / sizeof(scenarios[0]); scenario ++){
        printf("consumer start, %u pops from FIFO of size: %u\n", num_test_bytes, buff_size);

        for(unsigned i = 0; i< num_test_bytes; i++){
            uint8_t dut = 0;
            uint8_t ref = (uint8_t)i;

            uart_buffer_error_t err;
            uint32_t t0, t1;
            do{
                delay_random(c_lower, c_upper);
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
        printf("consumer finished secanrio: %d %s\n", scenario, scenarios[scenario]);
        printf("Interesting stats - min_pop: %u, max_pop: %u, num_empty_errs: %u\n", cycle_stats[1][0], cycle_stats[1][1], num_errs[1]);


        chan_in_word(sync);

        //Producer does stats

        chan_in_word(sync);

        cycle_stats[1][0] = 0xffffffff;
        cycle_stats[1][1] = 0;
        num_errs[1] = 0;

        chan_in_word(sync);
        
        printf("\n");
    }
    test_running = 0;
}

DECLARE_JOB(burn, (void));
void burn(void) {
    while(test_running);
}

int main(void) {
    uart_buffer_t fifo;
    uint8_t buff_storage[buff_size];
    init_buffer(&fifo, buff_storage, buff_size);
    channel_t sync = chan_alloc();

PAR_JOBS(PJOB(producer, (&fifo, sync.end_a)),
        PJOB(consumer, (&fifo, sync.end_b)),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ())
    );

    printf("PASS\n");
    return 0;
}
