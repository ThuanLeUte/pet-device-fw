/**
 * @file       sys_wifi.c
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2021-03-18
 * @author     Thuan Le
 * @brief      System module to handle WiFi
 * @note       None
 * @example    None
 */

/* Includes ----------------------------------------------------------------- */
#include "sys_wifi.h"
#include "sys_devcfg.h"
#include "sys_mqtt.h"

/* Private enum/structs ----------------------------------------------------- */
static struct
{
  wifi_config_t config;
  bool is_connected;

} m_wifi =
{
  .is_connected = false,
};

/* Private defines ---------------------------------------------------------- */
static const char *TAG = "sys_wifi";

#define WIFI_MAX_STATION_NUM            (100)       // Max wifi stations store in flash

/* Private variables -------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
static esp_err_t m_sys_wifi_event_handler(void *ctx, system_event_t *event);
static void m_wifi_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data);
static bool m_sys_wifi_connect(void);

/* Function definitions ----------------------------------------------------- */
void sys_wifi_init(void)
{
  // Init TCP/IP stack
  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  // Init ESP WiFi
  wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &m_wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &m_wifi_event_handler, NULL));

  // Wifi ssid manager create
  g_ssid_manager = wifi_ssid_manager_create(WIFI_MAX_STATION_NUM);
}

void sys_wifi_connect(void)
{
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
}

void sys_wifi_update_event_handler(void)
{
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &m_wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &m_wifi_event_handler, NULL));
}

bool sys_wifi_is_connected(void)
{
  return m_wifi.is_connected;
}

void sys_wifi_set_connection_status(bool status)
{
  m_wifi.is_connected = status;
}

void sys_wifi_erase_config(void)
{
  // Delete network configs
  ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &m_wifi.config));

  memset(m_wifi.config.sta.ssid, 0, sizeof(m_wifi.config.sta.ssid));
  memset(m_wifi.config.sta.password, 0, sizeof(m_wifi.config.sta.password));

  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &m_wifi.config));
}

bool sys_wifi_is_configured(void)
{
  esp_wifi_get_config(ESP_IF_WIFI_STA, &m_wifi.config); // Get stored WiFi configs

  if (strlen((char *)m_wifi.config.sta.ssid) == 0)
    return false;
    
  return true;
}

/* Private function --------------------------------------------------------- */
/**
 * @brief         Wifi connect
 *
 * @param[in]     None
 *
 * @attention     None
 *
 * @return
 *    - true:  Get best config success
 *    - false: Get best config failed
 */
static bool m_sys_wifi_connect(void)
{
  wifi_config_t config = {0};

  wifi_ssid_manager_list_show(g_ssid_manager);

  if (ESP_OK == wifi_ssid_manager_get_best_config(g_ssid_manager, &config))
  {
    ESP_LOGW(TAG, "Selected SSID: %s (%s)", config.sta.ssid, config.sta.password);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &config);
    esp_wifi_connect();
    return true;
  }
  else
  {
    ESP_LOGE(TAG, "Can not get the WiFi station in the WiFi save list");
    esp_wifi_connect();
    return false;
  }
}

/**
 * @brief         Wifi event handler
 *
 * @param[in]     event           Pointer to event
 *
 * @attention     None
 *
 * @return        esp_err_t
 */
static esp_err_t m_sys_wifi_event_handler(void *ctx, system_event_t *event)
{
  switch (event->event_id)
  {
  case SYSTEM_EVENT_STA_START:
  {
    m_sys_wifi_connect();
    break;
  }
  case SYSTEM_EVENT_STA_GOT_IP:
  {
    m_wifi.is_connected = true;
    ESP_LOGI(TAG, "Connected to the AP");
    sys_mqtt_init();
    break;
  }
  case SYSTEM_EVENT_STA_DISCONNECTED:
  {
    m_wifi.is_connected = false;
    m_sys_wifi_connect(); // WORKAROUND: as ESP32 WiFi libs don't currently auto-reassociate
    break;
  }

  default:
    break;
  }

  return ESP_OK;
}

static void m_wifi_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    m_sys_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    m_sys_wifi_connect();
    ESP_LOGI(TAG, "Connect to the AP fail");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    sys_mqtt_init();
  }
}

/* End of file -------------------------------------------------------- */
