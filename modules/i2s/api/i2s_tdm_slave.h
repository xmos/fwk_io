// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#pragma once

#include <xs1.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <xcore/port.h>
#include <xcore/clock.h>
#include <xcore/parallel.h>

#include "i2s.h"

/**
 * \addtogroup hil_i2s_tdm_core hil_i2s_tdm_core
 *
 * The public API for using the HIL I2S TDM core.
 * @{
 */

#define I2S_TDM_MAX_POUT_CNT  4
#define I2S_TDM_MAX_PIN_CNT   4

/* Max channels per frame.  This is overrideable to enable
 * memory savings in specific applications by allowing
 * IO buffers to be optimal size, saving up to
 * word length bytes * (default val - app defined value) bytes.
 */
#ifndef I2S_TDM_MAX_CH_PER_FRAME
#define I2S_TDM_MAX_CH_PER_FRAME 16
#endif /* I2S_TDM_MAX_CH_PER_FRAME */

/**
 * TDM post resource initialization event callback.
 *
 * The TDM component will call this after it first initializes the ports. 
 * This gives the app the chance to make adjustments to port timing which
 * are often needed when clocking above 15MHz.
 *
 * \param i2s_tdm_ctx Points to i2s_tdm_ctx_t struct allowing the resources to be
 *                    modified after they have been enabled and initialised. 
 *
 */
typedef void (*tdm_post_port_init_t)(void *i2s_tdm_ctx);

/**
 * This attribute must be specified on the TDM callback function
 * provided by the application.
 */
#define TDM_CALLBACK_ATTR __attribute__((fptrgroup("i2s_callback")))


/**
 * Struct to hold an I2S TDM context.
 *
 * The members in this struct should not be accessed directly.
 */
typedef struct {
    i2s_callback_group_t *i2s_cbg;
    TDM_CALLBACK_ATTR tdm_post_port_init_t tdm_post_port_init;
    port_t p_dout[I2S_TDM_MAX_POUT_CNT];
    size_t num_out;
    port_t p_din[I2S_TDM_MAX_PIN_CNT];
    size_t num_in;
    port_t p_fsync;
    port_t p_bclk;
    xclock_t bclk;
    uint32_t tx_offset;
    uint32_t fsync_len;
    uint32_t word_len;
    uint32_t ch_len;
    uint32_t ch_per_frame;
    i2s_slave_bclk_polarity_t slave_bclk_polarity;
    bool fysnch_error; /* This is set (and can be checked in tdm_post_port_init() if needed) */
    void *app_data;
} i2s_tdm_ctx_t;

/**@}*/ // END: addtogroup hil_i2s_tdm_core

DECLARE_JOB(i2s_tdm_slave_tx_16_thread, (i2s_tdm_ctx_t *));
DECLARE_JOB(i2s_slave_tdm_thread, (i2s_tdm_ctx_t *));

/**
 * \addtogroup hil_i2s_tdm_slave_tx16 hil_i2s_tdm_slave_tx16
 *
 * The public API for using the HIL I2S TDM TX 16 slave.
 * @{
 */

/**
 * I2S TDM slave context initialization for 16 channel TX only with
 * 1 output port, 32b word length, 32b channel length, 
 * and 16 channels per frame.
 *
 * This prepares a context for I2S TDM slave on the provided pins.
 * 
 * The resulting context can be used with i2s_tdm_slave_tx_16_thread().
 *
 * \param ctx                   A pointer to the I2S TDM context to use.
 * \param i2s_cbg               The I2S callback group pointing to the application's
 *                              functions to use for initialization and getting and receiving
 *                              frames.  For TDM the app_data variable within this
 *                              struct is NOT used.
 * \param p_dout                The data output port.  MUST be a 1b port
 * \param p_fsync               The fsync input port. MUST be a 1b port
 * \param p_bclk                The bit clock input port. MUST be a 1b port
 * \param bclk                  A clock that will get configured for use with
 *                              the bit clock
 * \param tx_offset             The number of bclks from FSYNC transition to the MSB
 *                              of Slot 0
 * \param slave_bclk_pol        The polarity of bclk
 * \param tdm_post_port_init    Callback to be called just after resource init.
 *                              Allows for modification of port timing for >15MHz clocks.
 *                              Set to NULL if not needed.
 */
