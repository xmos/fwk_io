// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include "uart_util.h"
#include <string.h>

void init_buffer(uart_buffer_t *buff_cfg, uint8_t *storage_array, unsigned size_plus_one){
    buff_cfg->buffer = storage_array;
    buff_cfg->size_plus_one = size_plus_one;
    buff_cfg->write_idx = 0;
    buff_cfg->read_idx = 0;
    memset(buff_cfg->buffer, 0, size_plus_one);
}

inline unsigned get_buffer_fill_level(uart_buffer_t *buff_cfg){
    unsigned fill_level = 0;
    if(buff_cfg->write_idx >= buff_cfg->read_idx){
        fill_level = buff_cfg->write_idx - buff_cfg->read_idx;
    } else {
        fill_level = buff_cfg->size_plus_one + buff_cfg->write_idx - buff_cfg->read_idx;
    }

    return fill_level;
}

uart_buffer_error_t push_byte_into_buffer(uart_buffer_t *buff_cfg, uint8_t data){
    unsigned next_write_idx = buff_cfg->write_idx + 1;
    if(next_write_idx == buff_cfg->size_plus_one){
        next_write_idx = 0;
    } 
    
    if(next_write_idx == buff_cfg->read_idx){
        return UART_BUFFER_FULL;
    }

    buff_cfg->buffer[buff_cfg->write_idx] = data; //Data first so valid after idx++
    buff_cfg->write_idx = next_write_idx; //Update wrapped next write idx
 
    return UART_BUFFER_OK;
}

uart_buffer_error_t pop_byte_from_buffer(uart_buffer_t *buff_cfg, uint8_t *data){    
    if(buff_cfg->read_idx == buff_cfg->write_idx){
        return UART_BUFFER_EMPTY;
    }
    *data = buff_cfg->buffer[buff_cfg->read_idx];

    unsigned next_read_idx = buff_cfg->read_idx + 1;
    if(next_read_idx == buff_cfg->size_plus_one){ //wrap
       buff_cfg->read_idx = 0;
    } else {
        buff_cfg->read_idx = next_read_idx;
    }
    
    return UART_BUFFER_OK;
}

