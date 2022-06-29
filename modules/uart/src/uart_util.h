// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/**
 * This file contains the prototypes for the FIFO used by the buffered UART modes (bare metal only)
 */

#pragma once
#include <stddef.h>
#include <stdint.h>

typedef enum {
    UART_BUFFER_OK = 0,
    UART_BUFFER_EMPTY,
    UART_BUFFER_FULL
} uart_buffer_error_t;

typedef struct {
    size_t size_plus_one;
    unsigned write_idx;
    unsigned read_idx;
    uint8_t * buffer;
} uart_buffer_t;

/**
 * Initialises the FIFO. Note that to avoid the need for locks, this FIFO needs
 * storage of one more than the size. The FIFO is thread safe and may be safely 
 * used across two threads without the need for mutex/locks etc.
 *
 * \param buff_cfg      The FIFO context to initialise.
 * \param storage_array The storage for the FIFO. Note this needs to be of size
 *                      one larger than the desired size of FIFO. e.g. 65 for a 64 item
 *                      FIFO.
 * \param size_plus_one The size of the storage array, which is one larger than the
 *                      actual FIFO size.
 * 
 */
void init_buffer(uart_buffer_t *buff_cfg, uint8_t *storage_array, unsigned size_plus_one);

unsigned get_buffer_fill_level(uart_buffer_t *uart_cfg);

uart_buffer_error_t push_byte_into_buffer(uart_buffer_t *buff_cfg, uint8_t data);

uart_buffer_error_t pop_byte_from_buffer(uart_buffer_t *buff_cfg, uint8_t *data);

__attribute__((always_inline))
inline int buffer_used(uart_buffer_t *buff_cfg){
    return(buff_cfg->buffer != NULL);
}
