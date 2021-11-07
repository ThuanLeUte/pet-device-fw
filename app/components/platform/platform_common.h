/**
* @file       platform_common.h
* @copyright  Copyright (C) 2021 ThuanLe. All rights reserved.
* @license    This project is released under the ThuanLe License.
* @version    01.00.00
* @date       2020-07-17
* @author     ThuanLe
* @brief      Platform common
* @note       None
* @example    None
*/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __PLATFORM_COMMON_H
#define __PLATFORM_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// FreeRTOS
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "freertos/xtensa_api.h"

// ESP32
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp32/clk.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------------- */
/* Public Constants --------------------------------------------------------- */
/* Public variables --------------------------------------------------------- */
/* Public macros ------------------------------------------------------------ */
#ifdef CHECK
#undef CHECK
#endif
#define CHECK(expr, ret)           \
  do {                             \
    if (!(expr)) {                 \
      ESP_LOGE(TAG, "%s", #expr);  \
      return (ret);                \
    }                              \
  } while (0)

/* Public APIs -------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
#ifdef __cplusplus
} // extern "C" {
#endif

#endif // __PLATFORM_COMMON_H

/* End of file -------------------------------------------------------------- */
