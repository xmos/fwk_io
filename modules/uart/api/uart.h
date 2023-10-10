// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#pragma once

/** \file
 *  \brief API for UART I/O
 */

#include <stdlib.h> /* for size_t */
#include <stdint.h>
#include <xcore/port.h>
#include <xcore/triggerable.h>
#include <xcore/hwtimer.h>
#include <xcore/interrupt.h>

#include "uart_util.h"

/**
 * \addtogroup hil_uart_tx hil_uart_tx
 *
 * The public API for using the HIL UART Tx Module.
 * @{
 */

/**
 * Enum type representing the different options
 * parity types.
 */
typedef enum uart_parity {
  UART_PARITY_NONE = 0,
  UART_PARITY_EVEN,
  UART_PARITY_ODD
} uart_parity_t;

/**
 * Define which sets the enum start point of RX errors. This is relied upon
 * by the RTOS drivers and allows optimisation of error handling.
 */
#define UART_START_BIT_ERROR_VAL 2

/**
 * Enum type representing the callback error codes.
 *
 */
typedef enum {
    UART_RX_COMPLETE        = 0,
    UART_UNDERRUN_ERROR     = 1,  //Buffered Tx Error only (buffer empty)
    UART_START_BIT_ERROR    = UART_START_BIT_ERROR_VAL,     //Rx Only
    UART_PARITY_ERROR       = UART_START_BIT_ERROR_VAL + 1, //Rx Only
    UART_FRAMING_ERROR      = UART_START_BIT_ERROR_VAL + 2, //Rx Only
    UART_OVERRUN_ERROR      = UART_START_BIT_ERROR_VAL + 3, //Buffered Rx only
} uart_callback_code_t;

/**
 * Enum type representing the different states
 * for the UART logic.
 */
typedef enum {
    UART_IDLE = 0,
    UART_START,
    UART_DATA,
    UART_PARITY,
    UART_STOP
} uart_state_t;


/**
 * This attribute must be specified on the UART TX UNDERRUN callback function
 * provided by the application. It ensures the correct stack usage
 * is calculated.
 */
#define HIL_UART_TX_CALLBACK_ATTR __attribute__((fptrgroup("hil_uart_tx_callback")))

/**
 * This attribute must be specified on the UART Rx callback functions
 * (both ERROR and Rx complete callbacks) provided by the application. 
 * It ensures the correct stack usage is correctly calculated.
 */
#define HIL_UART_RX_CALLBACK_ATTR __attribute__((fptrgroup("hil_uart_rx_callback")))


/**
 * Struct to hold a UART Tx context.
 *
 * The members in this struct should not be accessed directly. Use the
 * API provided instead.
 */
typedef struct {
    uart_state_t state;
    port_t tx_port;
    uint32_t bit_time_ticks;
    uint32_t next_event_time_ticks;
    uart_parity_t parity;
    uint8_t num_data_bits;
    uint8_t current_data_bit;
    uint8_t uart_data;
    uint8_t stop_bits;
    uint8_t current_stop_bit;

    HIL_UART_TX_CALLBACK_ATTR void(*uart_tx_empty_callback_fptr)(void* app_data);
    void *app_data;
    hwtimer_t tmr;
    uart_buffer_t buffer;
} uart_tx_t;


/**
 * Initializes a UART Tx I/O interface. Passing a valid buffer will enable
 * buffered mode with ISR for use in bare-metal applications.
 *
 * \param uart          The uart_tx_t context to initialise.
 * \param tx_port       The port used transmit the UART frames.
 * \param baud_rate     The baud rate of the UART in bits per second.
 * \param data_bits     The number of data bits per frame sent.
 * \param parity        The type of parity used. See uart_parity_t above.
 * \param stop_bits     The number of stop bits asserted at the of the frame.
 * \param tmr           The resource id of the timer to be used. Polling mode
 *                      will be used if set to 0.
 * \param tx_buff       Pointer to a buffer. Optional. If set to zero the 
 *                      UART will run in blocking mode. If initialised to a
 *                      valid buffer, the UART will be interrupt driven.
 * \param buffer_size_plus_one   Size of the buffer if enabled in tx_buff. 
 *                      Note that the buffer allocation and size argument
 *                      must be one greater than needed. Eg. buff[65] for a
 *                      64 byte buffer.
 * \param uart_tx_empty_callback_fptr Callback function pointer for UART buffer 
 *                      empty in buffered mode.
 * \param app_data      A pointer to application specific data provided
 *                      by the application. Used to share data between
 *                      this callback function and the application.
 */
void uart_tx_init(
        uart_tx_t *uart,
        port_t tx_port,
        uint32_t baud_rate,
        uint8_t data_bits,
        uart_parity_t parity,
        uint8_t stop_bits,

        hwtimer_t tmr,
        uint8_t *tx_buff,
        size_t buffer_size_plus_one,
        void(*uart_tx_empty_callback_fptr)(void* app_data),
        void *app_data
        );


/**
 * Initializes a UART Tx I/O interface. The API is hard wired to
 * blocking mode where the call to uart_tx will return at the end of
 * sending the stop bit.
 *
 * \param uart          The uart_tx_t context to initialise.
 * \param tx_port       The port used transmit the UART frames.
 * \param baud_rate     The baud rate of the UART in bits per second.
 * \param data_bits     The number of data bits per frame sent.
 * \param parity        The type of parity used. See uart_parity_t above.
 * \param stop_bits     The number of stop bits asserted at the of the frame.
 * \param tmr           The resource id of the timer to be used. Polling mode
 *                      will be used if set to 0.
 */
