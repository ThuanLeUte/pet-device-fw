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
/* Private Constants -------------------------------------------------------- */
static const char *TAG = "sys";
EventGroupHandle_t g_sys_evt_group;
uint16_t m_reset_cnt = 0;

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
  if (gpio_get_level(IO_KEY_B_PIN) == 0)
  {
    ESP_LOGI(TAG, "Enter Wifi setting");
    sys_wifi_erase_config();
    esp_restart();
  }

  vTaskDelay(pdMS_TO_TICKS(2000));

  // WORKAROUND: Reinit the NFC reader avoid read NFC faild
  m_reset_cnt++;
  if (m_reset_cnt == 6)
  {
    sys_nfc_deinit();
  }
  else if (m_reset_cnt == 7)
  {
    sys_nfc_init();
    m_reset_cnt = 0;
  }
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