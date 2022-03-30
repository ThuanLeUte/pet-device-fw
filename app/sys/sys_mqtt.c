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
#include "sys_wifi.h"
#include "sys_nvs.h"

/* Private defines ---------------------------------------------------------- */
#define MQTT_BROKER_ENDPOINT "mqtts://test.mosquitto.org"
/* Private Constants -------------------------------------------------------- */
static const char *TAG = "sys_mqtt";

static esp_mqtt_client_handle_t client;
static char aws_topic[100];

/* Private macros ----------------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------------- */
/* Private variables -------------------------------------------------------- */
static device_state_t mqtt_device_state = NORMAL;
static char * NFC_to_be_deleted;
static bool is_request_deleted = false;

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
  NFC_to_be_deleted = malloc(64U);
  client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, m_sys_mqtt_event_handler, client);
  esp_mqtt_client_start(client);
}

void sys_mqtt_publish(const char *topic, const char *data)
{
  ESP_LOGW(TAG, "MQTT publish     : %s", topic);
  ESP_LOGW(TAG, "MQTT publish data: %s", data);

  if (sys_wifi_is_connected())
    esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}

int sys_mqtt_subcribe(const char *topic)
{
  ESP_LOGW(TAG, "MQTT subcribe: %s", topic);

  if (sys_wifi_is_connected())
    return esp_mqtt_client_subscribe(client, topic, 0);

  return 0;
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
    // Remove retain message
    sprintf(aws_topic, "%s/down", g_nvs_setting_data.dev.qr_code);
    sys_mqtt_publish(aws_topic, "");
    msg_id = sys_mqtt_subcribe(aws_topic);

    sprintf(aws_topic, "%s/nfc_setting", g_nvs_setting_data.dev.qr_code);
    msg_id = sys_mqtt_subcribe(aws_topic);
    // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
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

  sprintf(aws_topic, "%s/nfc_setting", g_nvs_setting_data.dev.qr_code);

  if (strcmp(topic_name, aws_topic) == 0)
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
  else
  {
    sprintf(aws_topic, "%s/down", g_nvs_setting_data.dev.qr_code);

    if (strcmp(topic_name, aws_topic) == 0)
    {
      cJSON *parsed_json = cJSON_Parse(event->data);
      cJSON *type = cJSON_GetObjectItem(parsed_json, "type");
      cJSON *data = cJSON_GetObjectItem(parsed_json, "data");
      cJSON *data_req = cJSON_GetObjectItem(data, "req");
      cJSON *data_ncf = cJSON_GetObjectItem(data, "nfc_id");

      cJSON *request_key = cJSON_CreateString("req");
      cJSON *request_act = cJSON_CreateString("del_nfc");

      if (cJSON_Compare(type, request_key, true))
      {
        if (cJSON_Compare(data_req, request_act, true))
        {
          NFC_to_be_deleted = cJSON_GetStringValue(data_ncf);
          is_request_deleted = true;
        }
      }
    }
    else
    {
    }
  }
}

bool sys_mqtt_is_request_deleted() {
  const bool tmp_is_request_deleted = is_request_deleted;
  is_request_deleted = false;
  return tmp_is_request_deleted;
}

char *sys_mqtt_get_nfc_tobe_deleted() {
  return NFC_to_be_deleted;
}