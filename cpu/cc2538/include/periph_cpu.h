/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc2538
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

// The following are defines for the base address of the memories and
// peripherals on the top_s interface.
#define PKA_BASE                0x44004000  // PKA
#define PKA_RAM_BASE            0x44006000  // SRAM_PKA

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
