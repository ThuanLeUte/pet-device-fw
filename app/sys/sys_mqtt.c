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
#include "sys_mqtt.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include <cJSON.h>
#include "sys_mqtt.h"

/* Private defines ---------------------------------------------------------- */
#define MQTT_BROKER_ENDPOINT "mqtts://test.mosquitto.org"
/* Private Constants -------------------------------------------------------- */
static const char *TAG = "sys_mqtt";

static esp_mqtt_client_handle_t client;

/* Private macros ----------------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------------- */
/* Private variables -------------------------------------------------------- */
static device_state_t mqtt_device_state = NORMAL;
/* Public variables --------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
static void m_sys_mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static esp_err_t m_sys_mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
static void m_sys_mqtt_event_receive_callback(esp_mqtt_event_handle_t event);

/* Function definitions ----------------------------------------------------- */
void sys_mqtt_init(void)
{
  esp_mqtt_client_config_t mqtt_cfg = {
      .uri = "mqtt://test.mosquitto.org",
  };

  client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, m_sys_mqtt_event_handler, client);
  esp_mqtt_client_start(client);
}

void sys_mqtt_publish(const char *topic, const char *data)
{
  esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}

int sys_mqtt_subcribe(const char *topic)
{
  return esp_mqtt_client_subscribe(client, topic, 0);
}

device_state_t sys_mqtt_get_state()
{
  return mqtt_device_state;
}

/* Private function --------------------------------------------------------- */
/**
 * @brief         Ready state machine
 *
 * @param[in]     None
 *
 * @attention     None
 *
 * @return        None
 */
static esp_err_t m_sys_mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
  int msg_id;

  switch (event->event_id)
  {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

    msg_id = sys_mqtt_subcribe("Device_3/nfc_setting");
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
    break;

  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
    break;

  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
    break;

  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
    break;

  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
    break;

  case MQTT_EVENT_DATA:
    ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    m_sys_mqtt_event_receive_callback(event);

    break;

  case MQTT_EVENT_ERROR:
    ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
    break;

  default:
    ESP_LOGI(TAG, "Other event id:%d", event->event_id);
    break;
  }
  return ESP_OK;
}

static void m_sys_mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
  m_sys_mqtt_event_handler_cb(event_data);
}

static void m_sys_mqtt_event_receive_callback(esp_mqtt_event_handle_t event)
{
  char *topic_name;

  topic_name = malloc(event->topic_len + 1);
  snprintf(topic_name, event->topic_len + 1, "%s", event->topic);

  if (strcmp(topic_name, "Device_3/nfc_setting") == 0)
  {
    cJSON *parsed_json = cJSON_Parse(event->data);
    cJSON *type = cJSON_GetObjectItem(parsed_json, "type");
    cJSON *value = cJSON_GetObjectItem(parsed_json, "value");
    cJSON *request_key = cJSON_CreateString("request");

    // ESP_LOGI(TAG, "Data is %s", cJSON_GetStringValue(parsed_json));

    if (cJSON_Compare(type, request_key, true))
    {
      if (cJSON_IsTrue(value))
        mqtt_device_state = NFC_SETTING;
      else
        mqtt_device_state = NORMAL;

      ESP_LOGI(TAG, "TOPIC=%.*s\r\n", event->topic_len, event->topic);
      ESP_LOGI(TAG, "DATA=%.*s\r\n", event->data_len, event->data);
    }
  }
}