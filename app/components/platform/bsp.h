/**
* @file       bsp.h
* @copyright  Copyright (C) 2021 ThuanLe. All rights reserved.
* @license    This project is released under the ThuanLe License.
* @version    01.00.00
* @date       2021-03-13
* @author     ThuanLe
* @brief      BSP (Board Support Package)
* @note       None
* @example    None
*/

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef BSP_H
#define BSP_H

/* Includes ----------------------------------------------------------- */
#include "platform_common.h"

/* Public defines ----------------------------------------------------- */
#define SPI_SS_PIN              (0)   // NFC
#define SPI_MOSI_PIN            (0)   // NFC
#define SPI_MISO_PIN            (0)   // NFC
#define SPI_SCLK_PIN            (0)   // NFC

#define IRQ_OUT_PIN             (-1)    // NFC
#define IRQ_IN_PIN              (-1)   // NFC

/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief         Bsp init
 * 
 * @param[in]     None
 * 
 * @attention     None
 * 
 * @return        None
 */
void bsp_init(void);
/**
 * @brief         Spi transmit and receive
 * @param[in]     <tx_data>     Pointer to transmit data
 *                <rx_data>     Pointer to receive data
 *                <len>         Transmit data length
 *
 * @attention     None
 * @return        None
 */
void bsp_spi_transmit_receive(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len);

/**
 * @brief         Logging data
 * @param[in]     <format>      Pointer to format data
 *
 * @attention     None
 * @return        None
 */
void bsp_log_data(const char *format, ...);


void interrupt_init(void);

#endif /* BSP_H */

/* End of file -------------------------------------------------------- */