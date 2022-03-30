/**
 * @file       sys.c
 * @copyright  Copyright (C) 2021 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2020-07-23
 * @author     Thuan Le
 * @brief      System module to handle system state
 * @note       None
 * @example    None
 */

/* Includes ----------------------------------------------------------------- */
#include "platform_common.h"
#include "bsp.h"
#include "sys_nfc.h"
#include "sys_mqtt.h"
#include "sys_nvs.h"
#include "sys_devcfg.h"
#include "sys_wifi.h"

/* Private defines ---------------------------------------------------------- */
#define EXAMPLE_ESP_WIFI_SSID "A6.11"
#define EXAMPLE_ESP_WIFI_PASS "Khongcomatkhau"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/* Private Constants -------------------------------------------------------- */
static const char *TAG = "sys";
static EventGroupHandle_t m_wifi_event_group;

/* Private macros ----------------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------------- */
/* Private variables -------------------------------------------------------- */
/* Public variables --------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
static void m_wifi_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data);
static void m_wifi_init_sta(void);

/* Function definitions ----------------------------------------------------- */

void sys_boot(void)
{
  sys_nvs_init();

  bsp_init();

  sys_nfc_init();

  sys_wifi_init();
  if (sys_wifi_is_configured())
  {
    sys_wifi_connect();
  }
  else
  {
    sys_devcfg_init();
  }
}

void sys_run(void)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}

/* Private function --------------------------------------------------------- */
/* End of file -------------------------------------------------------- */