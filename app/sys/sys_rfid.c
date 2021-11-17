/**
 * @file       sys_nfc.c
 * @copyright  Copyright (C) 2020  Ltd. All rights reserved.
 * @license    This project is released under the License.
 * @version    3.0.0
 * @date       2020-07-20 
 * @author     Thuan Le - v1.0.0
 * @author     Khang Vo - v2.0.0
 * @brief      System module to handle NFC operations
 * @note       None
 * @example    None
 */
/* Includes ----------------------------------------------------------------- */
#include "rfal_nfc.h"
#include "ndef_poller.h"
#include "ndef_t2t.h"
#include "ndef_t4t.h"
#include "ndef_t5t.h"
#include "ndef_message.h"
#include "ndef_types_rtd.h"
#include "ndef_dump.h"

#include "sys_nfc.h"

/* Private enum/structs ----------------------------------------------------- */
/* Private defines ---------------------------------------------------------- */
#define NFC_TASK_STACK_SIZE          (4096 / sizeof(StackType_t))
#define NFC_TASK_PRIORITY            (3)
#define NFC_NDEF_BUF_SIZE            (256)
#define NFC_USE_NDEF                 (true)
#define NFC_CHECK_PRESENCE_PERIOD    (200)    // Unit: 200 (ms)
#define NFC_CHECK_PRESENCE_CNT       (1000 / NFC_CHECK_PRESENCE_PERIOD)
#define UNUSED(x)                    (void)(x)

// NFC default discovery parameters
#define NFC_DFLT_DISC_PARAMS                       \
{                                                  \
  .compMode            = RFAL_COMPLIANCE_MODE_NFC, \
  .devLimit            = 1U,                       \
  .nfcfBR              = RFAL_BR_212,              \
  .ap2pBR              = RFAL_BR_424,              \
  .maxBR               = RFAL_BR_KEEP,             \
  .notifyCb            = NULL,                     \
  .totalDuration       = 100U,                     \
  .wakeupEnabled       = false,                    \
  .wakeupConfigDefault = true,                     \
  .techs2Find          = RFAL_NFC_POLL_TECH_A      \
}

static const char *TAG = "sys/nfc";
const rfalNfcDiscoverParam NFC_DISC_PARAMS = NFC_DFLT_DISC_PARAMS;

/* Private variables -------------------------------------------------------- */
static uint8_t m_ndef_buf[NFC_NDEF_BUF_SIZE];

/* Private function prototypes ---------------------------------------------- */
static bool m_ndef_read(rfalNfcDevice *p_nfc_dev, ndefMessage *message);
static bool m_tag_valid(const uint8_t *p_data, uint32_t len);

/* Public variables --------------------------------------------------- */
/* Function definitions ----------------------------------------------------- */
bool sys_nfc_init(void)
{
  ReturnCode err;

#ifdef SELFTEST
  esp_log_level_set("spi_master", ESP_LOG_NONE); // Filter SPI driver logs
#else
  esp_log_level_set("spi_master", ESP_LOG_WARN); // Filter SPI driver logs
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif

  err = rfalNfcInitialize();
  if (ERR_NONE != err)
  {
    return false;
  }

  rfalNfcDeactivate(false);
  rfalNfcDiscover(&NFC_DISC_PARAMS);
  xTaskCreate(sys_nfc_task, "nfc_task", NFC_TASK_STACK_SIZE, NULL, NFC_TASK_PRIORITY, NULL);

  return true;
}

