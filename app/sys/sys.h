/**
 * @file       sys.h
 * @copyright  Copyright (C) 2021 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2020-07-23
 * @author     Thuan Le
 * @brief      System module to handle sytem state
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef SYSTEM_H
#define SYSTEM_H

/* Includes ----------------------------------------------------------- */
#include "platform_common.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief         System boot
 * 
 * @param[in]     None
 * 
 * @attention     None
 * 
 * @return        None
 */
void sys_boot(void);

/**
 * @brief         System run
 * 
 * @param[in]     None
 * 
 * @attention     None
 * 
 * @return        None
 */
void sys_run(void);

#endif /* SYSTEM_H */

/* End of file -------------------------------------------------------- */