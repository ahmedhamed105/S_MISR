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

#ifndef _SAMA5D2_SPI0_INSTANCE_
#define _SAMA5D2_SPI0_INSTANCE_

/* ========== Register definition for SPI0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_SPI0_CR                       (0xF8000000U) /**< \brief (SPI0) Control Register */
  #define REG_SPI0_MR                       (0xF8000004U) /**< \brief (SPI0) Mode Register */
  #define REG_SPI0_RDR                      (0xF8000008U) /**< \brief (SPI0) Receive Data Register */
  #define REG_SPI0_TDR                      (0xF800000CU) /**< \brief (SPI0) Transmit Data Register */
  #define REG_SPI0_SR                       (0xF8000010U) /**< \brief (SPI0) Status Register */
  #define REG_SPI0_IER                      (0xF8000014U) /**< \brief (SPI0) Interrupt Enable Register */
  #define REG_SPI0_IDR                      (0xF8000018U) /**< \brief (SPI0) Interrupt Disable Register */
  #define REG_SPI0_IMR                      (0xF800001CU) /**< \brief (SPI0) Interrupt Mask Register */
  #define REG_SPI0_CSR                      (0xF8000030U) /**< \brief (SPI0) Chip Select Register */
  #define REG_SPI0_FMR                      (0xF8000040U) /**< \brief (SPI0) FIFO Mode Register */
  #define REG_SPI0_FLR                      (0xF8000044U) /**< \brief (SPI0) FIFO Level Register */
  #define REG_SPI0_CMPR                     (0xF8000048U) /**< \brief (SPI0) Comparison Register */
  #define REG_SPI0_WPMR                     (0xF80000E4U) /**< \brief (SPI0) Write Protection Mode Register */
  #define REG_SPI0_WPSR                     (0xF80000E8U) /**< \brief (SPI0) Write Protection Status Register */
  #define REG_SPI0_VERSION                  (0xF80000FCU) /**< \brief (SPI0) Version Register */
#else
  #define REG_SPI0_CR      (*(__O  uint32_t*)0xF8000000U) /**< \brief (SPI0) Control Register */
  #define REG_SPI0_MR      (*(__IO uint32_t*)0xF8000004U) /**< \brief (SPI0) Mode Register */
  #define REG_SPI0_RDR     (*(__I  uint32_t*)0xF8000008U) /**< \brief (SPI0) Receive Data Register */
  #define REG_SPI0_TDR     (*(__O  uint32_t*)0xF800000CU) /**< \brief (SPI0) Transmit Data Register */
  #define REG_SPI0_SR      (*(__I  uint32_t*)0xF8000010U) /**< \brief (SPI0) Status Register */
  #define REG_SPI0_IER     (*(__O  uint32_t*)0xF8000014U) /**< \brief (SPI0) Interrupt Enable Register */
  #define REG_SPI0_IDR     (*(__O  uint32_t*)0xF8000018U) /**< \brief (SPI0) Interrupt Disable Register */
  #define REG_SPI0_IMR     (*(__I  uint32_t*)0xF800001CU) /**< \brief (SPI0) Interrupt Mask Register */
  #define REG_SPI0_CSR     (*(__IO uint32_t*)0xF8000030U) /**< \brief (SPI0) Chip Select Register */
  #define REG_SPI0_FMR     (*(__IO uint32_t*)0xF8000040U) /**< \brief (SPI0) FIFO Mode Register */
  #define REG_SPI0_FLR     (*(__I  uint32_t*)0xF8000044U) /**< \brief (SPI0) FIFO Level Register */
  #define REG_SPI0_CMPR    (*(__I  uint32_t*)0xF8000048U) /**< \brief (SPI0) Comparison Register */
  #define REG_SPI0_WPMR    (*(__IO uint32_t*)0xF80000E4U) /**< \brief (SPI0) Write Protection Mode Register */
  #define REG_SPI0_WPSR    (*(__I  uint32_t*)0xF80000E8U) /**< \brief (SPI0) Write Protection Status Register */
  #define REG_SPI0_VERSION (*(__I  uint32_t*)0xF80000FCU) /**< \brief (SPI0) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAMA5D2_SPI0_INSTANCE_ */
