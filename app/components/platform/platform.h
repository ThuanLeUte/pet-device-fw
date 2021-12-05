/**
 * @file       platform.h
 * @copyright  Copyright (C) 2020 ThuanLe., Ltd. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2020-07-19
 * @author     Thuan Le
 * @brief      Platform header file. Defining platform independent functionality.
 * @note       None
 * @example    None
 */
 
/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __PLATFORM_NFC_H
#define __PLATFORM_NFC_H

/* Includes ----------------------------------------------------------- */
#include "bsp.h"

#include "st_errno.h"
#include "timer.h"

/* Public defines ----------------------------------------------------- */
#define ST25R391X_SS_PIN                       (SPI_SS_PIN)           /*!< GPIO pin used for ST25R391X SPI SS                               */
#define ST25R391X_SS_PORT                      (-1)                   /*!< GPIO port used for ST25R391X SPI SS port                         */

#define ST25R391X_IRQ_OUT_PIN                  (IRQ_OUT_PIN)          /*!< GPIO pin used for ST25R391X nIRQ_OUT                             */
#define ST25R391X_IRQ_OUT_PORT                 (-1)                   /*!< GPIO port used for ST25R391X nIRQ_OUT                            */

#define ST25R391X_IRQ_IN_PIN                   (IRQ_IN_PIN)           /*!< GPIO pin used for ST25R391X nIRQ_IN                              */
#define ST25R391X_IRQ_IN_PORT                  (-1)                   /*!< GPIO port used for ST25R391X nIRQ_IN                             */

#define PLATFORM_LED_PIN                       (-1)                   /*!< GPIO pin used for LED                                          */
#define PLATFORM_LED_PORT                      (-1)                   /*!< GPIO port used for LED                                         */

#define ST25R391X_TAGDETECT_DEF_CALIBRATION      0x7C                 /*!< Tag Detection Calibration default value                        */
#define ST25R391X_TAGDETECT_CALIBRATE            true                 /*!< False: use default value, True: call calibration procedure     */

/*
******************************************************************************
* RFAL FEATURES CONFIGURATION
******************************************************************************
*/
#define RFAL_FEATURE_LISTEN_MODE               (false)    /*!< Enable/Disable RFAL support for Listen Mode                               */
#define RFAL_FEATURE_WAKEUP_MODE               (false)    /*!< Enable/Disable RFAL support for the Wake-Up mode                          */

#define RFAL_FEATURE_NFCA                      (true)     /*!< Enable/Disable RFAL support for NFC-A (ISO14443A)                         */
#define RFAL_FEATURE_NFCB                      (false)    /*!< Enable/Disable RFAL support for NFC-B (ISO14443B)                         */
#define RFAL_FEATURE_NFCF                      (false)    /*!< Enable/Disable RFAL support for NFC-F (FeliCa)                            */
#define RFAL_FEATURE_NFCV                      (false)    /*!< Enable/Disable RFAL support for NFC-V (ISO15693)                          */

#define RFAL_FEATURE_T1T                       (false)    /*!< Enable/Disable RFAL support for T1T (Topaz)                               */
#define RFAL_FEATURE_T2T                       (true)     /*!< Enable/Disable RFAL support for T2T                                       */
#define RFAL_FEATURE_T4T                       (false)    /*!< Enable/Disable RFAL support for T4T                                       */
#define RFAL_FEATURE_ST25TB                    (false)    /*!< Enable/Disable RFAL support for ST25TB                                    */
#define RFAL_FEATURE_ST25xV                    (false)    /*!< Enable/Disable RFAL support for ST25TV/ST25DV                             */

#define RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG     (false)    /*!< Enable/Disable Analog Configs to be dynamically updated (RAM)             */
#define RFAL_FEATURE_DYNAMIC_POWER             (false)    /*!< Enable/Disable RFAL dynamic power support                                 */
#define RFAL_FEATURE_ISO_DEP                   (true)     /*!< Enable/Disable RFAL support for ISO-DEP (ISO14443-4)                      */
#define RFAL_FEATURE_ISO_DEP_POLL              (true)     /*!< Enable/Disable RFAL support for Poller mode (PCD) ISO-DEP (ISO14443-4)    */
#define RFAL_FEATURE_ISO_DEP_LISTEN            (false)    /*!< Enable/Disable RFAL support for Listen mode (PICC) ISO-DEP (ISO14443-4)   */
#define RFAL_FEATURE_NFC_DEP                   (true)     /*!< Enable/Disable RFAL support for NFC-DEP (NFCIP1/P2P)                      */

