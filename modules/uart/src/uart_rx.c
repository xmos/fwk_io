// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdint.h>
#include <xcore/assert.h>
#include <xcore/interrupt_wrappers.h>

#include "uart.h"

#define UART_RX_DEBUG 0 //Drives debug port for checking state timing in simulator
#if UART_RX_DEBUG
port_t p_dbg = XS1_PORT_32A;
#endif

DECLARE_INTERRUPT_CALLBACK(uart_rx_handle_isr, callback_info);

void uart_rx_blocking_init(
        uart_rx_t *uart,
        port_t rx_port,
        uint32_t baud_rate,
        uint8_t num_data_bits,
        uart_parity_t parity,
        uint8_t stop_bits,
        hwtimer_t tmr,
        void(*uart_rx_error_callback_fptr)(uart_callback_code_t callback_code, void *app_data),
        void *app_data){

    uart_rx_init(uart, rx_port, baud_rate, num_data_bits, parity, stop_bits, tmr,
        NULL, 0, NULL, uart_rx_error_callback_fptr, app_data);
}

void uart_rx_init(
        uart_rx_t *uart,
        port_t rx_port,
        uint32_t baud_rate,
        uint8_t num_data_bits,
        uart_parity_t parity,
        uint8_t stop_bits,

        hwtimer_t tmr,
        uint8_t *buffer,
        size_t buffer_size_plus_one,
        void(*uart_rx_complete_callback_fptr)(void *app_data),
        void(*uart_rx_error_callback_fptr)(uart_callback_code_t callback_code, void *app_data),
        void *app_data
        ){

    #if UART_RX_DEBUG
    port_enable(p_dbg);
    #endif

    uart->rx_port = rx_port;
    uart->bit_time_ticks = XS1_TIMER_HZ / baud_rate;
    uart->next_event_time_ticks = 0;
    xassert(num_data_bits <= 8 && num_data_bits >= 5);
    uart->num_data_bits = num_data_bits;
    xassert(parity == UART_PARITY_NONE || parity == UART_PARITY_EVEN || parity == UART_PARITY_ODD);
    uart->parity = parity;
    uart->stop_bits = stop_bits;
    uart->current_data_bit = 0;
    uart->uart_data = 0;
    uart->state = UART_IDLE;

    //HW timer will be replaced by poll if set to zero
    uart->tmr = tmr;

    init_buffer(&uart->buffer, buffer, buffer_size_plus_one);
    if(buffer_used(&uart->buffer)){
        xassert(buffer_size_plus_one > (1 + 1)); // Buffer must be at least one deep to be valid
        //From now on we just check to see if buffer is NULL or not for speed
    }

    uart->cb_code = UART_RX_COMPLETE;
    uart->uart_rx_complete_callback_arg = uart_rx_complete_callback_fptr;
    uart->uart_rx_error_callback_arg = uart_rx_error_callback_fptr;
    uart->app_data = app_data;

    //Assert if buffer is used but no timer as we need the timer for buffered mode 
    if(buffer_used(&uart->buffer) && !tmr){
        xassert(0);    
    }

    port_enable(rx_port);

    if(buffer_used(&uart->buffer)){
        init_buffer(&uart->buffer, buffer, buffer_size_plus_one);

        //Setup interrupts
        interrupt_mask_all();
        port_in(rx_port); //Ensure port is input and clear trigger
        port_set_trigger_in_equal(rx_port, 0); //Trigger on low (start of start bit)
        triggerable_setup_interrupt_callback(rx_port, uart, INTERRUPT_CALLBACK(uart_rx_handle_isr) );

        hwtimer_clear_trigger_time(uart->tmr);
        triggerable_setup_interrupt_callback(tmr, uart, INTERRUPT_CALLBACK(uart_rx_handle_isr) );

        //Initial ISR will be on falling edge of rx, followed by timer ISRs for sampling the values
        triggerable_set_trigger_enabled(uart->rx_port, 1);
        triggerable_set_trigger_enabled(uart->tmr, 0);

        interrupt_unmask_all();
    } else {
        init_buffer(&uart->buffer, NULL, 0);
    }
}