void uart_tx_blocking_init(
        uart_tx_t *uart,
        port_t tx_port,
        uint32_t baud_rate,
        uint8_t data_bits,
        uart_parity_t parity,
        uint8_t stop_bits,
        hwtimer_t tmr);


/**
 * Transmits a single UART frame with parameters as specified in uart_tx_init()
 *
 * \param uart          The uart_tx_t context to initialise.
 * \param data          The word to transmit.
 */
void uart_tx(
        uart_tx_t *uart,
        uint8_t data);

/**
 * De-initializes the specified UART Tx interface. This disables the
 * port also. The timer, if used, needs to be freed by the application.
 *
 * \param uart          The uart_tx_t context to de-initialise.
 */
void uart_tx_deinit(
        uart_tx_t *uart);



/**@}*/ // END: addtogroup hil_uart_tx

/**
 * \addtogroup hil_uart_rx hil_uart_rx
 *
 * The public API for using the HIL UART Rx module.
 * @{
 */

/**
 * Struct to hold a UART Rx context.
 *
 * The members in this struct should not be accessed directly. Use the
 * API provided instead.
 */
typedef struct {
    uart_state_t state;
    port_t rx_port;
    uint32_t bit_time_ticks;
    uint32_t next_event_time_ticks;
    uart_parity_t parity;
    uint8_t num_data_bits;
    uint8_t current_data_bit;
    uint8_t uart_data;
    uint8_t stop_bits;
    uint8_t current_stop_bit;

    uart_callback_code_t cb_code;
    HIL_UART_RX_CALLBACK_ATTR void(*uart_rx_complete_callback_arg)(void* app_data);
    HIL_UART_RX_CALLBACK_ATTR void(*uart_rx_error_callback_arg)(uart_callback_code_t callback_code, void* app_data);
    void *app_data;
    hwtimer_t tmr;
    uart_buffer_t buffer;
} uart_rx_t;


/**
 * Initializes a UART Rx I/O interface. Passing a valid buffer will enable
 * buffered mode with ISR for use in bare-metal applications.
 *
 * \param uart          The uart_rx_t context to initialise.
 * \param rx_port       The port used receive the UART frames.
 * \param baud_rate     The baud rate of the UART in bits per second.
 * \param data_bits     The number of data bits per frame sent.
 * \param parity        The type of parity used. See uart_parity_t above.
 * \param stop_bits     The number of stop bits asserted at the of the frame.
 * \param tmr           The resource id of the timer to be used. Polling mode
 *                      will be used if set to 0.
 * \param rx_buff       Pointer to a buffer. Optional. If set to zero the 
 *                      UART will run in blocking mode. If initialised to a
 *                      valid buffer, the UART will be interrupt driven.
 * \param buffer_size_plus_one   Size of the buffer if enabled in rx_buff. 
 *                      Note that the buffer allocation and size argument
 *                      must be one greater than needed. Eg. buff[65] for a
 *                      64 byte buffer. 
 * \param uart_rx_complete_callback_fptr Callback function pointer for UART rx
 *                      complete (one word) in buffered mode only. Optionally NULL.
 * \param uart_rx_error_callback_fptr Callback function pointer for UART rx errors 
 *                      The error is contained in cb_code in the uart_rx_t struct.
 * \param app_data      A pointer to application specific data provided
 *                      by the application. Used to share data between
 *                      this callback function and the application.
 */
void uart_rx_init(
        uart_rx_t *uart,
        port_t rx_port,
        uint32_t baud_rate,
        uint8_t data_bits,
        uart_parity_t parity,
        uint8_t stop_bits,

        hwtimer_t tmr,
        uint8_t *rx_buff,
        size_t buffer_size_plus_one,
        void(*uart_rx_complete_callback_fptr)(void *app_data),
        void(*uart_rx_error_callback_fptr)(uart_callback_code_t callback_code, void *app_data),
        void *app_data
        );

/**
 * Initializes a UART Rx I/O interface. This API is fixed to blocking mode
 * which is where the call to uart_rx returns as soon as the stop bit has
 * been sampled.
 *
 * \param uart          The uart_rx_t context to initialise.
 * \param rx_port       The port used receive the UART frames.
 * \param baud_rate     The baud rate of the UART in bits per second.
 * \param data_bits     The number of data bits per frame sent.
 * \param parity        The type of parity used. See uart_parity_t above.
 * \param stop_bits     The number of stop bits asserted at the of the frame.
 * \param tmr           The resource id of the timer to be used. Polling mode
 *                      will be used if set to 0.
 * \param uart_rx_error_callback_fptr Callback function pointer for UART rx errors 
 *                      The error is contained in cb_code in the uart_rx_t struct.
 * \param app_data      A pointer to application specific data provided
 *                      by the application. Used to share data between
 *                      the error callback function and the application.
 */
void uart_rx_blocking_init(
        uart_rx_t *uart,
        port_t rx_port,
        uint32_t baud_rate,
        uint8_t data_bits,
        uart_parity_t parity,
        uint8_t stop_bits,
        hwtimer_t tmr,
        void(*uart_rx_error_callback_fptr)(uart_callback_code_t callback_code, void *app_data),
        void *app_data
        );

/**
 * Receives a single UART frame with parameters as specified in uart_rx_init()
 *
 * \param uart          The uart_rx_t context to receive from.
 * 
 * \return              The word received in the UART frame. In buffered mode
 *                      it gets the oldest received word.
 */
uint8_t uart_rx(uart_rx_t *uart);

/**
 * De-initializes the specified UART Rx interface. This disables the
 * port also. The timer, if used, needs to be freed by the application.
 *
 * \param uart          The uart_rx_t context to de-initialise.
 */
void uart_rx_deinit(uart_rx_t *uart);



/**@}*/ // END: addtogroup hil_uart_rx
