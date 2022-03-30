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
#include "platform_common.h"
#include "bsp_io_11.h"
#include "st25r3911_interrupt.h"
#include "sys_nfc.h"

/* Private defines ---------------------------------------------------------- */
#define MAX_HEX_STR           (4)
#define MAX_HEX_STR_LENGTH    (128)
#define LOG_BUFFER_SIZE       (256)

#define mutex_lock(x)       while (xSemaphoreTake(x, portMAX_DELAY) != pdPASS);
#define mutex_unlock(x)     xSemaphoreGive(x)
#define mutex_create()      xSemaphoreCreateMutex()
#define mutex_destroy(x)    vSemaphoreDelete(x)

/* Public variables --------------------------------------------------------- */
TaskHandle_t g_gpio_task_handle;
static SemaphoreHandle_t _bus_spi_lock = NULL;

/* Private variables -------------------------------------------------------- */
static const char *TAG = "BSP";
static spi_device_handle_t  m_spi_hdl;

/* Private function prototypes ---------------------------------------------- */
static inline void m_bsp_nvs_init(void);
static inline void m_bsp_spiffs_init(void);
static inline void bsp_spi_init(void);
static inline void bsp_gpio_init(void);

/* Function definitions ----------------------------------------------------- */
void bsp_init(void)
{
  m_bsp_spiffs_init();
  bsp_gpio_init();
  bsp_spi_init();
}

void bsp_spi_transmit_receive(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len)
{
  esp_err_t ret;
  spi_transaction_t t;

  mutex_lock(_bus_spi_lock);

  if (0 == len)
    return;
  memset(&t, 0, sizeof(t)); // Zero out the transaction

  t.length = len * 8;    // Len is in bytes, transaction length is in bits.
  t.tx_buffer = tx_data; // Data
  t.user = (void *)1;    // D/C needs to be set to 1
  t.rx_buffer = rx_data;

  ret = spi_device_polling_transmit(m_spi_hdl, &t);

  assert(ret == ESP_OK);

  mutex_unlock(_bus_spi_lock);
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
    .miso_io_num   = IO_NFC_SPI_MISO,
    .mosi_io_num   = IO_NFC_SPI_MOSI,
    .sclk_io_num   = IO_NFC_SPI_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1
  };

  spi_device_interface_config_t dev_cfg =
  {
    .clock_speed_hz = 5000000,   // Clock out at 1 MHz
    .mode           = 1,         // SPI mode 0
    .spics_io_num   = -1,        // CS pin
    .queue_size     = 100,       // We want to be able to queue 7 transactions at a time
  };

  // Initialize the SPI bus
  ret = spi_bus_initialize(HSPI_HOST, &bus_cfg, 1);
  assert(ret == ESP_OK);

  // Attach the LCD to the SPI bus
  ret = spi_bus_add_device(HSPI_HOST, &dev_cfg, &m_spi_hdl);
  assert(ret == ESP_OK);

  if (_bus_spi_lock)
  {
    mutex_destroy(_bus_spi_lock);
  }
  _bus_spi_lock = mutex_create();
}

static xQueueHandle gpio_evt_queue = NULL;

void IRAM_ATTR gpio_isr_handler(void *arg)
{
  uint32_t gpio_num = (uint32_t)arg;
  xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
  uint32_t io_num;

  while (1)
  {
    if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
    {
      st25r3911Isr();
    }
  }
}

static inline void bsp_gpio_init(void)
{
  gpio_pad_select_gpio(IO_NFC_SPI_SS);
  gpio_pad_select_gpio(IO_MOTOR_CONTROL_DIG_PIN);
  gpio_pad_select_gpio(IO_LED_RED_PIN);
  gpio_pad_select_gpio(IO_LED_GREEN_PIN);
  gpio_pad_select_gpio(IO_LED_BLUE_PIN);

  gpio_set_direction(IO_NFC_SPI_SS, GPIO_MODE_OUTPUT);
  gpio_set_direction(IO_MOTOR_CONTROL_DIG_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(IO_LED_RED_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(IO_LED_GREEN_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(IO_LED_BLUE_PIN, GPIO_MODE_OUTPUT);

  gpio_set_level(IO_LED_RED_PIN, 0);
  gpio_set_level(IO_LED_BLUE_PIN, 0);
  gpio_set_level(IO_LED_GREEN_PIN, 0);
}

void interrupt_init(void)
{
  gpio_config_t io_conf = {};

  // interrupt of rising edge
  io_conf.intr_type = GPIO_INTR_POSEDGE;

  // bit mask of the pins, use GPIO4/5 here
  io_conf.pin_bit_mask = (1ULL << IO_NFC_IRQ_IN_PIN);

  // set as input mode
  io_conf.mode = GPIO_MODE_INPUT;

  // enable pull-up mode
  io_conf.pull_up_en = 0;
  io_conf.pull_down_en = 1;
  gpio_config(&io_conf);

  // create a queue to handle gpio event from isr
  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

  // install gpio isr service
  gpio_install_isr_service(0);

  // hook isr handler for specific gpio pin
  gpio_isr_handler_add(IO_NFC_IRQ_IN_PIN, gpio_isr_handler, (void *)IO_NFC_IRQ_IN_PIN);

  // start gpio task
  xTaskCreatePinnedToCore(gpio_task_example, "gpio_task_example", 2048, NULL, 3, &g_gpio_task_handle, 1);
}

void bsp_error_handler(bsp_error_t error)
{

}

void bsp_motor_splash_watter(void)
{
  gpio_set_level(IO_MOTOR_CONTROL_DIG_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(100));
  gpio_set_level(IO_MOTOR_CONTROL_DIG_PIN, 0);
}

/* End of file -------------------------------------------------------- */