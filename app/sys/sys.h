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
#define SYS_DEVCFG_DONE_EVT               BIT1

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
extern EventGroupHandle_t g_sys_evt_group;

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

/**
 * @brief System event group set
 */
void sys_event_group_set(const EventBits_t bit_to_set);

/**
 * @brief System event group clear
 */
void sys_event_group_clear(const EventBits_t bit_to_clear);

#endif /* SYSTEM_H */

/* End of file -------------------------------------------------------- */