__attribute__((always_inline))
static inline uint32_t get_current_time(uart_rx_t *uart){
    // if(uart->tmr){
    //     return hwtimer_get_time(uart->tmr);
    // }
    //Note this has now been optimised since all timers share the same physical timer counter
    return get_reference_time();
}


__attribute__((always_inline))
static inline void sleep_until_start_transition(uart_rx_t *uart){
    if(uart->tmr){
        //Wait on a port transition to low
        port_in_when_pinseq(uart->rx_port, PORT_UNBUFFERED, 0);
    }else{
        //Poll the port
        while(port_in(uart->rx_port) & 0x1);
    }
}

__attribute__((always_inline))
static inline void sleep_until_next_sample(uart_rx_t *uart){
    if(uart->tmr){
        //Wait on a the timer
        hwtimer_wait_until(uart->tmr, uart->next_event_time_ticks);
    }else{
        //Poll the timer
        while(get_current_time(uart) < uart->next_event_time_ticks);
    }
}

// Interrupt latency and calling overhead has been measured at 510ns @ 75-120MHz thread speed 
// Latency for polling/timer wait mode is around 320ns @ 75-120MHz thread speed   
// These values have been experimentally derived using xsim (See enabling of debug in test_rx_uart.py)
// and then inspecting the VCD waveform to see where the start bit samples in relation 
// to the falling edge of the start bit. Turn on debug mode and observe p_dbg

#define INTERRUPT_LATENCY_COMPENSATION_TICKS (XS1_TIMER_MHZ * 510 / 1000)
#define BLOCKING_LATENCY_COMPENSATION_TICKS  (XS1_TIMER_MHZ * 320 / 1000)


