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

int bsp_uart_write(const char *data, uint16_t len);

int bsp_uart_read(char *data);

#endif /* BSP_H */

/* End of file -------------------------------------------------------- */