void i2s_tdm_slave_tx_16_init(
        i2s_tdm_ctx_t *ctx,
        i2s_callback_group_t *i2s_cbg,
        port_t p_dout,
        port_t p_fsync,
        port_t p_bclk,
        xclock_t bclk,
        uint32_t tx_offset,
        i2s_slave_bclk_polarity_t slave_bclk_polarity,
        tdm_post_port_init_t tdm_post_port_init);

/**
 * I2S TDM TX 16 ch slave task
 *
 * This task performs I2S TDM slave on the provided context which was
 * initialized with i2s_tdm_slave_tx_16_init(). It will perform
 * callbacks over the i2s_callback_group_t callback group to get
 * data from the application using this component.
 * 
 * This thread assumes 1 data output port, 32b word length,
 * 32b channel length, and 16 channels per frame.
 *
 * The component performs I2S TDM slave so will expect the fsync and
 * bit clock to be driven externally.
 *
 * \param ctx             A pointer to the I2S TDM context to use.
 */
void i2s_tdm_slave_tx_16_thread(
        i2s_tdm_ctx_t *ctx);

/**@}*/ // END: addtogroup hil_i2s_tdm_slave_tx16

/**
 * \addtogroup hil_i2s_tdm_slave hil_i2s_tdm_slave
 *
 * The public API for using the HIL I2S TDM generic slave.
 * @{
 */

/**
 * I2S generic TDM slave context initialization
 *
 * This prepares a context for I2S TDM slave on the provided pins.
 * 
 * The resulting context can be used with an I2S thread call.
 *
 * \param ctx                   A pointer to the I2S TDM context to use.
 * \param i2s_cbg               The I2S callback group pointing to the application's
 *                              functions to use for initialization and getting and receiving
 *                              frames.  For TDM the app_data variable within this
 *                              struct is NOT used.
 * \param p_dout                An array of data output ports.  MUST be 1b ports
 * \param num_out               The number of output data ports
 * \param p_din                 An array of data input ports.  MUST be 1b ports
 * \param num_in                The number of input data ports
 * \param p_fsync               The fsync input port. MUST be a 1b port
 * \param p_bclk                The bit clock input port. MUST be a 1b port
 * \param bclk                  A clock that will get configured for use with
 *                              the bit clock
 * \param tx_offset             The number of BCLKS from FSYNC transition to the MSB
 *                              of Slot 0 
 * \param fsync_len             The length of the FSYNC in BCLKS
 * \param word_len              The number of bits in each sample frame slot.
 *                              MUST be 32.
 * \param ch_len                The number of bits in each channel. MUST be less than
 *                              word_len
 * \param ch_per_frame          The number of channels per frame
 * \param slave_bclk_pol        The polarity of bclk
 * \param tdm_post_port_init    Callback to be called just after resource init.
 *                              Allows for modification of port timing for >15MHz clocks.
 *                              Set to NULL if not needed.
 */
void i2s_tdm_slave_init(
        i2s_tdm_ctx_t *ctx,
        i2s_callback_group_t *i2s_cbg,
        port_t p_dout[],
        size_t num_out,
        port_t p_din[],
        size_t num_in,
        port_t p_fsync,
        port_t p_bclk,
        xclock_t bclk,
        uint32_t tx_offset,
        uint32_t fsync_len,
        uint32_t word_len,
        uint32_t ch_len,
        uint32_t ch_per_frame,
        i2s_slave_bclk_polarity_t slave_bclk_pol,
        tdm_post_port_init_t tdm_post_port_init
);

/**
 * I2S generic TDM slave task
 *
 * This task performs I2S TDM slave on the provided context. It will perform
 * callbacks over the i2s_callback_group_t callback group to get/receive
 * data from the application using this component.
 *
 * The component performs I2S TDM slave so will expect the fsync and
 * bit clock to be driven externally.
 *
 * \param ctx             A pointer to the I2S TDM context to use.
 */
void i2s_slave_tdm_thread(
        i2s_tdm_ctx_t *ctx);

/**@}*/ // END: addtogroup hil_i2s_tdm_slave
