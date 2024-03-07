// Copyright 2016-2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <platform.h>
#include <xs1.h>
#include <i2s.h>
#include <stdlib.h>
#include <stdio.h>
#include <xcore/parallel.h>

#ifndef NUM_I2S_LINES
#define NUM_I2S_LINES   2
#endif
#ifndef BURN_THREADS
#define BURN_THREADS    6
#endif
#ifndef SAMPLE_FREQUENCY
#define SAMPLE_FREQUENCY 192000
#endif
#ifndef TEST_LEN
#define TEST_LEN 1000
#endif
#ifndef RECEIVE_DELAY_INCREMENT
#define RECEIVE_DELAY_INCREMENT 5
#endif
#ifndef SEND_DELAY_INCREMENT
#define SEND_DELAY_INCREMENT 5
#endif
#ifndef DATA_BITS
#define DATA_BITS 32
#endif

// Applications are expected to define this macro if they want non-32b I2S width
#define I2S_DATA_BITS DATA_BITS

#ifndef GENERATE_MCLK
#define GENERATE_MCLK 0
#endif

#if GENERATE_MCLK
#define MASTER_CLOCK_FREQUENCY 25000000
#else
#define MASTER_CLOCK_FREQUENCY 24576000
#endif

#define SETSR(c) asm volatile("setsr %0" : : "n"(c));

enum {
    SAMPLE_RATE_192000,
    SAMPLE_RATE_384000,
    NUM_SAMPLE_RATES
}e_sample_rates;

enum {
    BITDEPTH_16,
    BITDEPTH_32,
    NUM_BIT_DEPTHS
}e_bit_depth;

enum {
    NUM_I2S_LINES_1,
    NUM_I2S_LINES_2,
    NUM_I2S_LINES_3,
    NUM_I2S_LINES_4,
}e_channel_config;

static int acceptable_receive_delay = 0, acceptable_send_delay = 0;
static int acceptable_delay_ticks[NUM_SAMPLE_RATES][NUM_I2S_LINES_4+1][NUM_BIT_DEPTHS];

static inline void populate_acceptable_delay_ticks()
{
    // These numbers are logged by running the test and logging the delay at the last passing iteration
    // before the backpressure test starts failing. So, on top of the bare minimum code in the i2s_send()
    // and i2s_receive() functions in this file plus their calling overheads, we have acceptable_delay_ticks number
    // of cycles per send and receive callback function call to add any extra processing.

    // 192 KHz
    acceptable_delay_ticks[SAMPLE_RATE_192000][NUM_I2S_LINES_1][BITDEPTH_16] = 170;
    acceptable_delay_ticks[SAMPLE_RATE_192000][NUM_I2S_LINES_1][BITDEPTH_32] = 185;
    acceptable_delay_ticks[SAMPLE_RATE_192000][NUM_I2S_LINES_2][BITDEPTH_16] = 140;
    acceptable_delay_ticks[SAMPLE_RATE_192000][NUM_I2S_LINES_2][BITDEPTH_32] = 155;
    acceptable_delay_ticks[SAMPLE_RATE_192000][NUM_I2S_LINES_3][BITDEPTH_16] = 105;
    acceptable_delay_ticks[SAMPLE_RATE_192000][NUM_I2S_LINES_3][BITDEPTH_32] = 125;
    acceptable_delay_ticks[SAMPLE_RATE_192000][NUM_I2S_LINES_4][BITDEPTH_16] = 70;
    acceptable_delay_ticks[SAMPLE_RATE_192000][NUM_I2S_LINES_4][BITDEPTH_32] = 90;

    // 384 KHz
    acceptable_delay_ticks[SAMPLE_RATE_384000][NUM_I2S_LINES_1][BITDEPTH_16] = 50;
    acceptable_delay_ticks[SAMPLE_RATE_384000][NUM_I2S_LINES_1][BITDEPTH_32] = 55;
    acceptable_delay_ticks[SAMPLE_RATE_384000][NUM_I2S_LINES_2][BITDEPTH_16] = 15;
    acceptable_delay_ticks[SAMPLE_RATE_384000][NUM_I2S_LINES_2][BITDEPTH_32] = 25;
    // For 384 KHz, we have non-zero backpressure only up to 2 channels
    acceptable_delay_ticks[SAMPLE_RATE_384000][NUM_I2S_LINES_3][BITDEPTH_16] = 0;
    acceptable_delay_ticks[SAMPLE_RATE_384000][NUM_I2S_LINES_3][BITDEPTH_32] = 0;
    acceptable_delay_ticks[SAMPLE_RATE_384000][NUM_I2S_LINES_4][BITDEPTH_16] = 0;
    acceptable_delay_ticks[SAMPLE_RATE_384000][NUM_I2S_LINES_4][BITDEPTH_32] = 0;
}

