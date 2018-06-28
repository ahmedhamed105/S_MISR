/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
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
/*                                                                              */

#ifndef _CHIP_MISC_H_
#define _CHIP_MISC_H_

/** Size of Cortex-A5 L1 cache line */
#define L1_CACHE_WORDS (8u)
#define L1_CACHE_BYTES (32u)

/** FLEXCOM USART FIFO depth */
#define FLEXCOM_USART_FIFO_DEPTH (32u)

/** FLEXCOM SPI FIFO depth */
#define FLEXCOM_SPI_FIFO_DEPTH (32u)

/** SPI FIFO depth */
#define SPI_FIFO_DEPTH (16u)

/** TWI FIFO depth */
#define TWI_FIFO_DEPTH (16u)

/** Frequency of the on-chip slow clock oscillator */
#define SLOW_CLOCK_INT_OSC 32000

/** Frequency of the on-chip main clock oscillator */
#define MAIN_CLOCK_INT_OSC 12000000

/** AIC redirection unlock key */
#define AICREDIR_KEY 0x5B6C0E26u

#define ID_SAIC_FIQ     ( 0) /**< \brief FIQ Interrupt ID (SAIC_FIQ) */
#define ID_FIQ          (ID_SAIC_FIQ)
#define ID_ARM_PMU      ( 2) /**< \brief Performance Monitor Unit (PMU) (ARM_PMU) */
#define ID_PIT          ( 3) /**< \brief Periodic Interval Timer Interrupt (PIT) */
#define ID_WDT          ( 4) /**< \brief Watchdog timer Interrupt (WDT) */
#define ID_GMAC0        ( 5) /**< \brief Ethernet MAC (GMAC0) */
#define ID_GMAC         (ID_GMAC0)
#define ID_XDMAC0       ( 6) /**< \brief DMA Controller 0 (XDMAC0) */
#define ID_XDMAC1       ( 7) /**< \brief DMA Controller 1 (XDMAC1) */
#define ID_ICM          ( 8) /**< \brief Integritry Check Monitor (ICM) */
#define ID_AES          ( 9) /**< \brief Advanced Enion Standard (AES) */
#define ID_AESB         (10) /**< \brief AES bridge (AESB) */
#define ID_TDES         (11) /**< \brief Triple Data Enion Standard (TDES) */
#define ID_SHA          (12) /**< \brief SHA Signature (SHA) */
#define ID_MPDDRC       (13) /**< \brief MPDDR controller (MPDDRC) */
#define ID_MATRIX1      (14) /**< \brief H32MX, 32-bit AHB Matrix (MATRIX1) */
#define ID_MATRIX0      (15) /**< \brief H64MX, 64-bit AHB Matrix (MATRIX0) */
#define ID_HSMC         (17) /**< \brief Multi-bit ECC Interrupt (HSMC) */
#define ID_PIOA         (18) /**< \brief Parallel I/O Controller (PIOA) */
#define ID_FLEXCOM0     (19) /**< \brief FLEXCOM 0 (FLEXCOM0) */
#define ID_USART0       (19) /**< \brief USART (USART0) from FLEXCOM0 */
#define ID_FCOMSPI0     (19) /**< \brief Serial Peripheral Interface (SPI0) from FLEXCOM0 */
#define ID_TWI0         (19) /**< \brief Two-Wire Interface (TWI0) from FLEXCOM0 */
#define ID_FLEXCOM1     (20) /**< \brief FLEXCOM 1 (FLEXCOM1) */
#define ID_USART1       (20) /**< \brief USART (USART1) from FLEXCOM1 */
#define ID_FCOMSPI1     (20) /**< \brief Serial Peripheral Interface (SPI1) from FLEXCOM1 */
#define ID_TWI1         (20) /**< \brief Two-Wire Interface (TWI1) from FLEXCOM1 */
#define ID_FLEXCOM2     (21) /**< \brief FLEXCOM 1 (FLEXCOM1) */
#define ID_USART2       (21) /**< \brief USART (USART1) from FLEXCOM1 */
#define ID_FCOMSPI2     (21) /**< \brief Serial Peripheral Interface (SPI1) from FLEXCOM1 */
#define ID_TWI2         (21) /**< \brief Two-Wire Interface (TWI1) from FLEXCOM1 */
#define ID_FLEXCOM3     (22) /**< \brief FLEXCOM 3 (FLEXCOM3) */
#define ID_USART3       (22) /**< \brief USART (USART3) from FLEXCOM3 */
#define ID_FCOMSPI3     (22) /**< \brief Serial Peripheral Interface (SPI3) from FLEXCOM3 */
#define ID_TWI3         (22) /**< \brief Two-Wire Interface (TWI3) from FLEXCOM3 */
#define ID_FLEXCOM4     (23) /**< \brief FLEXCOM 4 (FLEXCOM4) */
#define ID_USART4       (23) /**< \brief USART (USART4) from FLEXCOM4 */
#define ID_FCOMSPI4     (23) /**< \brief Serial Peripheral Interface (SPI4) from FLEXCOM4 */
#define ID_TWI4         (23) /**< \brief Two-Wire Interface (TWI4) from FLEXCOM4 */
#define ID_UART0        (24) /**< \brief UART 0 (UART0) */
#define ID_UART1        (25) /**< \brief UART 1 (UART1) */
#define ID_UART2        (26) /**< \brief UART 2 (UART2) */
#define ID_UART3        (27) /**< \brief UART 3 (UART3) */
#define ID_UART4        (28) /**< \brief UART 4 (UART4) */
#define ID_TWIHS0       (29) /**< \brief Two-Wire Interface 0 (TWIHS0) */
#define ID_TWIHS1       (30) /**< \brief Two-Wire Interface 1 (TWIHS1) */
#define ID_SDHC0        (31) /**< \brief Ultra High Speed SD Host Controller 0 (SDHC0) */
#define ID_SDMMC0       (ID_SDHC0)
#define ID_SDHC1        (32) /**< \brief Ultra High Speed SD Host Controller 1 (SDHC1) */
#define ID_SDMMC1       (ID_SDHC1)
#define ID_SPI0         (33) /**< \brief Serial Peripheral Interface 0 (SPI0) */
#define ID_SPI1         (34) /**< \brief Serial Peripheral Interface 1 (SPI1) */
#define ID_TC0          (35) /**< \brief Timer Counter 0 (ch. 0, 1, 2) (TC0) */
#define ID_TC1          (36) /**< \brief Timer Counter 1 (ch. 3, 4, 5) (TC1) */
#define ID_PWM          (38) /**< \brief Pulse Width Modulation Controller0 (ch. 0, 1, 2, 3) (PWM) */
#define ID_ADC          (40) /**< \brief Touch Screen ADC Controller (ADC) */
#define ID_UHPHS        (41) /**< \brief USB Host High Speed (UHPHS) */
#define ID_UDPHS        (42) /**< \brief USB Device High Speed (UDPHS) */
#define ID_SSC0         (43) /**< \brief Synchronous Serial Controller 0 (SSC0) */
#define ID_SSC1         (44) /**< \brief Synchronous Serial Controller 1 (SSC1) */
#define ID_LCDC         (45) /**< \brief LCD Controller (LCDC) */
#define ID_ISC          (46) /**< \brief Camera Interface (ISC) */
#define ID_TRNG         (47) /**< \brief True Random Number Generator (TRNG) */
#define ID_PDMIC        (48) /**< \brief Pulse Density Modulation Interface Controller (PDMIC) */
#define ID_AIC_IRQ      (49) /**< \brief IRQ Interrupt ID (AIC_IRQ) */
#define ID_IRQ          (ID_AIC_IRQ)
#define ID_SFC          (50) /**< \brief Fuse Controller (SFC) */
#define ID_SECURAM      (51) /**< \brief Secured RAM (SECURAM) */
#define ID_QSPI0        (52) /**< \brief QSPI 0 (QSPI0) */
#define ID_QSPI1        (53) /**< \brief QSPI 1 (QSPI1) */
#define ID_I2SC0        (54) /**< \brief Inter-IC Sound Controller 0 (I2SC0) */
#define ID_I2SC1        (55) /**< \brief Inter-IC Sound Controller 1 (I2SC1) */
#define ID_CAN0_INT0    (56) /**< \brief MCAN 0 Interrupt0 (CAN0_INT0) */
#define ID_CAN1_INT0    (57) /**< \brief MCAN 1 Interrupt0 (CAN1_INT0) */
#define ID_CLASSD       (59) /**< \brief Audio Class D amplifier (CLASSD) */
#define ID_SFR          (60) /**< \brief Special Function Register  (SFR) */
#define ID_SAIC         (61) /**< \brief Secured Advanced Interrupt Controller  (SAIC) */
#define ID_AIC          (62) /**< \brief Advanced Interrupt Controller  (AIC) */
#define ID_L2CC         (63) /**< \brief L2 Cache Controller (L2CC) */
#define ID_CAN0_INT1    (64) /**< \brief MCAN 0 Interrupt1 (CAN0_INT1) */
#define ID_CAN1_INT1    (65) /**< \brief MCAN 1 Interrupt1 (CAN1_INT1) */
#define ID_GMAC0_Q1     (66) /**< \brief GMAC Queue 1 Interrupt (GMAC0_Q1) */
#define ID_GMAC0_Q2     (67) /**< \brief GMAC Queue 2 Interrupt (GMAC0_Q2) */
#define ID_PIOB         (68) /**< \brief  (PIOB) */
#define ID_PIOC         (69) /**< \brief  (PIOC) */
#define ID_PIOD         (70) /**< \brief  (PIOD) */
#define ID_SDHC0_TIMER  (71) /**< \brief  (SDHC0_TIMER) */
#define ID_SDHC1_TIMER  (72) /**< \brief  (SDHC1_TIMER) */
#define ID_SYSC         (74) /**< \brief System Controller Interrupt, RTC, RSTC, PMC (SYSC) */
#define ID_ACC          (75) /**< \brief Analog Comparator (ACC) */
#define ID_RXLP         (76) /**< \brief Uart Low Power (RXLP) */
#define ID_CHIPID       (78) /**< \brief Chip ID (CHIPID) */

#define ID_PERIPH_COUNT (79) /**< \brief Number of peripheral IDs */

#endif /* _CHIP_MISC_H_ */