#define RFAL_FEATURE_ISO_DEP_IBLOCK_MAX_LEN    (256U)     /*!< ISO-DEP I-Block max length. Please use values as defined by rfalIsoDepFSx */
#define RFAL_FEATURE_NFC_DEP_BLOCK_MAX_LEN     (254U)     /*!< NFC-DEP Block/Payload length. Allowed values: 64, 128, 192, 254           */
#define RFAL_FEATURE_NFC_RF_BUF_LEN            (258U)     /*!< RF buffer length used by RFAL NFC layer                                   */

#define RFAL_FEATURE_ISO_DEP_APDU_MAX_LEN      (1024U)    /*!< ISO-DEP APDU max length.                                                  */
#define RFAL_FEATURE_NFC_DEP_PDU_MAX_LEN       (512U)     /*!< NFC-DEP PDU max length.                                                   */


#define platformLedOn( port, pin )             platformGpioSet(port, pin)         /*!< Turns the given LED On                            */
#define platformLedOff( port, pin )            platformGpioClear(port, pin)       /*!< Turns the given LED Off                           */
#define platformLedToogle( port, pin )         platformGpioToogle(port, pin)      /*!< Toogle the given LED                              */

#define platformGpioSet(port, pin)             gpio_set_level(pin, 1)             /*!< Turns the given GPIO High                         */
#define platformGpioClear(port, pin)           gpio_set_level(pin, 0)             /*!< Turns the given GPIO Low                          */
#define platformGpioIsHigh(port, pin)          (gpio_get_level(pin) == 1)         /*!< Checks if the given LED is High                   */
#define platformGpioIsLow(port, pin)           (gpio_get_level(pin) == 0)         /*!< Checks if the given LED is Low                    */

#define platformTimerCreate(t)                 timerCalculateTimer(t)             /*!< Create a timer with the given time (ms)           */
#define platformTimerIsExpired(timer)          timerIsExpired(timer)              /*!< Checks if the given timer is expired              */
#define platformDelay(t)                       vTaskDelay(pdMS_TO_TICKS(t))       /*!< Performs a delay for the given time (ms)          */
#define platformGetSysTick()                   xTaskGetTickCount()                /*!< Get System Tick ( 1 tick = 1 ms)                  */
#define platformLog(...)                       bsp_log_data(__VA_ARGS__)          /*!< Log  method                                       */
#define platformTimerDestroy( timer )
#define platformErrorHandle()

#if !(ST25R391X_INTERFACE_UART) /* ST25R391X_INTERFACE_SPI */
/*!< SPI SS\CS: Chip|Slave Select                */
#define platformSpiSelect()                    platformGpioClear(ST25R391X_SS_PORT, ST25R391X_SS_PIN)

/*!< SPI SS\CS: Chip|Slave Deselect              */
#define platformSpiDeselect()                  platformGpioSet(ST25R391X_SS_PORT, ST25R391X_SS_PIN)

/*!< SPI transceive                              */
#define platformSpiTxRx(txBuf, rxBuf, len)     bsp_spi_transmit_receive(txBuf, rxBuf, len)

#define platformUartTx(TxBuf, len)                        /*!< UART transceive                             */
#define platformUartRx(RxBuf, len)                        /*!< UART transceive                             */
#else /* ! ST25R391X_INTERFACE_SPI */
#define platformSpiSelect()                               /*!< SPI SS\CS: Chip|Slave Select                */
#define platformSpiDeselect()                             /*!< SPI SS\CS: Chip|Slave Deselect              */
#define platformSpiTxRx(txBuf, rxBuf, len)                /*!< SPI transceive                              */
#define platformUartTx(TxBuf, len)                        /*!< UART transceive                             */
#define platformUartRx(RxBuf, len)                        /*!< UART transceive                             */
#define platformUartTxIT(TxBuf, len)                      /*!< UART transceive                             */
#define platformUartRxIT(RxBuf, len)                      /*!< UART transceive                             */
#define platformUartReset()                               /*!< UART abort Tx/Rx and reset uart             */
#endif /* ST25R391X_INTERFACE_SPI */


/* Protect RFAL Worker/Task/Process from concurrent execution on multi thread platforms   */
#define platformProtectWorker()                            

/* Unprotect RFAL Worker/Task/Process from concurrent execution on multi thread platforms */
#define platformUnprotectWorker()                          

char *hex2Str(unsigned char *data, size_t dataLen);

#endif /* PLATFORM_H */

/* End of file -------------------------------------------------------- */