void get_acceptable_delay()
{
    int sample_rate;
    if(SAMPLE_FREQUENCY == 192000)
    {
        sample_rate = SAMPLE_RATE_192000;
    }
    else if(SAMPLE_FREQUENCY == 384000)
    {
        sample_rate = SAMPLE_RATE_384000;
    }
    else
    {
        printf("ERROR: Invalid sample rate %d\n", SAMPLE_FREQUENCY);
        _Exit(1);
    }

    int bit_depth;
    if(DATA_BITS == 16)
    {
        bit_depth = BITDEPTH_16;
    }
    else if(DATA_BITS == 32)
    {
        bit_depth = BITDEPTH_32;
    }
    else
    {
        printf("ERROR: Invalid bit_depth %d\n", DATA_BITS);
        _Exit(1);
    }
    if((NUM_I2S_LINES < 1) || (NUM_I2S_LINES > 4))
    {
        printf("ERROR: Invalid NUM_I2S_LINES %d\n", NUM_I2S_LINES);
        _Exit(1);
    }
    int delay = acceptable_delay_ticks[sample_rate][NUM_I2S_LINES-1][bit_depth];

    if(delay <= 0)
    {
        printf("ERROR: Invalid delay %d. Check if testing an unsupported configuration\n", delay);
        _Exit(1);
    }

    // get the send and receive delay based on the
    if((RECEIVE_DELAY_INCREMENT == 5) && (SEND_DELAY_INCREMENT == 5))
    {
        // Backpressure passes at delay, so add another increment number of ticks to get to the first fail instance
        acceptable_receive_delay = delay + 5;
        acceptable_send_delay = delay + 5;
    }
    else if((RECEIVE_DELAY_INCREMENT == 0) && (SEND_DELAY_INCREMENT == 10))
    {
        // Backpressure passes at 2*delay, so add another increment number of ticks to get to the first fail instance
        acceptable_receive_delay = 0;
        acceptable_send_delay = 2*delay + 10;
    }
    else if((RECEIVE_DELAY_INCREMENT == 10) && (SEND_DELAY_INCREMENT == 0))
    {
        // Backpressure passes at 2*delay, so add another increment number of ticks to get to the first fail instance
        acceptable_receive_delay = 2*delay + 10;
        acceptable_send_delay = 0;
    }
    else
    {
        printf("ERROR: Unsupported receive (%d) and send (%d) delay increment combination\n", RECEIVE_DELAY_INCREMENT, SEND_DELAY_INCREMENT);
        _Exit(1);
    }
}

/* Ports and clocks used by the application */
port_t p_lrclk = XS1_PORT_1G;
port_t p_bclk = XS1_PORT_1H;
port_t p_mclk = XS1_PORT_1F;
port_t p_dout[4] = {XS1_PORT_1M, XS1_PORT_1N, XS1_PORT_1O, XS1_PORT_1P};
port_t p_din [4] = {XS1_PORT_1I, XS1_PORT_1J, XS1_PORT_1K, XS1_PORT_1L};

xclock_t mclk = XS1_CLKBLK_1;
xclock_t bclk = XS1_CLKBLK_2;

static volatile int receive_delay = 0;
static volatile int send_delay = 0;
static volatile int32_t receive_data_store[8];

void i2s_init(void *app_data, i2s_config_t *i2s_config)
{
    i2s_config->mode = I2S_MODE_I2S;
    i2s_config->mclk_bclk_ratio = MASTER_CLOCK_FREQUENCY/(SAMPLE_FREQUENCY*2*DATA_BITS);
}

void i2s_send(void *app_data, size_t n, int32_t *send_data)
{
    for (size_t c = 0; c < n; c++) {
        send_data[c] = c;
    }
    if (send_delay) {
        delay_ticks(send_delay);
    }
}

void i2s_receive(void *app_data, size_t n, int32_t *receive_data)
{
    for (size_t c = 0; c < n; c++) {
        receive_data_store[c] = receive_data[c];
    }
    if (receive_delay) {
        delay_ticks(receive_delay);
    }
}

i2s_restart_t i2s_restart_check(void *app_data)
{
    return I2S_NO_RESTART;
}

