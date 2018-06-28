/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2015, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */

#ifndef _SAMA5D28_
#define _SAMA5D28_

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#include "compiler.h"
#endif

/** \addtogroup SAMA5D28_definitions SAMA5D28 definitions
  This file defines all structures and symbols for SAMA5D28:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - PIO definitions
*/
/*@{*/

#ifdef __cplusplus
 extern "C" {
#endif

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#include <stdint.h>

typedef enum IRQn
{
  FIQ_IRQn             =  0, /**<  0 SAMA5D28 Fast Interrupt Request (FIQ) */
  PIT_IRQn             =  3, /**<  3 SAMA5D28 Periodic Interval Timer Interrupt (PIT) */
  WDT_IRQn             =  4, /**<  4 SAMA5D28 Watchdog timer Interrupt (WDT) */
  GMAC0_IRQn           =  5, /**<  5 SAMA5D28 Ethernet MAC (GMAC0) */
  XDMAC0_IRQn          =  6, /**<  6 SAMA5D28 DMA Controller 0 (XDMAC0) */
  XDMAC1_IRQn          =  7, /**<  7 SAMA5D28 DMA Controller 1 (XDMAC1) */
  ICM_IRQn             =  8, /**<  8 SAMA5D28 Integritry Check Monitor (ICM) */
  AES_IRQn             =  9, /**<  9 SAMA5D28 Advanced Enion Standard (AES) */
  AESB_IRQn            = 10, /**< 10 SAMA5D28 AES bridge (AESB) */
  TDES_IRQn            = 11, /**< 11 SAMA5D28 Triple Data Enion Standard (TDES) */
  SHA_IRQn             = 12, /**< 12 SAMA5D28 SHA Signature (SHA) */
  MPDDRC_IRQn          = 13, /**< 13 SAMA5D28 MPDDR controller (MPDDRC) */
  MATRIX1_IRQn         = 14, /**< 14 SAMA5D28 H32MX, 32-bit AHB Matrix (MATRIX1) */
  MATRIX0_IRQn         = 15, /**< 15 SAMA5D28 H64MX, 64-bit AHB Matrix (MATRIX0) */
  SECMOD_IRQn          = 16, /**< 16 SAMA5D28 SECMOD Interrupt (SECMOD) */
  HSMC_IRQn            = 17, /**< 17 SAMA5D28 Multi-bit ECC Interrupt (HSMC) */
  PIOA_IRQn            = 18, /**< 18 SAMA5D28 Parallel I/O Controller (PIOA) */
  FLEXCOM0_IRQn        = 19, /**< 19 SAMA5D28 FLEXCOM 0 (FLEXCOM0) */
  FLEXCOM1_IRQn        = 20, /**< 20 SAMA5D28 FLEXCOM 1 (FLEXCOM1) */
  FLEXCOM2_IRQn        = 21, /**< 21 SAMA5D28 FLEXCOM 2 (FLEXCOM2) */
  FLEXCOM3_IRQn        = 22, /**< 22 SAMA5D28 FLEXCOM 3 (FLEXCOM3) */
  FLEXCOM4_IRQn        = 23, /**< 23 SAMA5D28 FLEXCOM 4 (FLEXCOM4) */
  UART0_IRQn           = 24, /**< 24 SAMA5D28 UART 0 (UART0) */
  UART1_IRQn           = 25, /**< 25 SAMA5D28 UART 1 (UART1) */
  UART2_IRQn           = 26, /**< 26 SAMA5D28 UART 2 (UART2) */
  UART3_IRQn           = 27, /**< 27 SAMA5D28 UART 3 (UART3) */
  UART4_IRQn           = 28, /**< 28 SAMA5D28 UART 4 (UART4) */
  TWIHS0_IRQn          = 29, /**< 29 SAMA5D28 Two-Wire Interface 0 (TWIHS0) */
  TWIHS1_IRQn          = 30, /**< 30 SAMA5D28 Two-Wire Interface 1 (TWIHS1) */
  SDHC0_IRQn           = 31, /**< 31 SAMA5D28 Secured Data Memory Card Controller 0 (SDMMC0) */
  SDHC1_IRQn           = 32, /**< 32 SAMA5D28 Secured Data Memory Card Controller 1 (SDMMC1) */
  SPI0_IRQn            = 33, /**< 33 SAMA5D28 Serial Peripheral Interface 0 (SPI0) */
  SPI1_IRQn            = 34, /**< 34 SAMA5D28 Serial Peripheral Interface 1 (SPI1) */
  TC0_IRQn             = 35, /**< 35 SAMA5D28 Timer Counter 0 (ch. 0, 1, 2) (TC0) */
  TC1_IRQn             = 36, /**< 36 SAMA5D28 Timer Counter 1 (ch. 3, 4, 5) (TC1) */
  PWM_IRQn             = 38, /**< 38 SAMA5D28 Pulse Width Modulation Controller0 (ch. 0, 1, 2, 3) (PWM) */
  ADC_IRQn             = 40, /**< 40 SAMA5D28 Touch Screen ADC Controller (ADC) */
  UHPHS_IRQn           = 41, /**< 41 SAMA5D28 USB Host High Speed (UHPHS) */
  UDPHS_IRQn           = 42, /**< 42 SAMA5D28 USB Device High Speed (UDPHS) */
  SSC0_IRQn            = 43, /**< 43 SAMA5D28 Synchronous Serial Controller 0 (SSC0) */
  SSC1_IRQn            = 44, /**< 44 SAMA5D28 Synchronous Serial Controller 1 (SSC1) */
  LCDC_IRQn            = 45, /**< 45 SAMA5D28 LCD Controller (LCDC) */
  ISC_IRQn             = 46, /**< 46 SAMA5D28 Camera Interface (ISC) */
  TRNG_IRQn            = 47, /**< 47 SAMA5D28 True Random Number Generator (TRNG) */
  PDMIC_IRQn           = 48, /**< 48 SAMA5D28 Pulse Density Modulation Interface Controller (PDMIC) */
  IRQ_IRQn             = 49, /**< 49 SAMA5D28 AIC1 Interrupt Request (IRQ) */
  SFC_IRQn             = 50, /**< 50 SAMA5D28 Fuse Controller (SFC) */
  QSPI0_IRQn           = 52, /**< 52 SAMA5D28 QSPI 0 (QSPI0) */
  QSPI1_IRQn           = 53, /**< 53 SAMA5D28 QSPI 1 (QSPI1) */
  I2SC0_IRQn           = 54, /**< 54 SAMA5D28 Inter-IC Sound Controller 0 (I2SC0) */
  I2SC1_IRQn           = 55, /**< 55 SAMA5D28 Inter-IC Sound Controller 1 (I2SC1) */
  CLASSD_IRQn          = 59, /**< 59 SAMA5D28 Audio Class D amplifier (CLASSD) */
  SFR_IRQn             = 60, /**< 60 SAMA5D28 Special Function Register  (SFR) */
  SAIC_IRQn            = 61, /**< 61 SAMA5D28 Secured Advanced Interrupt Controller  (SAIC) */
  AIC_IRQn             = 62, /**< 62 SAMA5D28 Advanced Interrupt Controller  (AIC) */
  L2CC_IRQn            = 63, /**< 63 SAMA5D28 L2 Cache Controller (L2CC) */
  PIOB_IRQn            = 68, /**< 68 SAMA5D28  (PIOB) */
  PIOC_IRQn            = 69, /**< 69 SAMA5D28  (PIOC) */
  PIOD_IRQn            = 70, /**< 70 SAMA5D28  (PIOD) */
  SYSC_IRQn            = 74, /**< 74 SAMA5D28 System Controller Interrupt, RTC, RSTC, PMC (SYSC) */
  ACC_IRQn             = 75, /**< 75 SAMA5D28 Analog Comparator (ACC) */
  RXLP_IRQn            = 76, /**< 76 SAMA5D28 Uart Low Power (RXLP) */
  CHIPID_IRQn          = 78, /**< 78 SAMA5D28 Chip ID (CHIPID) */

  PERIPH_COUNT_IRQn    = 79  /**< Number of peripheral IDs */
} IRQn_Type;
#endif

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAMA5D28 */
/* ************************************************************************** */
/** \addtogroup SAMA5D28_api Peripheral Software API */
/*@{*/

#include "sama5d2/component/component_acc.h"
#include "sama5d2/component/component_adc.h"
#include "sama5d2/component/component_aes.h"
#include "sama5d2/component/component_aesb.h"
#include "sama5d2/component/component_aic.h"
#include "sama5d2/component/component_aximx.h"
#include "sama5d2/component/component_chipid.h"
#include "sama5d2/component/component_classd.h"
#include "sama5d2/component/component_flexcom.h"
#include "sama5d2/component/component_gmac.h"
#include "sama5d2/component/component_i2sc.h"
#include "sama5d2/component/component_icm.h"
#include "sama5d2/component/component_isc.h"
#include "sama5d2/component/component_l2cc.h"
#include "sama5d2/component/component_lcdc.h"
#include "sama5d2/component/component_matrix.h"
#include "sama5d2/component/component_mcan.h"
#include "sama5d2/component/component_mpddrc.h"
#include "sama5d2/component/component_pdmic.h"
#include "sama5d2/component/component_pio.h"
#include "sama5d2/component/component_pit.h"
#include "sama5d2/component/component_pmc.h"
#include "sama5d2/component/component_pwm.h"
#include "sama5d2/component/component_qspi.h"
#include "sama5d2/component/component_rstc.h"
#include "sama5d2/component/component_rtc.h"
#include "sama5d2/component/component_rxlp.h"
#include "sama5d2/component/component_sckc.h"
#include "sama5d2/component/component_sfc.h"
#include "sama5d2/component/component_sfr.h"
#include "sama5d2/component/component_sha.h"
#include "sama5d2/component/component_shdwc.h"
#include "sama5d2/component/component_smc.h"
#include "sama5d2/component/component_spi.h"
#include "sama5d2/component/component_ssc.h"
#include "sama5d2/component/component_tc.h"
#include "sama5d2/component/component_tdes.h"
#include "sama5d2/component/component_trng.h"
#include "sama5d2/component/component_twihs.h"
#include "sama5d2/component/component_uart.h"
#include "sama5d2/component/component_udphs.h"
#include "sama5d2/component/component_wdt.h"
#include "sama5d2/component/component_xdmac.h"
#include "component/component_secumod.h"
/*@}*/

/* ************************************************************************** */
/*   REGISTER ACCESS DEFINITIONS FOR SAMA5D28 */
/* ************************************************************************** */
/** \addtogroup SAMA5D28_reg Registers Access Definitions */
/*@{*/

#include "sama5d2/instance/instance_aximx.h"
#include "sama5d2/instance/instance_l2cc.h"
#include "sama5d2/instance/instance_lcdc.h"
#include "sama5d2/instance/instance_xdmac1.h"
#include "sama5d2/instance/instance_mpddrc.h"
#include "sama5d2/instance/instance_xdmac0.h"
#include "sama5d2/instance/instance_pmc.h"
#include "sama5d2/instance/instance_matrix0.h"
#include "sama5d2/instance/instance_aesb.h"
#include "sama5d2/instance/instance_qspi0.h"
#include "sama5d2/instance/instance_qspi1.h"
#include "sama5d2/instance/instance_sha.h"
#include "sama5d2/instance/instance_aes.h"
#include "sama5d2/instance/instance_spi0.h"
#include "sama5d2/instance/instance_ssc0.h"
#include "sama5d2/instance/instance_gmac.h"
#include "sama5d2/instance/instance_tc0.h"
#include "sama5d2/instance/instance_tc1.h"
#include "sama5d2/instance/instance_hsmc.h"
#include "sama5d2/instance/instance_pdmic.h"
#include "sama5d2/instance/instance_uart0.h"
#include "sama5d2/instance/instance_uart1.h"
#include "sama5d2/instance/instance_uart2.h"
#include "sama5d2/instance/instance_twihs0.h"
#include "sama5d2/instance/instance_pwm.h"
#include "sama5d2/instance/instance_sfr.h"
#include "sama5d2/instance/instance_flexcom0.h"
#include "sama5d2/instance/instance_flexcom1.h"
#include "sama5d2/instance/instance_saic.h"
#include "sama5d2/instance/instance_icm.h"
#include "sama5d2/instance/instance_rstc.h"
#include "sama5d2/instance/instance_shdwc.h"
#include "sama5d2/instance/instance_pit.h"
#include "sama5d2/instance/instance_wdt.h"
#include "sama5d2/instance/instance_sckc.h"
#include "sama5d2/instance/instance_rtc.h"
#include "sama5d2/instance/instance_rxlp.h"
#include "sama5d2/instance/instance_acc.h"
#include "sama5d2/instance/instance_sfc.h"
#include "sama5d2/instance/instance_i2sc0.h"
#include "sama5d2/instance/instance_mcan0.h"
#include "sama5d2/instance/instance_spi1.h"
#include "sama5d2/instance/instance_ssc1.h"
#include "sama5d2/instance/instance_uart3.h"
#include "sama5d2/instance/instance_uart4.h"
#include "sama5d2/instance/instance_flexcom2.h"
#include "sama5d2/instance/instance_flexcom3.h"
#include "sama5d2/instance/instance_flexcom4.h"
#include "sama5d2/instance/instance_trng.h"
#include "sama5d2/instance/instance_aic.h"
#include "sama5d2/instance/instance_twihs1.h"
#include "sama5d2/instance/instance_udphs.h"
#include "sama5d2/instance/instance_adc.h"
#include "sama5d2/instance/instance_pioa.h"
#include "sama5d2/instance/instance_matrix1.h"
#include "sama5d2/instance/instance_tdes.h"
#include "sama5d2/instance/instance_classd.h"
#include "sama5d2/instance/instance_i2sc1.h"
#include "sama5d2/instance/instance_mcan1.h"
#include "sama5d2/instance/instance_chipid.h"
/*@}*/

/* ************************************************************************** */
/*   BASE ADDRESS DEFINITIONS FOR SAMA5D28 */
/* ************************************************************************** */
/** \addtogroup SAMA5D28_base Peripheral Base Address Definitions */
/*@{*/

#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define AXIMX    (0x00600000U) /**< \brief (AXIMX   ) Base Address */
#define L2CC     (0x00A00000U) /**< \brief (L2CC    ) Base Address */
#define LCDC     (0xF0000000U) /**< \brief (LCDC    ) Base Address */
#define XDMAC1   (0xF0004000U) /**< \brief (XDMAC1  ) Base Address */
#define MPDDRC   (0xF000C000U) /**< \brief (MPDDRC  ) Base Address */
#define XDMAC0   (0xF0010000U) /**< \brief (XDMAC0  ) Base Address */
#define PMC      (0xF0014000U) /**< \brief (PMC     ) Base Address */
#define MATRIX0  (0xF0018000U) /**< \brief (MATRIX0 ) Base Address */
#define AESB     (0xF001C000U) /**< \brief (AESB    ) Base Address */
#define QSPI0    (0xF0020000U) /**< \brief (QSPI0   ) Base Address */
#define QSPI1    (0xF0024000U) /**< \brief (QSPI1   ) Base Address */
#define SHA      (0xF0028000U) /**< \brief (SHA     ) Base Address */
#define AES      (0xF002C000U) /**< \brief (AES     ) Base Address */
#define SPI0     (0xF8000000U) /**< \brief (SPI0    ) Base Address */
#define SSC0     (0xF8004000U) /**< \brief (SSC0    ) Base Address */
#define GMAC0    (0xF8008000U) /**< \brief (GMAC    ) Base Address */
#define GMAC     (GMAC0)
#define TC0      (0xF800C000U) /**< \brief (TC0     ) Base Address */
#define TC1      (0xF8010000U) /**< \brief (TC1     ) Base Address */
#define HSMC     (0xF8014000U) /**< \brief (HSMC    ) Base Address */
#define PDMIC    (0xF8018000U) /**< \brief (PDMIC   ) Base Address */
#define UART0    (0xF801C000U) /**< \brief (UART0   ) Base Address */
#define UART1    (0xF8020000U) /**< \brief (UART1   ) Base Address */
#define UART2    (0xF8024000U) /**< \brief (UART2   ) Base Address */
#define TWIHS0   (0xF8028000U) /**< \brief (TWIHS0  ) Base Address */
#define PWM      (0xF802C000U) /**< \brief (PWM     ) Base Address */
#define SFR      (0xF8030000U) /**< \brief (SFR     ) Base Address */
#define FLEXCOM0 (0xF8034000U) /**< \brief (FLEXCOM0) Base Address */
#define FLEXCOM1 (0xF8038000U) /**< \brief (FLEXCOM1) Base Address */
#define SAIC     (0xF803C000U) /**< \brief (SAIC    ) Base Address */
#define ICM      (0xF8040000U) /**< \brief (ICM     ) Base Address */
#define RSTC     (0xF8048000U) /**< \brief (RSTC    ) Base Address */
#define SHDWC    (0xF8048010U) /**< \brief (SHDWC   ) Base Address */
#define PIT      (0xF8048030U) /**< \brief (PIT     ) Base Address */
#define WDT      (0xF8048040U) /**< \brief (WDT     ) Base Address */
#define SCKC     (0xF8048050U) /**< \brief (SCKC    ) Base Address */
#define RTC      (0xF80480B0U) /**< \brief (RTC     ) Base Address */
#define RXLP     (0xF8049000U) /**< \brief (RXLP    ) Base Address */
#define ACC      (0xF804A000U) /**< \brief (ACC     ) Base Address */
#define SFC      (0xF804C000U) /**< \brief (SFC     ) Base Address */
#define I2SC0    (0xF8050000U) /**< \brief (I2SC0   ) Base Address */
#define MCAN0    (0xF8054000U) /**< \brief (MCAN0   ) Base Address */
#define SPI1     (0xFC000000U) /**< \brief (SPI1    ) Base Address */
#define SSC1     (0xFC004000U) /**< \brief (SSC1    ) Base Address */
#define UART3    (0xFC008000U) /**< \brief (UART3   ) Base Address */
#define UART4    (0xFC00C000U) /**< \brief (UART4   ) Base Address */
#define FLEXCOM2 (0xFC010000U) /**< \brief (FLEXCOM2) Base Address */
#define FLEXCOM3 (0xFC014000U) /**< \brief (FLEXCOM3) Base Address */
#define FLEXCOM4 (0xFC018000U) /**< \brief (FLEXCOM4) Base Address */
#define TRNG     (0xFC01C000U) /**< \brief (TRNG    ) Base Address */
#define AIC      (0xFC020000U) /**< \brief (AIC     ) Base Address */
#define TWIHS1   (0xFC028000U) /**< \brief (TWIHS1  ) Base Address */
#define UDPHS    (0xFC02C000U) /**< \brief (UDPHS   ) Base Address */
#define ADC      (0xFC030000U) /**< \brief (ADC     ) Base Address */
#define PIOA     (0xFC038000U) /**< \brief (PIOA    ) Base Address */
#define MATRIX1  (0xFC03C000U) /**< \brief (MATRIX1 ) Base Address */
#define TDES     (0xFC044000U) /**< \brief (TDES    ) Base Address */
#define CLASSD   (0xFC048000U) /**< \brief (CLASSD  ) Base Address */
#define I2SC1    (0xFC04C000U) /**< \brief (I2SC1   ) Base Address */
#define MCAN1    (0xFC050000U) /**< \brief (MCAN1   ) Base Address */
#define CHIPID   (0xFC069000U) /**< \brief (CHIPID  ) Base Address */
#else
#define AXIMX    ((Aximx    *)0x00600000U) /**< \brief (AXIMX   ) Base Address */
#define L2CC     ((L2cc     *)0x00A00000U) /**< \brief (L2CC    ) Base Address */
#define LCDC     ((Lcdc     *)0xF0000000U) /**< \brief (LCDC    ) Base Address */
#define XDMAC1   ((Xdmac    *)0xF0004000U) /**< \brief (XDMAC1  ) Base Address */
#define ISC      ((Isc      *)0xF0008000U) /**< \brief (ISC     ) Base Address */
#define MPDDRC   ((Mpddrc   *)0xF000C000U) /**< \brief (MPDDRC  ) Base Address */
#define XDMAC0   ((Xdmac    *)0xF0010000U) /**< \brief (XDMAC0  ) Base Address */
#define PMC      ((Pmc      *)0xF0014000U) /**< \brief (PMC     ) Base Address */
#define MATRIX0  ((Matrix   *)0xF0018000U) /**< \brief (MATRIX0 ) Base Address */
#define AESB     ((Aesb     *)0xF001C000U) /**< \brief (AESB    ) Base Address */
#define QSPI0    ((Qspi     *)0xF0020000U) /**< \brief (QSPI0   ) Base Address */
#define QSPI1    ((Qspi     *)0xF0024000U) /**< \brief (QSPI1   ) Base Address */
#define SHA      ((Sha      *)0xF0028000U) /**< \brief (SHA     ) Base Address */
#define AES      ((Aes      *)0xF002C000U) /**< \brief (AES     ) Base Address */
#define SPI0     ((Spi      *)0xF8000000U) /**< \brief (SPI0    ) Base Address */
#define SSC0     ((Ssc      *)0xF8004000U) /**< \brief (SSC0    ) Base Address */
#define GMAC0    ((Gmac     *)0xF8008000U) /**< \brief (GMAC0   ) Base Address */
#define GMAC     (GMAC0)
#define TC0      ((Tc       *)0xF800C000U) /**< \brief (TC0     ) Base Address */
#define TC1      ((Tc       *)0xF8010000U) /**< \brief (TC1     ) Base Address */
#define HSMC     ((Smc      *)0xF8014000U) /**< \brief (HSMC    ) Base Address */
#define PDMIC    ((Pdmic    *)0xF8018000U) /**< \brief (PDMIC   ) Base Address */
#define UART0    ((Uart     *)0xF801C000U) /**< \brief (UART0   ) Base Address */
#define UART1    ((Uart     *)0xF8020000U) /**< \brief (UART1   ) Base Address */
#define UART2    ((Uart     *)0xF8024000U) /**< \brief (UART2   ) Base Address */
#define TWIHS0   ((Twihs    *)0xF8028000U) /**< \brief (TWIHS0  ) Base Address */
#define PWM      ((Pwm      *)0xF802C000U) /**< \brief (PWM     ) Base Address */
#define SFR      ((Sfr      *)0xF8030000U) /**< \brief (SFR     ) Base Address */
#define FLEXCOM0 ((Flexcom  *)0xF8034000U) /**< \brief (FLEXCOM0) Base Address */
#define USART0   ((Usart    *)0xF8034200U) /**< \brief (FLEXCOM0_USART) Base Address */
#define FCOMSPI0 ((Spi      *)0xF8034400U) /**< \brief (FLEXCOM0_SPI) Base Address */
#define TWI0     ((Twi      *)0xF8034600U) /**< \brief (FLEXCOM0_TWI) Base Address */
#define FLEXCOM1 ((Flexcom  *)0xF8038000U) /**< \brief (FLEXCOM1) Base Address */
#define USART1   ((Usart    *)0xF8038200U) /**< \brief (FLEXCOM1_USART) Base Address */
#define FCOMSPI1 ((Spi      *)0xF8038400U) /**< \brief (FLEXCOM1_SPI) Base Address */
#define TWI1     ((Twi      *)0xF8038600U) /**< \brief (FLEXCOM1_TWI) Base Address */
#define SAIC     ((Aic      *)0xF803C000U) /**< \brief (SAIC    ) Base Address */
#define ICM      ((Icm      *)0xF8040000U) /**< \brief (ICM     ) Base Address */
#define RSTC     ((Rstc     *)0xF8048000U) /**< \brief (RSTC    ) Base Address */
#define SHDWC    ((Shdwc    *)0xF8048010U) /**< \brief (SHDWC   ) Base Address */
#define PIT      ((Pit      *)0xF8048030U) /**< \brief (PIT     ) Base Address */
#define WDT      ((Wdt      *)0xF8048040U) /**< \brief (WDT     ) Base Address */
#define SCKC     ((Sckc     *)0xF8048050U) /**< \brief (SCKC    ) Base Address */
#define RTC      ((Rtc      *)0xF80480B0U) /**< \brief (RTC     ) Base Address */
#define RXLP     ((Rxlp     *)0xF8049000U) /**< \brief (RXLP    ) Base Address */
#define ACC      ((Acc      *)0xF804A000U) /**< \brief (ACC     ) Base Address */
#define SFC      ((Sfc      *)0xF804C000U) /**< \brief (SFC     ) Base Address */
#define I2SC0    ((I2sc     *)0xF8050000U) /**< \brief (I2SC0   ) Base Address */
#define MCAN0    ((Mcan     *)0xF8054000U) /**< \brief (MCAN0   ) Base Address */
#define SPI1     ((Spi      *)0xFC000000U) /**< \brief (SPI1    ) Base Address */
#define SSC1     ((Ssc      *)0xFC004000U) /**< \brief (SSC1    ) Base Address */
#define UART3    ((Uart     *)0xFC008000U) /**< \brief (UART3   ) Base Address */
#define UART4    ((Uart     *)0xFC00C000U) /**< \brief (UART4   ) Base Address */
#define FLEXCOM2 ((Flexcom  *)0xFC010000U) /**< \brief (FLEXCOM2) Base Address */
#define USART2   ((Usart    *)0xFC010200U) /**< \brief (FLEXCOM2_USART) Base Address */
#define FCOMSPI2 ((Spi      *)0xFC010400U) /**< \brief (FLEXCOM2_SPI) Base Address */
#define TWI2     ((Twi      *)0xFC010600U) /**< \brief (FLEXCOM2_TWI) Base Address */
#define FLEXCOM3 ((Flexcom  *)0xFC014000U) /**< \brief (FLEXCOM3) Base Address */
#define USART3   ((Usart    *)0xFC014200U) /**< \brief (FLEXCOM3_USART) Base Address */
#define FCOMSPI3 ((Spi      *)0xFC014400U) /**< \brief (FLEXCOM3_SPI) Base Address */
#define TWI3     ((Twi      *)0xFC014600U) /**< \brief (FLEXCOM3_TWI) Base Address */
#define FLEXCOM4 ((Flexcom  *)0xFC018000U) /**< \brief (FLEXCOM4) Base Address */
#define USART4   ((Usart    *)0xFC018200U) /**< \brief (FLEXCOM4_USART) Base Address */
#define FCOMSPI4 ((Spi      *)0xFC018400U) /**< \brief (FLEXCOM4_SPI) Base Address */
#define TWI4     ((Twi      *)0xFC018600U) /**< \brief (FLEXCOM4_TWI) Base Address */
#define TRNG     ((Trng     *)0xFC01C000U) /**< \brief (TRNG    ) Base Address */
#define AIC      ((Aic      *)0xFC020000U) /**< \brief (AIC     ) Base Address */
#define TWIHS1   ((Twihs    *)0xFC028000U) /**< \brief (TWIHS1  ) Base Address */
#define UDPHS    ((Udphs    *)0xFC02C000U) /**< \brief (UDPHS   ) Base Address */
#define ADC      ((Adc      *)0xFC030000U) /**< \brief (ADC     ) Base Address */
#define PIOA     ((Pio      *)0xFC038000U) /**< \brief (PIOA    ) Base Address */
#define MATRIX1  ((Matrix   *)0xFC03C000U) /**< \brief (MATRIX1 ) Base Address */
#define SECUMOD  ((SecuMod  *)0xFC040000U) /**< \brief (SECUMOD ) Base Address */
#define TDES     ((Tdes     *)0xFC044000U) /**< \brief (TDES    ) Base Address */
#define CLASSD   ((Classd   *)0xFC048000U) /**< \brief (CLASSD  ) Base Address */
#define I2SC1    ((I2sc     *)0xFC04C000U) /**< \brief (I2SC1   ) Base Address */
#define MCAN1    ((Mcan     *)0xFC050000U) /**< \brief (MCAN1   ) Base Address */
#define CHIPID   ((Chipid   *)0xFC069000U) /**< \brief (CHIPID  ) Base Address */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

/* ************************************************************************** */
/*   PIO DEFINITIONS FOR SAMA5D28 */
/* ************************************************************************** */
/** \addtogroup SAMA5D28_pio Peripheral Pio Definitions */
/*@{*/

#include "sama5d2/pio/pio_sama5d28.h"

/*@}*/

/* ************************************************************************** */
/*   MEMORY MAPPING DEFINITIONS FOR SAMA5D28 */
/* ************************************************************************** */


#define EBI_CS0_ADDR    (0x10000000u) /**< EBI Chip Select 0 base address */
#define DDR_CS_ADDR     (0x20000000u) /**< DDR Chip Select base address */
#define DDR_AES_CS_ADDR (0x40000000u) /**< DDR with AES Chip Select base address */
#define EBI_CS1_ADDR    (0x60000000u) /**< EBI Chip Select 1 base address */
#define EBI_CS2_ADDR    (0x70000000u) /**< EBI Chip Select 2 base address */
#define EBI_CS3_ADDR    (0x80000000u) /**< EBI Chip Select 3 base address */
#define QSPI_AES0_ADDR  (0x90000000u) /**< QPSI Memory crypted with AES 0 base address */
#define QSPI_AES1_ADDR  (0x98000000u) /**< QPSI Memory crypted with AES 1 base address */
#define SDMMC0_ADDR     (0xA0000000u) /**< SDMMC 0 base address */
#define SDMMC1_ADDR     (0xB0000000u) /**< SDMMC 1 base address */
#define NFC_ADDR        (0xC0000000u) /**< NAND Flash Controller Command base address */
#define QSPIMEM0_ADDR   (0xD0000000u) /**< QSPI Memory 0 base address */
#define QSPIMEM1_ADDR   (0xD8000000u) /**< QSPI Memory 1 base address */
#define IROM_ADDR       (0x00000000u) /**< Internal ROM base address */
#define ECC_ROM_ADDR    (0x00040000u) /**< ECC ROM base address */
#define NFC_RAM_ADDR    (0x00100000u) /**< NAND Flash Controller RAM base address */
#define IRAM0_ADDR      (0x00200000u) /**< Internal RAM 0 base address */
#define IRAM1_ADDR      (0x00220000u) /**< Internal RAM 1 base address */
#define UDPHS_RAM_ADDR  (0x00300000u) /**< USB High Speed Device Port RAM base address */
#define UHPHS_OHCI_ADDR (0x00400000u) /**< USB High Speed Device Port RAM base address */
#define UHPHS_EHCI_ADDR (0x00500000u) /**< USB High Speed Device Port RAM base address */
#define AXIMX_ADDR      (0x00600000u) /**< AXI Bus Matrix base address */
#define DAP_ADDR        (0x00700000u) /**< Debug Access Port base address */
#define PTCMEM_ADDR     (0x00800000u) /**< PTC Memory base address */
#define L2CC_ADDR       (0x00A00000u) /**< L2CC base address */

/* ************************************************************************** */
/*   MISCELLANEOUS DEFINITIONS FOR SAMA5D28 */
/* ************************************************************************** */

#define CHIP_JTAGID (0x05B3F03FUL)
#define CHIP_CIDR   (0x8A5C08C0UL)
#define CHIP_EXID   (0x00000002UL)

/* ************************************************************************** */
/*   ELECTRICAL DEFINITIONS FOR SAMA5D28 */
/* ************************************************************************** */

/* %ATMEL_ELECTRICAL% */

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN           (20000UL)
#define CHIP_FREQ_SLCK_RC               (32000UL)
#define CHIP_FREQ_SLCK_RC_MAX           (44000UL)
#define CHIP_FREQ_MAINCK_RC_4MHZ        (4000000UL)
#define CHIP_FREQ_MAINCK_RC_8MHZ        (8000000UL)
#define CHIP_FREQ_MAINCK_RC_12MHZ       (12000000UL)
#define CHIP_FREQ_CPU_MAX               (120000000UL)
#define CHIP_FREQ_XTAL_32K              (32768UL)
#define CHIP_FREQ_XTAL_12M              (12000000UL)

/* Embedded Flash Read Wait State (VDDCORE set at 1.20V) */
#define CHIP_FREQ_FWS_0                 (20000000UL)  /**< \brief Maximum operating frequency when FWS is 0 */
#define CHIP_FREQ_FWS_1                 (40000000UL)  /**< \brief Maximum operating frequency when FWS is 1 */
#define CHIP_FREQ_FWS_2                 (60000000UL)  /**< \brief Maximum operating frequency when FWS is 2 */
#define CHIP_FREQ_FWS_3                 (80000000UL)  /**< \brief Maximum operating frequency when FWS is 3 */
#define CHIP_FREQ_FWS_4                 (100000000UL) /**< \brief Maximum operating frequency when FWS is 4 */
#define CHIP_FREQ_FWS_5                 (123000000UL) /**< \brief Maximum operating frequency when FWS is 5 */

#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* _SAMA5D28_ */
