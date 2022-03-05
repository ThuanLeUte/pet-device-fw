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
/* Public enumerate/structure ----------------------------------------------- */
typedef enum
{
  BSP_ERR_NONE

  //                                            |-------- 1: On-chip, 2: Off-chip
  //                                             ||------ 00 - 99: Peripheral type
  //                                               ||---- 00 - 99: Error code
  ,BSP_ERR_SD_INIT                            = 20000
  ,BSP_ERR_SD_COMMUNICATION                   = 20001
  
  ,BSP_ERR_IOE_INIT                           = 20100
  ,BSP_ERR_IOE_COMMUNICATION                  = 20101

  ,BSP_ERR_TEMPERATURE_INIT                   = 20200
  ,BSP_ERR_TEMPERATURE_COMMUNICATION          = 20201

  ,BSP_ERR_RTC_INIT                           = 20300
  ,BSP_ERR_RTC_COMMUNICATION                  = 20301

  ,BSP_ERR_NVS_INIT                           = 20400
  ,BSP_ERR_NVS_COMMUNICATION                  = 20401
}
bsp_error_t;

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

/**
 * @brief         This function is executed in case of error occurrence.
 * 
 * @param[in]     error    Error
 * 
 * @return        None
 */
void bsp_error_handler(bsp_error_t error);

void interrupt_init(void);

#endif /* BSP_H */

/* End of file -------------------------------------------------------- */