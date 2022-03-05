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
#ifndef _SYS_MQTT_H
#define _SYS_MQTT_H

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

void sys_mqtt_init(void);
void sys_mqtt_publish(const char *topic, const char *data, uint16_t len);
void sys_mqtt_subcribe(const char *topic);

#endif /* _SYS_MQTT_H */

/* End of file -------------------------------------------------------- */