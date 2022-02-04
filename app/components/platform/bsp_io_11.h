/**
 * @file       bsp_io_11.h
 * @copyright  Copyright (C) 2020 Hydratech. All rights reserved.
 * @license    This project is released under the Hydratech License.
 * @version    1.0.0
 * @date       2021-01-23
 * @author     Thuan Le
 * @brief      Pin description for hardware version 1.1
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_IO_11_H
#define __BSP_IO_11_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------- */
#include "platform_common.h"

/* Public defines ----------------------------------------------------- */
                                    // Schematic
// PIN NAME PORT 0 ``````````````````````````````
#define IO_NFC_SPI_SS               (GPIO_NUM_8)    // NFC
#define IO_NFC_SPI_CLK              (GPIO_NUM_25)   // NFC
#define IO_NFC_SPI_MISO             (GPIO_NUM_32)   // NFC
#define IO_NFC_SPI_MOSI             (GPIO_NUM_33)   // NFC
#define IO_NFC_IRQ_OUT_PIN          (-1)            // NFC
#define IO_NFC_IRQ_IN_PIN           (-1)            // NFC

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/* -------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C"
#endif
#endif // __BSP_IO_11_H

/* End of file -------------------------------------------------------- */
