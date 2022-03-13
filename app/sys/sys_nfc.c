/**
 * @file       sys_nvs.c
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2021-03-18
 * @author     Thuan Le
 * @brief      System module to handle NFC operations
 * @note       None
 * @example    None
 */
/* Includes ----------------------------------------------------------------- */
#include "sys_nfc.h"
#include "platform_common.h"
#include "bsp.h"
#include "demo.h"
#include "st25r3911.h"

/* Private enum/structs ----------------------------------------------------- */
/* Private defines ---------------------------------------------------------- */
#define NFC_TASK_STACK_SIZE          (4096 / sizeof(StackType_t))
#define NFC_TASK_PRIORITY            (25)

static const char *TAG = "sys/nfc";

/* Private variables -------------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private function prototypes ---------------------------------------------- */
void sys_nfc_task(void *p_param);

/* Function definitions ----------------------------------------------------- */
bool sys_nfc_init(void)
{
  // Initialize RFAL
  rfalAnalogConfigInitialize();

  if (rfalInitialize() != ERR_NONE)
  {
    ESP_LOGI(TAG, "Init faild: %d", rfalInitialize());

    // Initialization failed - indicate on LEDs
    while (1)
    {
      platformDelay(500);
      st25r3911CheckChipID(NULL);
    }
  }

  ESP_LOGI(TAG, "Init ok");
  interrupt_init();

  xTaskCreatePinnedToCore(sys_nfc_task, "nfc_task", NFC_TASK_STACK_SIZE, NULL, NFC_TASK_PRIORITY, NULL, 1);

  return true;
}

void sys_nfc_task(void *p_param)
{
  while (1)
  {
    // Run RFAL Worker
    rfalWorker();

    // Run Demo Application
    demoCycle();

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/* Private function --------------------------------------------------------- */
/* End of file -------------------------------------------------------------- */