__attribute__((always_inline))
static inline void uart_rx_handle_event(uart_rx_t *uart){
    switch(uart->state){
        case UART_IDLE: {
            #if UART_RX_DEBUG
            port_out(p_dbg, uart->state);
            #endif

            uart->next_event_time_ticks = get_current_time(uart);
            uart->next_event_time_ticks += uart->bit_time_ticks >> 1; //Halfway through start bit
            uart->state = UART_START;
            if(buffer_used(&uart->buffer)){
                uart->next_event_time_ticks -= INTERRUPT_LATENCY_COMPENSATION_TICKS;
                triggerable_set_trigger_enabled(uart->rx_port, 0);
                port_clear_trigger_in(uart->rx_port);
                hwtimer_set_trigger_time(uart->tmr, uart->next_event_time_ticks);
                triggerable_set_trigger_enabled(uart->tmr, 1);
            } else {
                uart->next_event_time_ticks -= BLOCKING_LATENCY_COMPENSATION_TICKS;
            }
            break;
        }

        case UART_START: {
            #if UART_RX_DEBUG
            port_out(p_dbg, uart->state);
            #endif

            uint32_t pin = port_in(uart->rx_port) & 0x1;
            if(pin != 0){
                uart->cb_code = UART_START_BIT_ERROR;
                (*uart->uart_rx_error_callback_arg)(uart->cb_code, uart->app_data);
            }
            uart->state = UART_DATA;
            uart->uart_data = 0;
            uart->current_data_bit = 0;
            uart->next_event_time_ticks += uart->bit_time_ticks;
            if(buffer_used(&uart->buffer)){
                hwtimer_set_trigger_time(uart->tmr, uart->next_event_time_ticks);
            }
            break;
        }

        case UART_DATA: { 
            #if UART_RX_DEBUG
            port_out(p_dbg, uart->state);
            #endif

            uint32_t pin = port_in(uart->rx_port) & 0x1;
            uart->uart_data |= pin << uart->current_data_bit;
            uart->current_data_bit += 1;

            if(uart->current_data_bit == uart->num_data_bits){
                if(uart->parity == UART_PARITY_NONE){
                    uart->state = UART_STOP;
                } else {
                    uart->state = UART_PARITY;
                }
            }
            uart->next_event_time_ticks += uart->bit_time_ticks;
            if(buffer_used(&uart->buffer)){
                hwtimer_set_trigger_time(uart->tmr, uart->next_event_time_ticks);
            }
            break;
        }

        case UART_PARITY: {
            #if UART_RX_DEBUG
            port_out(p_dbg, uart->state);
            #endif

            uint32_t pin = port_in(uart->rx_port) & 0x1;
            uint32_t parity_setting = (uart->parity == UART_PARITY_EVEN) ? 0 : 1;
            uint32_t parity = (unsigned)uart->uart_data;
            // crc32(parity, parity_setting, 1); //http://bugzilla/show_bug.cgi?id=18663
            asm volatile("crc32 %0, %2, %3" : "=r" (parity) : "0" (parity), "r" (parity_setting), "r" (1));
            parity &= 1;
            if(pin != parity){
                uart->cb_code = UART_PARITY_ERROR;
                (*uart->uart_rx_error_callback_arg)(uart->cb_code, uart->app_data);
            }
            uart->state = UART_STOP;
            uart->next_event_time_ticks += uart->bit_time_ticks;
            if(buffer_used(&uart->buffer)){
                hwtimer_set_trigger_time(uart->tmr, uart->next_event_time_ticks);
            }
            break;
        }
     
        case UART_STOP: {
            #if UART_RX_DEBUG
            port_out(p_dbg, uart->state);
            #endif

            uint32_t pin = port_in(uart->rx_port) & 0x1;
            if(pin != 1){
                uart->cb_code = UART_FRAMING_ERROR;
                (*uart->uart_rx_error_callback_arg)(uart->cb_code, uart->app_data);
            } else {
                uart->cb_code = UART_RX_COMPLETE;
            }
            uart->state = UART_IDLE;

            //Go back to waiting for next start bit transition
            if(buffer_used(&uart->buffer)){
                uart_buffer_error_t err = push_byte_into_buffer(&uart->buffer, uart->uart_data);
                if(err == UART_BUFFER_FULL){
                    uart->cb_code = UART_OVERRUN_ERROR;
                    (*uart->uart_rx_error_callback_arg)(uart->cb_code, uart->app_data);
                }
                hwtimer_clear_trigger_time(uart->tmr);
                triggerable_set_trigger_enabled(uart->tmr, 0);

                port_set_trigger_in_equal(uart->rx_port, 0); //Trigger on low (start of start bit)
                triggerable_set_trigger_enabled(uart->rx_port, 1);

                if(uart->uart_rx_complete_callback_arg != NULL){
                    (*uart->uart_rx_complete_callback_arg)(uart->app_data);
                }
            }
            break;
        }

        default: {
            xassert(0);
        }
    }
}

DEFINE_INTERRUPT_CALLBACK(UART_RX_INTERRUPTABLE_FUNCTIONS, uart_rx_handle_isr, callback_info){
    uart_rx_t *uart = (uart_rx_t *)callback_info;
    uart_rx_handle_event(uart);
}

uint8_t uart_rx(uart_rx_t *uart){
    if(buffer_used(&uart->buffer)){
        uint8_t rx_data = 0;
        uart_buffer_error_t err = pop_byte_from_buffer(&uart->buffer, &rx_data);
        if(err == UART_BUFFER_EMPTY){
            uart->cb_code = UART_UNDERRUN_ERROR;
            (*uart->uart_rx_error_callback_arg)(uart->cb_code, uart->app_data);
        }
        return rx_data;
    } else {
        uart->state = UART_IDLE;
        sleep_until_start_transition(uart);
        do{
            uart_rx_handle_event(uart);
            sleep_until_next_sample(uart);
        } while(uart->state != UART_IDLE);

        return uart->uart_data;
    }
}

void uart_rx_deinit(uart_rx_t *uart){
    interrupt_mask_all();
    if(buffer_used(&uart->buffer)){        
        triggerable_set_trigger_enabled(uart->rx_port, 0);
        triggerable_set_trigger_enabled(uart->tmr, 0);
    }
    port_disable(uart->rx_port);
    interrupt_unmask_all();
}

