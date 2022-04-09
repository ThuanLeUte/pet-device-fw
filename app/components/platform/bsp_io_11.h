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

#if(_CONFIG_USING_DEVKIT)
#define IO_NFC_SPI_SS               (GPIO_NUM_33)   // NFC
#define IO_NFC_SPI_CLK              (GPIO_NUM_25)   // NFC
#define IO_NFC_SPI_MISO             (GPIO_NUM_27)   // NFC
#define IO_NFC_SPI_MOSI             (GPIO_NUM_26)   // NFC

#define IO_NFC_IRQ_IN_PIN           (GPIO_NUM_14)   // NFC
#else
#define IO_NFC_SPI_SS               (GPIO_NUM_14)   // NFC
#define IO_NFC_SPI_CLK              (GPIO_NUM_25)   // NFC
#define IO_NFC_SPI_MISO             (GPIO_NUM_32)   // NFC
#define IO_NFC_SPI_MOSI             (GPIO_NUM_33)   // NFC
#define IO_NFC_IRQ_IN_PIN           (GPIO_NUM_35)   // NFC

#define IO_MOTOR_CONTROL_PWM_PIN    (GPIO_NUM_26)
#define IO_MOTOR_CONTROL_DIG_PIN    (GPIO_NUM_27)

#define IO_LED_RED_PIN              (GPIO_NUM_23)
#define IO_LED_GREEN_PIN            (GPIO_NUM_21)
#define IO_LED_BLUE_PIN             (GPIO_NUM_22)

#define IO_KEY_A_PIN                (GPIO_NUM_19)
#define IO_KEY_B_PIN                (GPIO_NUM_18)
#endif

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
