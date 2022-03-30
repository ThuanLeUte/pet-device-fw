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
#include "bsp_io_11.h"
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
EventGroupHandle_t g_sys_evt_group;

/* Private macros ----------------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------------- */
/* Private variables -------------------------------------------------------- */
/* Public variables --------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
static void m_sys_evt_group_init(void);

/* Function definitions ----------------------------------------------------- */
void sys_boot(void)
{
  sys_nvs_init();

  bsp_init();

  m_sys_evt_group_init();

  sys_wifi_init();
  if (sys_wifi_is_configured())
  {
    sys_nfc_init();
    sys_wifi_connect();
  }
  else
  {
    sys_devcfg_init();
  }
}

void sys_run(void)
{
  // // Check network config
  // EventBits_t evt_bit = xEventGroupWaitBits(g_sys_evt_group, SYS_DEVCFG_DONE_EVT, true, true, pdMS_TO_TICKS(100));
  // if (evt_bit & SYS_DEVCFG_DONE_EVT)
  // {
  //   ESP_LOGE(TAG, "Device config successfull");

  //   // Disable bluetooth
  //   esp_blufi_profile_deinit();
  //   esp_bluedroid_disable();
  //   esp_bluedroid_deinit();
  //   esp_bt_controller_disable();
  //   esp_bt_controller_deinit();

  //   // Update wifi event handler
  //   sys_wifi_update_event_handler();

  //   sys_mqtt_init();
  // }

  if (gpio_get_level(IO_KEY_B_PIN) == 1)
  {
    ESP_LOGI(TAG, "Enter Wifi setting");
    sys_wifi_erase_config();
    esp_restart();
  }

  vTaskDelay(pdMS_TO_TICKS(1000));
}

void sys_event_group_set(const EventBits_t bit_to_set)
{
  if (g_sys_evt_group != NULL)
    xEventGroupSetBits(g_sys_evt_group, bit_to_set);
}

void sys_event_group_clear(const EventBits_t bit_to_clear)
{
  if (g_sys_evt_group != NULL)
    xEventGroupClearBits(g_sys_evt_group, bit_to_clear);
}

/* Private function --------------------------------------------------------- */
/**
 * @brief System event group init
 */
static void m_sys_evt_group_init(void)
{
  g_sys_evt_group = xEventGroupCreate();
}

/* End of file -------------------------------------------------------- */