void sys_nfc_task(void *p_param)
{
  static rfalNfcDevice   *p_nfc_dev;
  static rfalNfcaSensRes  sens_res;
  static sys_nfc_tag_t    nfc_evt;
  static ndefMessage      msg;

  while (1)
  {
    // Run RFAL worker periodically
    rfalNfcWorker();

    if (!rfalNfcIsDevActivated(rfalNfcGetState()))
    {
      vTaskDelay(pdMS_TO_TICKS(50));
      continue;
    }

    rfalNfcGetActiveDevice(&p_nfc_dev);

    // NDEF message detected 
    if (m_ndef_read(p_nfc_dev, &msg))
    {
      ESP_LOGI(TAG, "UID: %s\r\n", hex2Str(p_nfc_dev->nfcid, p_nfc_dev->nfcidLen));

      // Event: TAG_PUT
      memcpy(&nfc_evt.uid, p_nfc_dev->nfcid, p_nfc_dev->nfcidLen);
      memcpy(&nfc_evt.message, &msg, sizeof(ndefMessage));
      xQueueSend(g_sys_evt_queue, (void *) &nfc_evt, pdMS_TO_TICKS(100));
      xEventGroupSetBits(g_sys_evt_group, SYS_NFC_PLACED_BIT);

      // Loop until tag is removed from the field 
      uint8_t tag_not_presence_count = 0;

      rfalNfcaPollerSleep();  // IMPORTANT: DO NOT REMOVE
      rfalNfcaPollerInitialize();
      while (1)
      {
        if (rfalNfcaPollerCheckPresence(RFAL_14443A_SHORTFRAME_CMD_WUPA, &sens_res) == ERR_NONE)
        {
          rfalNfcaPollerSleep();
          tag_not_presence_count = 0;
        }
        else
        {
          tag_not_presence_count++;
          ESP_LOGI(TAG, "TAG NOT FOUND: %d", tag_not_presence_count);
          if (tag_not_presence_count >= NFC_CHECK_PRESENCE_CNT)
          {
            tag_not_presence_count = 0;
            break;
          }
        }

        vTaskDelay(pdMS_TO_TICKS(NFC_CHECK_PRESENCE_PERIOD));
      }

      // Event: TAG_REMOVED
      nfc_evt.uid = 0;
      xQueueSend(g_sys_evt_queue, (void *) &nfc_evt, pdMS_TO_TICKS(100));
      xEventGroupClearBits(g_sys_evt_group, SYS_NFC_PLACED_BIT);
    }

    rfalNfcDeactivate(false);
    rfalNfcDiscover(&NFC_DISC_PARAMS);
  }
}

/* Private function --------------------------------------------------------- */
/**
 * @brief         NDEF read data
 * 
 * @param[in]     p_nfc_dev     Pointer to NFC device
 * 
 * @attention     None
 * 
 * @return
 *  - true:   Valid NDEF message
 *  - false:  Invalid NDEF message
 */
static bool m_ndef_read(rfalNfcDevice *p_nfc_dev, ndefMessage *p_msg)
{
#ifndef SELFTEST
  static ndefContext  ndef_ctx;
  static ndefInfo     ndef_info;
  uint32_t            raw_msg_len;
  ndefConstBuffer     p_raw_buf;

#ifdef NFC_USE_NDEF
  // Perform NDEF Context Initialization
  CHECK(ERR_NONE == ndefPollerContextInitialization(&ndef_ctx, p_nfc_dev), false);

  // Perform NDEF Detect procedure
  CHECK(ERR_NONE == ndefPollerNdefDetect(&ndef_ctx, &ndef_info), false);

  // Read raw message
  CHECK(ERR_NONE == ndefPollerReadRawMessage(&ndef_ctx, m_ndef_buf, sizeof(m_ndef_buf), &raw_msg_len), false);

  // Exception: NDEF message is too large
  if (raw_msg_len > sizeof(m_ndef_buf))
  {
    return false;
  }

  // Decode message
  p_raw_buf.buffer = m_ndef_buf;
  p_raw_buf.length = raw_msg_len;
  CHECK(ERR_NONE == ndefMessageDecode(&p_raw_buf, p_msg), false);

  // Check NDEF message content
  return m_tag_valid(p_msg->record->bufPayload.buffer, p_msg->record->bufPayload.length);

#else
  CHECK(ERR_NONE == ndefPollerReadBytes(&ndef_ctx, 0, 144, &m_ndef_buf, &raw_msg_len), false);
  ESP_LOGI(TAG, "raw length: %d", raw_msg_len);
  ESP_LOG_BUFFER_HEXDUMP(TAG, m_ndef_buf, raw_msg_len, ESP_LOG_INFO);
#endif

#undef CHECK
#else
  UNUSED(m_tag_valid);
  UNUSED(m_ndef_buf);
  return true;
#endif
}

/**
 * @brief         Validate NDEF message
 * 
 * @param[in]     p_data  Pointer to raw message
 * @param[in]     len     Length of raw message
 * 
 * @attention     None
 * 
 * @return
 *  - true:   Valid NDEF message
 *  - false:  Invalid NDEF message
 */
static bool m_tag_valid(const uint8_t *p_data, uint32_t len)
{
  const char *TAG_ID = "scp:";
  int TAG_ID_LEN = strlen(TAG_ID);

  // Find tag ID
  for (int i = 0; i < len - TAG_ID_LEN; i++)
  {
    if (!strncmp((char *) &p_data[i], TAG_ID, TAG_ID_LEN))
    {
      // Dump message
      ESP_LOGI(TAG, "NDEF msg: %d\r\n%.*s\r\n", len, len, p_data);
      return true;
    }
  }
  return false;
}

/* End of file -------------------------------------------------------------- */
