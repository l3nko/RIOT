/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Internal interfaces for CC2420 drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef CC2420_INTERNAL_H_
#define CC2420_INTERNAL_H_

#include <stdint.h>

#include "cc2420.h"


#ifdef __cplusplus
extern "C" {
#endif

/* startup timeout (2 ms) in 16MHz-clock cycles */
#define CC2420_STARTUP_TIMEOUT  32000U
/* Various configuration settings for the CC2420 drivers  */
#define CC2420_SYNC_WORD_TX_TIME 900000
#define CC2420_RX_BUF_SIZE      3
#define CC2420_WAIT_TIME        500

/* Shortcut macro */
#define CC2420_STATUS_BYTE() cc2420_strobe(dev, NOBYTE)

uint8_t cc2420_strobe(const cc2420_t *dev, const uint8_t command);

/**
 * @brief   Read from a register at address `addr` from device `dev`.
 *
 * @param[in] dev       device to read from
 * @param[in] addr      address of the register to read
 *
 * @return              the value of the specified register
 */
uint16_t cc2420_reg_read(const cc2420_t *dev, const uint8_t addr);

/**
 * @brief   Write to a register at address `addr` from device `dev`.
 *
 * @param[in] dev       device to write to
 * @param[in] addr      address of the register to write
 * @param[in] value     value to write to the given register
 */
void cc2420_reg_write(const cc2420_t *dev, const uint8_t addr,
                      const uint16_t value);

/**
 * @brief   Read a chunk of data from the SRAM of the given device
 *
 * @param[in]  dev      device to read from
 * @param[in]  offset   starting address to read from
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read from SRAM
 */
void cc2420_ram_read(const cc2420_t *dev,
                     const uint16_t offset,
                     uint8_t *data,
                     const size_t len);

/**
 * @brief   Write a chunk of data into the SRAM of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] offset    address in the SRAM to write to [valid 0x00-0x7f]
 * @param[in] data      data to copy into SRAM
 * @param[in] len       number of bytes to write to SRAM
 */
void cc2420_ram_write(const cc2420_t *dev,
                      const uint16_t offset,
                      const uint8_t *data,
                      const size_t len);

/**
 * @brief   Read the internal frame buffer of the given device
 *
 * Reading the frame buffer returns some extra bytes that are not accessible
 * through reading the RAM directly.
 *
 * @param[in]  dev      device to read from
 * @param[out] data     buffer to copy the data to
 * @param[in]  len      number of bytes to read from the frame buffer
 */
void cc2420_fb_read(const cc2420_t *dev,
                    uint8_t *data, const size_t len);

/**
 * @brief   Write the internal frame buffer of the given device
 *
 * Reading the frame buffer returns some extra bytes that are not accessible
 * through reading the RAM directly.
 *
 * @param[in]  dev      device to read from
 * @param[out] data     buffer to copy the data to
 * @param[in]  len      number of bytes to read from the frame buffer
 */
void cc2420_fb_write(const cc2420_t *dev,
                     uint8_t *data, const size_t len);

/**
 * @brief   Convenience function for reading the status of the given device
 *
 * @param[in] dev       device to read the status from
 *
 * @return              internal status of the given device
 */
// uint8_t cc2420_get_status(const cc2420_t *dev);

/************************* L3NKO ****************************/
/**
 * @brief   Trigger a hardware reset and configure radio with default values
 *
 * @param[in] dev           device to reset
 */
void cc2420_reset(cc2420_t *dev);

/**
 * @brief   Trigger a clear channel assessment
 *
 * @param[in] dev           device to use
 *
 * @return                  true if channel is clear
 * @return                  false if channel is busy
 */
bool cc2420_cca(cc2420_t *dev);

/**
 * @brief   Get the short address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (2-byte) short address
 */
uint16_t cc2420_get_addr_short(cc2420_t *dev);

/**
 * @brief   Set the short address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (2-byte) short address to set
 */
void cc2420_set_addr_short(cc2420_t *dev, uint16_t addr);

/**
 * @brief   Get the configured long address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (8-byte) long address
 */
uint64_t cc2420_get_addr_long(cc2420_t *dev);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void cc2420_set_addr_long(cc2420_t *dev, uint64_t addr);

/**
 * @brief   Get the configured channel of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel
 */
uint8_t cc2420_get_chan(cc2420_t *dev);

/**
 * @brief   Set the channel of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] chan          channel to set
 */
void cc2420_set_chan(cc2420_t *dev, uint8_t chan);

/**
 * @brief   Get the configured PAN ID of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set PAN ID
 */
uint16_t cc2420_get_pan(cc2420_t *dev);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] pan           PAN ID to set
 */
