/**
* @file       bsp.c
* @copyright  Copyright (C) 2021 ThuanLe. All rights reserved.
* @license    This project is released under the ThuanLe License.
* @version    01.00.00
* @date       2021-03-13
* @author     ThuanLe
* @brief      BSP (Board Support Package)
* @note       None
* @example    None
*/

/* Includes ----------------------------------------------------------------- */
#include "bsp.h"

/* Private defines ---------------------------------------------------------- */
static const char *TAG = "BSP";

#define TXD_PIN         (GPIO_NUM_17)
#define RXD_PIN         (GPIO_NUM_16)
#define RX_BUF_SIZE     (2000)

/* Public variables --------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
static inline void m_bsp_nvs_init(void);
static inline void m_bsp_spiffs_init(void);

/* Function definitions ----------------------------------------------------- */
void bsp_init(void)
{
  m_bsp_nvs_init();
  m_bsp_spiffs_init();
}

/* Private function --------------------------------------------------------- */
static inline void m_bsp_nvs_init(void)
{
  esp_err_t ret = ESP_OK;

  ret = nvs_flash_init();
  if ((ESP_ERR_NVS_NO_FREE_PAGES == ret) || (ESP_ERR_NVS_NEW_VERSION_FOUND == ret))
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
  }
}

static inline void m_bsp_spiffs_init(void)
{
  esp_err_t ret = ESP_OK;
  ESP_LOGI(TAG, "Initializing SPIFFS");

  esp_vfs_spiffs_conf_t spiffs_init_cfg = 
  {
    .base_path              = "/spiffs",
    .partition_label        = NULL,
    .max_files              = 5,
    .format_if_mount_failed = true
  };
  ret = esp_vfs_spiffs_register(&spiffs_init_cfg);

  if (ESP_OK != ret)
  {
    ESP_LOGE(TAG, "SPIFFS init failed: %s", esp_err_to_name(ret));
    return;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(NULL, &total, &used);

  if (ESP_OK == ret)
  {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }
  else
  {
    ESP_LOGE(TAG, "SPIFFS get info failed: %s", esp_err_to_name(ret));
  }
}

/* End of file -------------------------------------------------------- */