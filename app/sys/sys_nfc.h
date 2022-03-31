/**
 * @file       sys_nfc.h
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2021-03-18
 * @author     Thuan Le
 * @brief      System module to handle NFC operations
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYS_NFC_H
#define __SYS_NFC_H

/* Includes ----------------------------------------------------------- */
#include "platform_common.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
extern TaskHandle_t g_nfc_task_handle;

/* Public function prototypes ----------------------------------------- */
/**
 * @brief         NFC application init
 * 
 * @param[in]     None
 * 
 * @attention     None
 * 
 * @return
 *     - true:   Init success
 *     - false:  Init fail
 */
bool sys_nfc_init(void);

void sys_nfc_deinit(void);

#endif /* __SYS_NFC_H */

/* End of file -------------------------------------------------------- */