void cc2420_set_pan(cc2420_t *dev, uint16_t pan);

/**
 * @brief   Get the configured transmission power of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured transmission power in dBm
 */
int16_t cc2420_get_txpower(cc2420_t *dev);

/**
 * @brief   Set the transmission power of the given device [in dBm]
 *
 * If the device does not support the exact dBm value given, it will set a value
 * as close as possible to the given value. If the given value is larger or
 * lower then the maximal or minimal possible value, the min or max value is
 * set, respectively.
 *
 * @param[in] dev           device to write to
 * @param[in] txpower       transmission power in dBm
 */
void cc2420_set_txpower(cc2420_t *dev, int16_t txpower);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in] dev           device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void cc2420_set_option(cc2420_t *dev, uint16_t option, bool state);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in] dev           device to change state of
 * @param[in] state         the targeted new state
 */
void cc2420_set_state(cc2420_t *dev, uint8_t state);

/**
 * @brief   Get the state of the given device
 *
 * @param[in] dev           device to change state of
 *
 * @return                  the device's current state
 */
uint8_t cc2420_get_state(cc2420_t *dev);

/**
 * @brief   Reset the internal state machine to TRX_OFF mode.
 *
 * This will force a transition to TRX_OFF regardless of whether the transceiver
 * is currently busy sending or receiving. This function is used to get back to
 * a known state during driver initialization.
 *
 * @param[in] dev           device to operate on
 */
void cc2420_reset_state_machine(cc2420_t *dev);

/**
 * @brief   Convenience function for simply sending data
 *
 * @note This function ignores the PRELOADING option
 *
 * @param[in] dev           device to use for sending
 * @param[in] data          data to send (must include IEEE802.15.4 header)
 * @param[in] len           length of @p data
 *
 * @return                  number of bytes that were actually send
 * @return                  0 on error
 */
size_t cc2420_send(cc2420_t *dev, uint8_t *data, size_t len);

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in] dev            device to prepare for sending
 */
void cc2420_tx_prepare(cc2420_t *dev);

/**
 * @brief   Load chunks of data into the transmit buffer of the given device
 *
 * @param[in] dev           device to write data to
 * @param[in] data          buffer containing the data to load
 * @param[in] len           number of bytes in @p buffer
 *
 * @return                  offset + number of bytes written
 */
size_t cc2420_tx_load(cc2420_t *dev, uint8_t *data, size_t len);

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 */
void cc2420_tx_exec(cc2420_t *dev);

/**
 * @brief   Read the length of a received packet
 *
 * @param dev               device to read from
 *
 * @return                  overall length of a received packet in byte
 */
size_t cc2420_rx_len(cc2420_t *dev);

/**
 * @brief   Read a chunk of data from the receive buffer of the given device
 *
 * @param[in]  dev          device to read from
 * @param[out] data         buffer to write data to
 * @param[in]  len          number of bytes to read from device
 * @param[in]  offset       offset in the receive buffer
 */
void cc2420_rx_read(cc2420_t *dev, uint8_t *data, size_t len,
                       size_t offset);
/************************* L3NKO ****************************/

#ifdef __cplusplus
}
#endif

#endif /* CC2420_INTERNAL_H_ */
/** @} */
