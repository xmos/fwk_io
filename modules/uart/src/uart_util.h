// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/**
 * This file contains the prototypes for the FIFO used by the buffered UART modes (bare metal only)
 */

#pragma once
#include <stddef.h>
#include <stdint.h>
#include <xcore/lock.h>
#include <xcore/port.h>


typedef enum {
    UART_BUFFER_OK = 0,
    UART_BUFFER_EMPTY,
    UART_BUFFER_FULL
} uart_buffer_error_t;

typedef struct {
    unsigned size;
    unsigned next_write_idx;
    unsigned next_read_idx;
    unsigned size_overlow_flag;
    uint8_t * buffer;
} uart_buffer_t;

void init_buffer(uart_buffer_t *buff_cfg, uint8_t *buffer, unsigned size);
unsigned get_buffer_fill_level(uart_buffer_t *uart_cfg);
uart_buffer_error_t push_byte_into_buffer(uart_buffer_t *buff_cfg, uint8_t data);
uart_buffer_error_t pop_byte_from_buffer(uart_buffer_t *buff_cfg, uint8_t *data);

__attribute__((always_inline))
inline int buffer_used(uart_buffer_t *buff_cfg){
    return((buff_cfg->size && buff_cfg->buffer != NULL));
}

__attribute__((always_inline))
inline void pin_out(port_t port, uint32_t mask, lock_t lock, unsigned value){
    if(lock){
        lock_acquire(lock);
        uint32_t curr_val = port_peek(port);
        if(value){
            curr_val |= mask;
        } else {
            curr_val &= ~mask;
        }
        port_out(port, curr_val);
        lock_release(lock);
    } else {
        port_out(port, value);
    }
}

__attribute__((always_inline))
inline unsigned pin_in(port_t port, uint32_t mask, lock_t lock){
    if(lock){
        lock_acquire(lock);
        uint32_t port_val = port_in(port);
        lock_release(lock);
        return ((port_val & mask) == mask);
    } else {
        return port_in(port);
    }
}

__attribute__((always_inline))
inline void pin_in_when_pinseq(port_t port, uint32_t mask, lock_t lock, unsigned val){
    if(lock){
        /* We have no way of easily event waiting on multiple pins in a port so we poll */ 
        uint32_t eq_val = val ? mask : 0;
        uint32_t port_val;
        do{
            lock_acquire(lock);
            port_val = port_in(port);
            lock_release(lock);
        } while((port_val & mask) != eq_val);
    } else {
        /* Use event mechanism */
        port_in_when_pinseq(port, PORT_UNBUFFERED, val);
    }
}

