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
#define MAX_HEX_STR           (4)
#define MAX_HEX_STR_LENGTH    (128)
#define LOG_BUFFER_SIZE       (256)

/* Public variables --------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */
static const char *TAG = "BSP";
static spi_device_handle_t  m_spi_hdl;

/* Private function prototypes ---------------------------------------------- */
static inline void m_bsp_nvs_init(void);
static inline void m_bsp_spiffs_init(void);
static inline void bsp_spi_init(void);

/* Function definitions ----------------------------------------------------- */
void bsp_init(void)
{
  m_bsp_nvs_init();
  m_bsp_spiffs_init();
  bsp_spi_init();
}

void bsp_spi_transmit_receive(uint8_t *tx_data, uint8_t *rx_data, uint16_t len)
{
  esp_err_t ret;
  spi_transaction_t t;

  if (0 == len)
    return;
  memset(&t, 0, sizeof(t)); // Zero out the transaction

  t.length    = len * 8;    // Len is in bytes, transaction length is in bits.
  t.tx_buffer = tx_data;    // Data
  t.user      = (void *)1;  // D/C needs to be set to 1
  t.rx_buffer = rx_data;

  ret = spi_device_transmit(m_spi_hdl, &t);

  assert(ret == ESP_OK); 
}

void bsp_log_data(const char *format, ...)
{
  char buf[LOG_BUFFER_SIZE];
  va_list argptr;

  va_start(argptr, format);
  vsnprintf(buf, LOG_BUFFER_SIZE, format, argptr);
  va_end(argptr);

  const char *TAG_NFC = "NFC";
  ESP_LOGI(TAG_NFC, "%s", buf);
}

char *hex2Str(unsigned char *data, size_t dataLen)
{
  static char hex_str[MAX_HEX_STR][MAX_HEX_STR_LENGTH];
  static uint8_t hex_str_idx = 0;

  unsigned char *pin  = data;
  const char    *hex  = "0123456789ABCDEF";
  char          *pout = hex_str[hex_str_idx];
  uint8_t       i     = 0;
  uint8_t       idx   = hex_str_idx;

  if (dataLen > (MAX_HEX_STR_LENGTH / 2))
  {
    dataLen = (MAX_HEX_STR_LENGTH / 2);
  }

  if (dataLen == 0)
  {
    pout[0] = 0;
  }
  else
  {
    for (; i < dataLen - 1; ++i)
    {
      *pout++ = hex[(*pin >> 4) & 0xF];
      *pout++ = hex[(*pin++) & 0xF];
    }
    *pout++ = hex[(*pin >> 4) & 0xF];
    *pout++ = hex[(*pin) & 0xF];
    *pout   = 0;
  }

  hex_str_idx++;
  hex_str_idx %= MAX_HEX_STR;

  return hex_str[idx];
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

static inline void bsp_spi_init(void)
{
  esp_err_t ret;

  spi_bus_config_t bus_cfg =
  {
    .miso_io_num   = SPI_MISO_PIN,
    .mosi_io_num   = SPI_MOSI_PIN,
    .sclk_io_num   = SPI_SCLK_PIN,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1
  };

  spi_device_interface_config_t dev_cfg =
  {
    .clock_speed_hz = 1000000,   // Clock out at 1 MHz
    .mode           = 0,         // SPI mode 0
    .spics_io_num   = -1,        // CS pin
    .queue_size     = 7,         // We want to be able to queue 7 transactions at a time
  };

  // Initialize the SPI bus
  ret = spi_bus_initialize(HSPI_HOST, &bus_cfg, 1);
  assert(ret == ESP_OK);

  // Attach the LCD to the SPI bus
  ret = spi_bus_add_device(HSPI_HOST, &dev_cfg, &m_spi_hdl);
  assert(ret == ESP_OK);
}

/* End of file -------------------------------------------------------- */