#define JITTER  1   //Allow for rounding so does not break when diff = period + 1
#define N_CYCLES_AT_DELAY   1 //How many LR clock cycles to measure at each backpressure delay value
#define DIFF_WRAP_16(new, old)  (new > old ? new - old : new + 0x10000 - old)
port_t p_lr_test = XS1_PORT_1A;
DECLARE_JOB(test_lr_period, (void));
void test_lr_period() {
    const int ref_tick_per_sample = XS1_TIMER_HZ/SAMPLE_FREQUENCY;
    const int period = ref_tick_per_sample;

    port_enable(p_lr_test);
    int time;

    // Synchronise with LR clock
    port_set_trigger_in_equal(p_lr_test, 1);
    (void) port_in(p_lr_test);
    port_set_trigger_in_equal(p_lr_test, 0);
    (void) port_in(p_lr_test);
    port_set_trigger_in_equal(p_lr_test, 1);
    (void) port_in(p_lr_test);
    port_set_trigger_in_equal(p_lr_test, 0);
    (void) port_in(p_lr_test);
    port_set_trigger_in_equal(p_lr_test, 1);
    (void) port_in(p_lr_test);
    time = port_get_trigger_time(p_lr_test);

    int time_old = time;
    int counter = 0; // Do a number cycles at each delay value
    while (1) {
        port_set_trigger_in_equal(p_lr_test, 0);
        (void) port_in(p_lr_test);
        counter++;

        port_set_trigger_in_equal(p_lr_test, 1);
        (void) port_in(p_lr_test);
        time = port_get_trigger_time(p_lr_test);

        int diff = DIFF_WRAP_16(time, time_old);
        if (diff > (period + JITTER)) {
            // The delay we're able to add in the i2s_receive() function should be acceptable_receive_delay ticks or more
            if(receive_delay < acceptable_receive_delay)
            {
                printf("Backpressure breaks at receive delay ticks = %d, acceptable receive delay = %d\n",
                    receive_delay, acceptable_receive_delay);
                printf("actual diff: %d, maximum (period + Jitter): %d\n", diff, (period + JITTER));
                _Exit(1);
            }

            // The delay we're able to add in the i2s_send() function should be acceptable_send_delay ticks or more
            if(send_delay < acceptable_send_delay)
            {
                printf("Backpressure breaks at send delay ticks = %d, acceptable send delay = %d\n",
                    send_delay, acceptable_send_delay);
                printf("actual diff: %d, maximum (period + Jitter): %d\n", diff, (period + JITTER));
                _Exit(1);
            }
            printf("PASS\n");
            _Exit(0);
        }

        if (counter == N_CYCLES_AT_DELAY) {
            receive_delay += RECEIVE_DELAY_INCREMENT;
            send_delay += SEND_DELAY_INCREMENT;
            counter = 0;
        }
        time_old = time;
    }
}

DECLARE_JOB(burn, (void));

void burn(void) {
    for(;;);
}

int main() {
    populate_acceptable_delay_ticks();
    get_acceptable_delay();

    i2s_callback_group_t i_i2s = {
            .init = (i2s_init_t) i2s_init,
            .restart_check = (i2s_restart_check_t) i2s_restart_check,
            .receive = (i2s_receive_t) i2s_receive,
            .send = (i2s_send_t) i2s_send,
            .app_data = NULL,
    };
    port_enable(p_bclk);
    port_enable(p_mclk);


#if GENERATE_MCLK
    // Generate a 25Mhz clock internally and drive p_mclk from that
    printf("Using divided reference clock\n");

    clock_enable(mclk);
    clock_set_source_clk_ref(mclk);
    clock_set_divide(mclk,2);   // 100 / 2*2 = 25Mhz
    port_set_clock(p_mclk, mclk);
    port_set_out_clock(p_mclk);
    clock_start(mclk);
#endif

  PAR_JOBS (
      PJOB(i2s_master, (
          &i_i2s,
          p_dout,
          NUM_I2S_LINES,
          p_din,
          NUM_I2S_LINES,
          p_bclk,
          p_lrclk,
          p_mclk,
          bclk)),

      PJOB(test_lr_period, ())
#if BURN_THREADS > 0
      ,
#endif

#if BURN_THREADS > 5
      PJOB(burn, ()),
#endif
#if BURN_THREADS > 4
      PJOB(burn, ()),
#endif
#if BURN_THREADS > 3
      PJOB(burn, ()),
#endif
#if BURN_THREADS > 2
      PJOB(burn, ()),
#endif
#if BURN_THREADS > 1
      PJOB(burn, ()),
#endif
#if BURN_THREADS > 0
      PJOB(burn, ())
#endif
  );
  return 0;
}
