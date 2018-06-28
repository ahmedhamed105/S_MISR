

#ifndef _SAMA5D2_SECUMOD_COMPONENT_
#define _SAMA5D2_SECUMOD_COMPONENT_

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
   __O  uint32_t SECUMOD_CR;
   __IO uint32_t SECUMOD_SYSR;
   __I  uint32_t SECUMOD_SR;
   __I  uint32_t SECUMOD_ASR;
   __O  uint32_t SECUMOD_SCR;
   __I  uint32_t SECUMOD_RAMRDY;
   __IO uint32_t SECUMOD_PIOBU[8];
   __I  uint32_t Reserved1[8];
   __IO uint32_t SECUMOD_VBUFR;
   __I  uint32_t Reserved2[2];
   __IO uint32_t SECUMOD_VCOREFR;
   __IO uint32_t SECUMOD_JTAGCR;
   __IO uint32_t SECUMOD_DYSTUNE;
   __IO uint32_t SECUMOD_SCRKEY;
   __IO uint32_t SECUMOD_RAMACC;
   __IO uint32_t SECUMOD_RAMACCSR;
   __IO uint32_t SECUMOD_BMPR;
   __IO uint32_t SECUMOD_NMPR;
   __O  uint32_t SECUMOD_NIEPR;
   __O  uint32_t SECUMOD_NIDPR;
   __I  uint32_t SECUMOD_NIMPR;
   __IO uint32_t SECUMOD_WKPR;
} SecuMod;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* -------- SECUMOD_CR -------- */
#define SECUMOD_CR_BACKUP                   (0x1u << 0)
#define SECUMOD_CR_NORMAL                   (0x1u << 1)
#define SECUMOD_CR_SWPROT                   (0x1u << 2)
#define SECUMOD_CR_BACKUP                   (0x1u << 0)
#define SECUMOD_CR_NIMP_EN_Pos               3
#define SECUMOD_CR_NIMP_EN_Msk               (0x3u << SECUMOD_CR_NIMP_EN_Pos)
#define SECUMOD_CR_NIMP_EN(value)            ((SECUMOD_CR_NIMP_EN_Msk & ((value) << SECUMOD_CR_NIMP_EN_Pos)))
  #define SECUMOD_CR_NIMP_DISABLE              (0x2u << SECUMOD_CR_NIMP_EN_Pos)
  #define SECUMOD_CR_NIMP_ENABLE               (0x1u << SECUMOD_CR_NIMP_EN_Pos)
#define SECUMOD_CR_AUTOBKP_Pos               5
#define SECUMOD_CR_AUTOBKP_Msk               (0x3u << SECUMOD_CR_AUTOBKP_Pos)
#define SECUMOD_CR_AUTOBKP(value)            ((SECUMOD_CR_AUTOBKP_Msk & ((value) << SECUMOD_CR_AUTOBKP_Pos)))
  #define SECUMOD_CR_AUTOBKP_DISABLE           (0x2u << SECUMOD_CR_AUTOBKP_Pos)
  #define SECUMOD_CR_AUTOBKP_ENABLE            (0x1u << SECUMOD_CR_AUTOBKP_Pos)
#define SECUMOD_CR_SCRAMB_Pos                9
#define SECUMOD_CR_SCRAMB_Msk                (0x3u << SECUMOD_CR_SCRAMB_Pos)
#define SECUMOD_CR_SCRAMB(value)             ((SECUMOD_CR_SCRAMB_Msk & ((value) << SECUMOD_CR_SCRAMB_Pos)))
  #define SECUMOD_CR_SCRAMB_DISABLE            (0x2u << SECUMOD_CR_AUTOBKP_Pos)
  #define SECUMOD_CR_SCRAMB_ENABLE             (0x1u << SECUMOD_CR_AUTOBKP_Pos)
#define SECUMOD_CR_KEY_Pos                   16
#define SECUMOD_CR_KEY_Msk                   (0xffffu << PSECUMOD_CR_KEY_Pos)
#define SECUMOD_CR_KEY(value)                ((SECUMOD_CR_KEY_Msk & ((value) << SECUMOD_CR_KEY_Pos)))
  #define SECUMOD_CR_KEY_PASSWD                (0x89CAu << SECUMOD_CR_KEY_Pos)

/* -------- SECUMOD_SYSR -------- */
#define SECUMOD_SYSR_ERASE_DONE             (0x1u << 0)
#define SECUMOD_SYSR_ERASE_ON               (0x1u << 1)
#define SECUMOD_SYSR_BACKUP_MODE            (0x1u << 2)
#define SECUMOD_SYSR_SWKUP                  (0x1u << 3)
#define SECUMOD_SYSR_NIMP_ENABLED           (0x1u << 5)
#define SECUMOD_SYSR_AUTOBKP_ENABLED        (0x1u << 6)
#define SECUMOD_SYSR_SCRAMB_ENABLED         (0x1u << 7)

/* -------- SECUMOD_SR -------- */
#define SECUMOD_SR_SHLDM_ALARM              (0x1u << 0)
#define SECUMOD_SR_DBLFM_ALARM              (0x1u << 1)
#define SECUMOD_SR_TST_ALARM                (0x1u << 2)
#define SECUMOD_SR_JTAG_ALARM               (0x1u << 3)
#define SECUMOD_SR_MCKM_ALARM               (0x1u << 5)
#define SECUMOD_SR_TPML_ALARM               (0x1u << 6)
#define SECUMOD_SR_TPMH_ALARM               (0x1u << 7)
#define SECUMOD_SR_VDDBUL_ALARM             (0x1u << 10)
#define SECUMOD_SR_VDDBUH_ALARM             (0x1u << 11)
#define SECUMOD_SR_VDDCOREL_ALARM           (0x1u << 12)
#define SECUMOD_SR_VDDCOREH_ALARM           (0x1u << 13)
#define SECUMOD_SR_DET0_ALARM               (0x1u << 16)
#define SECUMOD_SR_DET1_ALARM               (0x1u << 17)
#define SECUMOD_SR_DET2_ALARM               (0x1u << 18)
#define SECUMOD_SR_DET3_ALARM               (0x1u << 19)
#define SECUMOD_SR_DET4_ALARM               (0x1u << 20)
#define SECUMOD_SR_DET5_ALARM               (0x1u << 21)
#define SECUMOD_SR_DET6_ALARM               (0x1u << 22)
#define SECUMOD_SR_DET7_ALARM               (0x1u << 23)

/* -------- SECUMOD_ASR -------- */
#define SECUMOD_ASR_MCKM_LO_ALARM           (0x1u << 0)
#define SECUMOD_ASR_MCKM_HI_ALARM           (0x1u << 1)
#define SECUMOD_ASR_JTAG_ALARM              (0x1u << 4)
#define SECUMOD_ASR_TCK_ALARM               (0x1u << 5)
#define SECUMOD_ASR_BULO_ALARM              (0x1u << 6)
#define SECUMOD_ASR_PSWLO_ALARM             (0x1u << 7)
#define SECUMOD_ASR_BUHI_ALARM              (0x1u << 8)
#define SECUMOD_ASR_PSWHI_ALARM             (0x1u << 9)

/* -------- SECUMOD_SCR -------- */
#define SECUMOD_SCR_SHLDM_CLEAR             (0x1u << 0)
#define SECUMOD_SCR_DBLFM_CLEAR             (0x1u << 1)
#define SECUMOD_SCR_TST_CLEAR               (0x1u << 2)
#define SECUMOD_SCR_JTAG_CLEAR              (0x1u << 3)
#define SECUMOD_SCR_MCKM_CLEAR              (0x1u << 5)
#define SECUMOD_SCR_TPML_CLEAR              (0x1u << 6)
#define SECUMOD_SCR_TPMH_CLEAR              (0x1u << 7)
#define SECUMOD_SCR_VDDBUL_CLEAR            (0x1u << 10)
#define SECUMOD_SCR_VDDBUH_CLEAR            (0x1u << 11)
#define SECUMOD_SCR_VDDCOREL_CLEAR          (0x1u << 12)
#define SECUMOD_SCR_VDDCOREH_CLEAR          (0x1u << 13)
#define SECUMOD_SCR_DET0_CLEAR              (0x1u << 16)
#define SECUMOD_SCR_DET1_CLEAR              (0x1u << 17)
#define SECUMOD_SCR_DET2_CLEAR              (0x1u << 18)
#define SECUMOD_SCR_DET3_CLEAR              (0x1u << 19)
#define SECUMOD_SCR_DET4_CLEAR              (0x1u << 20)
#define SECUMOD_SCR_DET5_CLEAR              (0x1u << 21)
#define SECUMOD_SCR_DET6_CLEAR              (0x1u << 22)
#define SECUMOD_SCR_DET7_CLEAR              (0x1u << 23)
#define SECUMOD_SCR_CLEAR_ALL               (0x00FF3CEF)

/* -------- SECUMOD_RAMRDY -------- */
#define SECUMOD_RAMRDY_READY                (0x1u << 0)

/* -------- SECUMOD_PIOBU -------- */
#define SECUMOD_PIOBU_PIOBU_AFV_Pos         0
#define SECUMOD_PIOBU_PIOBU_AFV_Msk         (0xfu << SECUMOD_PIOBU_PIOBU_AFV_Pos)
#define SECUMOD_PIOBU_PIOBU_AFV(value)      (SECUMOD_PIOBU_PIOBU_AFV_Msk & ((value) << SECUMOD_PIOBU_PIOBU_AFV_Pos)))
  #define SECUMOD_PIOBU_PIOBU_AFV_0           (0x0u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_2           (0x1u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_4           (0x2u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_8           (0x3u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_16          (0x4u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_32          (0x5u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_64          (0x6u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_128         (0x7u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_256         (0x8u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_AFV_512         (0x9u << SECUMOD_PIOBU_PIOBU_AFV_Pos)
#define SECUMOD_PIOBU_PIOBU_RFV_Pos         4
#define SECUMOD_PIOBU_PIOBU_RFV_Msk          (0xfu << SECUMOD_PIOBU_PIOBU_RFV_Pos)
#define SECUMOD_PIOBU_PIOBU_RFV(value)       (SECUMOD_PIOBU_PIOBU_RFV_Msk & ((value) << SECUMOD_PIOBU_PIOBU_RFV_Pos)))
  #define SECUMOD_PIOBU_PIOBU_RFV_0            (0x0u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_2            (0x1u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_4            (0x2u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_8            (0x3u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_16           (0x4u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_32           (0x5u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_64           (0x6u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_128          (0x7u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_256          (0x8u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
  #define SECUMOD_PIOBU_PIOBU_RFV_512          (0x9u << SECUMOD_PIOBU_PIOBU_RFV_Pos)
#define SECUMOD_PIOBU_OUTPUT_ENABLE         (0x1u << 8)
#define SECUMOD_PIOBU_PIO_SOD               (0x1u << 9)
#define SECUMOD_PIOBU_PIO_PDS               (0x1u << 10)
#define SECUMOD_PIOBU_PULLUP_Pos            12
#define SECUMOD_PIOBU_PULLUP_Msk            (0x2u << SECUMOD_PIOBU_PULLUP_Pos)
#define SECUMOD_PIOBU_PULLUP(value)         (SECUMOD_PIOBU_PULLUP_Msk & ((value) << SECUMOD_PIOBU_PULLUP_Pos)))
  #define SECUMOD_PIOBU_NO_PULL               (0x0u << SECUMOD_PIOBU_PULLUP_Pos)
  #define SECUMOD_PIOBU_PULL_UP               (0x1u << SECUMOD_PIOBU_PULLUP_Pos)
  #define SECUMOD_PIOBU_PULL_DOWN             (0x2u << SECUMOD_PIOBU_PULLUP_Pos)
#define SECUMOD_PIOBU_SCHEDULE_ENABLE       (0x1u << 14)
#define SECUMOD_PIOBU_INSTRUSION_HIGH       (0x0u << 15)
#define SECUMOD_PIOBU_INSTRUSION_LOW        (0x1u << 15)
#define SECUMOD_PIOBU_DYNAMIC_ENABLE        (0x1u << 20)
#define SECUMOD_PIOBU_FILTER5               (0x1u << 21)

/* -------- SECUMOD_VBUFR -------- */
#define SECUMOD_VBUFR_VDDBUFV_Pos           0
#define SECUMOD_VBUFR_VDDBUFV_Msk           (0x7u << SECUMOD_VBUFR_VDDBUFV_Pos)
#define SECUMOD_VBUFR_VDDBUFV(value)        (SECUMOD_VBUFR_VDDBUFV_Msk & ((value) << SECUMOD_VBUFR_VDDBUFV_Pos)))
  #define SECUMOD_VBUFR_VDDBUFV_0             (0x0u << SECUMOD_VBUFR_VDDBUFV_Pos)
  #define SECUMOD_VBUFR_VDDBUFV_1             (0x1u << SECUMOD_VBUFR_VDDBUFV_Pos)
  #define SECUMOD_VBUFR_VDDBUFV_2             (0x2u << SECUMOD_VBUFR_VDDBUFV_Pos)
  #define SECUMOD_VBUFR_VDDBUFV_3             (0x3u << SECUMOD_VBUFR_VDDBUFV_Pos)
  #define SECUMOD_VBUFR_VDDBUFV_4             (0x4u << SECUMOD_VBUFR_VDDBUFV_Pos)
  #define SECUMOD_VBUFR_VDDBUFV_5             (0x5u << SECUMOD_VBUFR_VDDBUFV_Pos)
  #define SECUMOD_VBUFR_VDDBUFV_6             (0x6u << SECUMOD_VBUFR_VDDBUFV_Pos)
  #define SECUMOD_VBUFR_VDDBUFV_7             (0x7u << SECUMOD_VBUFR_VDDBUFV_Pos)

/* -------- SECUMOD_VCOREFR -------- */
#define SECUMOD_VCOREFR_Pos                 0
#define SECUMOD_VCOREFR_Msk                 (0x1fffu << SECUMOD_VCOREFR_Pos)
#define SECUMOD_VCOREFR(value)              (SECUMOD_VCOREFR_Msk & ((value) << SECUMOD_VCOREFR_Pos))

/* -------- SECUMOD_JTAGCR -------- */
#define SECUMOD_JTAGCR_FNTRST               (0x1u << 0)
#define SECUMOD_JTAGCR_CA5_DEBUG_MODE_Pos   0
#define SECUMOD_JTAGCR_CA5_DEBUG_MODE_Msk   (0x7u << SECUMOD_JTAGCR_CA5_DEBUG_MODE_Pos)
#define SECUMOD_JTAGCR_CA5_DEBUG_MODE(value) (SECUMOD_JTAGCR_CA5_DEBUG_MODE_Msk & ((value) << SECUMOD_JTAGCR_CA5_DEBUG_MODE_Pos)))
#define SECUMOD_JTAGCR_CA5_DEBUG_MON        (0x1u << 4)

/* -------- SECUMOD_DYSTUNE -------- */
#define SECUMOD_DYSTUNE_RX_ERROR_THRESHOLD_Pos     0
#define SECUMOD_DYSTUNE_RX_ERROR_THRESHOLD_Msk     (0x7fu << SECUMOD_DYSTUNE_RX_ERROR_THRESHOLD_Pos)
#define SECUMOD_DYSTUNE_RX_ERROR_THRESHOLD(value)  (SECUMOD_DYSTUNE_RX_ERROR_THRESHOLD_Msk & ((value) << SECUMOD_DYSTUNE_RX_ERROR_THRESHOLD_Pos)))
#define SECUMOD_DYSTUNE_NOPA          (0x1u << 7)
#define SECUMOD_DYSTUNE_RX_OK_CORREL_NUMBER_Pos    8
#define SECUMOD_DYSTUNE_RX_OK_CORREL_NUMBER_Msk    (0xffu << SECUMOD_DYSTUNE_RX_OK_CORREL_NUMBER_Pos)
#define SECUMOD_DYSTUNE_RX_OK_CORREL_NUMBER(value) (SECUMOD_DYSTUNE_RX_OK_CORREL_NUMBER_Msk & ((value) << SECUMOD_DYSTUNE_RX_OK_CORREL_NUMBER_Pos)))
#define SECUMOD_DYSTUNE_PERIOD_Pos                 16
#define SECUMOD_DYSTUNE_PERIOD_Msk                 (0xffffu << SECUMOD_DYSTUNE_PERIOD_Pos)
#define SECUMOD_DYSTUNE_PERIOD(value)              (SECUMOD_DYSTUNE_PERIOD_Msk & ((value) << SECUMOD_DYSTUNE_PERIOD_Pos)))

/* -------- SECUMOD_RAMACC -------- */
#define SECUMOD_RAMACC_RW0_Pos              0
#define SECUMOD_RAMACC_RW0_Msk              (0x3u << SECUMOD_RAMACC_RW0_Pos)
#define SECUMOD_RAMACC_RW0(value)           (SECUMOD_RAMACC_RW0_Msk & ((value) << SECUMOD_RAMACC_RW0_Pos)))
  #define SECUMOD_RAMACC_RW0_NO_ACCESS        (0x0u << SECUMOD_RAMACC_RW0_Pos)
  #define SECUMOD_RAMACC_RW0_WRITE_ONLY       (0x1u << SECUMOD_RAMACC_RW0_Pos)
  #define SECUMOD_RAMACC_RW0_READ_ONLY        (0x2u << SECUMOD_RAMACC_RW0_Pos)
  #define SECUMOD_RAMACC_RW0_READ_WRITE       (0x3u << SECUMOD_RAMACC_RW0_Pos)
#define SECUMOD_RAMACC_RW1_Pos              2
#define SECUMOD_RAMACC_RW1_Msk              (0x3u << SECUMOD_RAMACC_RW1_Pos)
#define SECUMOD_RAMACC_RW1(value)           (SECUMOD_RAMACC_RW1_Msk & ((value) << SECUMOD_RAMACC_RW1_Pos)))
  #define SECUMOD_RAMACC_RW1_NO_ACCESS        (0x0u << SECUMOD_RAMACC_RW1_Pos)
  #define SECUMOD_RAMACC_RW1_WRITE_ONLY       (0x1u << SECUMOD_RAMACC_RW1_Pos)
  #define SECUMOD_RAMACC_RW1_READ_ONLY        (0x2u << SECUMOD_RAMACC_RW1_Pos)
  #define SECUMOD_RAMACC_RW1_READ_WRITE       (0x3u << SECUMOD_RAMACC_RW1_Pos)
#define SECUMOD_RAMACC_RW2_Pos              4
#define SECUMOD_RAMACC_RW2_Msk              (0x3u << SECUMOD_RAMACC_RW2_Pos)
#define SECUMOD_RAMACC_RW2(value)           (SECUMOD_RAMACC_RW2_Msk & ((value) << SECUMOD_RAMACC_RW2_Pos)))
  #define SECUMOD_RAMACC_RW2_NO_ACCESS        (0x0u << SECUMOD_RAMACC_RW2_Pos)
  #define SECUMOD_RAMACC_RW2_WRITE_ONLY       (0x1u << SECUMOD_RAMACC_RW2_Pos)
  #define SECUMOD_RAMACC_RW2_READ_ONLY        (0x2u << SECUMOD_RAMACC_RW2_Pos)
  #define SECUMOD_RAMACC_RW2_READ_WRITE       (0x3u << SECUMOD_RAMACC_RW2_Pos)
#define SECUMOD_RAMACC_RW3_Pos              6
#define SECUMOD_RAMACC_RW3_Msk              (0x3u << SECUMOD_RAMACC_RW3_Pos)
#define SECUMOD_RAMACC_RW3(value)           (SECUMOD_RAMACC_RW3_Msk & ((value) << SECUMOD_RAMACC_RW3_Pos)))
  #define SECUMOD_RAMACC_RW3_NO_ACCESS        (0x0u << SECUMOD_RAMACC_RW3_Pos)
  #define SECUMOD_RAMACC_RW3_WRITE_ONLY       (0x1u << SECUMOD_RAMACC_RW3_Pos)
  #define SECUMOD_RAMACC_RW3_READ_ONLY        (0x2u << SECUMOD_RAMACC_RW3_Pos)
  #define SECUMOD_RAMACC_RW3_READ_WRITE       (0x3u << SECUMOD_RAMACC_RW3_Pos)
#define SECUMOD_RAMACC_RW4_Pos              8
#define SECUMOD_RAMACC_RW4_Msk              (0x3u << SECUMOD_RAMACC_RW4_Pos)
#define SECUMOD_RAMACC_RW4(value)            (SECUMOD_RAMACC_RW4_Msk & ((value) << SECUMOD_RAMACC_RW4_Pos)))
  #define SECUMOD_RAMACC_RW4_NO_ACCESS         (0x0u << SECUMOD_RAMACC_RW4_Pos)
  #define SECUMOD_RAMACC_RW4_WRITE_ONLY        (0x1u << SECUMOD_RAMACC_RW4_Pos)
  #define SECUMOD_RAMACC_RW4_READ_ONLY         (0x2u << SECUMOD_RAMACC_RW4_Pos)
  #define SECUMOD_RAMACC_RW4_READ_WRITE        (0x3u << SECUMOD_RAMACC_RW4_Pos)
#define SECUMOD_RAMACC_RW5_Pos              10
#define SECUMOD_RAMACC_RW5_Msk              (0x3u << SECUMOD_RAMACC_RW5_Pos)
#define SECUMOD_RAMACC_RW5(value)           (SECUMOD_RAMACC_RW5_Msk & ((value) << SECUMOD_RAMACC_RW5_Pos)))
  #define SECUMOD_RAMACC_RW5_NO_ACCESS        (0x0u << SECUMOD_RAMACC_RW5_Pos)
  #define SECUMOD_RAMACC_RW5_WRITE_ONLY       (0x1u << SECUMOD_RAMACC_RW5_Pos)
  #define SECUMOD_RAMACC_RW5_READ_ONLY        (0x2u << SECUMOD_RAMACC_RW5_Pos)
  #define SECUMOD_RAMACC_RW5_READ_WRITE       (0x3u << SECUMOD_RAMACC_RW5_Pos)

/* -------- SECUMOD_RAMACCSR -------- */
#define SECUMOD_RAMACCSR_RW0_Pos            0
#define SECUMOD_RAMACCSR_RW0_Msk            (0x3u << SECUMOD_RAMACCSR_RW0_Pos)
#define SECUMOD_RAMACCSR_RW1_Pos            2
#define SECUMOD_RAMACCSR_RW1_Msk            (0x3u << SECUMOD_RAMACCSR_RW1_Pos)
#define SECUMOD_RAMACCSR_RW2_Pos            4
#define SECUMOD_RAMACCSR_RW2_Msk            (0x3u << SECUMOD_RAMACCSR_RW2_Pos)
#define SECUMOD_RAMACCSR_RW3_Pos            6
#define SECUMOD_RAMACCSR_RW3_Msk            (0x3u << SECUMOD_RAMACCSR_RW3_Pos)
#define SECUMOD_RAMACCSR_RW4_Pos            8
#define SECUMOD_RAMACCSR_RW4_Msk            (0x3u << SECUMOD_RAMACCSR_RW4_Pos)
#define SECUMOD_RAMACCSR_RW5_Pos            10
#define SECUMOD_RAMACCSR_RW5_Msk            (0x3u << SECUMOD_RAMACCSR_RW5_Pos)

/* -------- SECUMOD_BMPR -------- */
#define SECUMOD_BMPR_SHLDM_ENABLE           (0x1u << 0)
#define SECUMOD_BMPR_DBLFM_ENABLE           (0x1u << 1)
#define SECUMOD_BMPR_TST_ENABLE             (0x1u << 2)
#define SECUMOD_BMPR_JTAG_ENABLE            (0x1u << 3)
#define SECUMOD_BMPR_TPML_ENABLE            (0x1u << 6)
#define SECUMOD_BMPR_TPMH_ENABLE            (0x1u << 7)
#define SECUMOD_BMPR_VDDBUL_ENABLE          (0x1u << 10)
#define SECUMOD_BMPR_VDDBUH_ENABLE          (0x1u << 11)
#define SECUMOD_BMPR_DET0_ENABLE            (0x1u << 16)
#define SECUMOD_BMPR_DET1_ENABLE            (0x1u << 17)
#define SECUMOD_BMPR_DET2_ENABLE            (0x1u << 18)
#define SECUMOD_BMPR_DET3_ENABLE            (0x1u << 19)
#define SECUMOD_BMPR_DET4_ENABLE            (0x1u << 20)
#define SECUMOD_BMPR_DET5_ENABLE            (0x1u << 21)
#define SECUMOD_BMPR_DET6_ENABLE            (0x1u << 22)
#define SECUMOD_BMPR_DET7_ENABLE            (0x1u << 23)

/* -------- SECUMOD_NMPR -------- */
#define SECUMOD_NMPR_SHLDM_ENABLE           (0x1u << 0)
#define SECUMOD_NMPR_DBLFM_ENABLE           (0x1u << 1)
#define SECUMOD_NMPR_TST_ENABLE             (0x1u << 2)
#define SECUMOD_NMPR_JTAG_ENABLE            (0x1u << 3)
#define SECUMOD_NMPR_MCKM_ENABLE            (0x1u << 5)
#define SECUMOD_NMPR_TPML_ENABLE            (0x1u << 6)
#define SECUMOD_NMPR_TPMH_ENABLE            (0x1u << 7)
#define SECUMOD_NMPR_VDDBUL_ENABLE          (0x1u << 10)
#define SECUMOD_NMPR_VDDBUH_ENABLE          (0x1u << 11)
#define SECUMOD_NMPR_VDDCOREL_ENABLE        (0x1u << 12)
#define SECUMOD_NMPR_VDDCOREH_ENABLE        (0x1u << 13)
#define SECUMOD_NMPR_DET0_ENABLE            (0x1u << 16)
#define SECUMOD_NMPR_DET1_ENABLE            (0x1u << 17)
#define SECUMOD_NMPR_DET2_ENABLE            (0x1u << 18)
#define SECUMOD_NMPR_DET3_ENABLE            (0x1u << 19)
#define SECUMOD_NMPR_DET4_ENABLE            (0x1u << 20)
#define SECUMOD_NMPR_DET5_ENABLE            (0x1u << 21)
#define SECUMOD_NMPR_DET6_ENABLE            (0x1u << 22)
#define SECUMOD_NMPR_DET7_ENABLE            (0x1u << 23)

/* -------- SECUMOD_NIEPR -------- */
#define SECUMOD_NIEPR_SHLDM_ENABLE          (0x1u << 0)
#define SECUMOD_NIEPR_DBLFM_ENABLE          (0x1u << 1)
#define SECUMOD_NIEPR_TST_ENABLE            (0x1u << 2)
#define SECUMOD_NIEPR_JTAG_ENABLE           (0x1u << 3)
#define SECUMOD_NIEPR_MCKM_ENABLE           (0x1u << 5)
#define SECUMOD_NIEPR_TPML_ENABLE           (0x1u << 6)
#define SECUMOD_NIEPR_TPMH_ENABLE           (0x1u << 7)
#define SECUMOD_NIEPR_VDDBUL_ENABLE         (0x1u << 10)
#define SECUMOD_NIEPR_VDDBUH_ENABLE         (0x1u << 11)
#define SECUMOD_NIEPR_VDDCOREL_ENABLE       (0x1u << 12)
#define SECUMOD_NIEPR_VDDCOREH_ENABLE       (0x1u << 13)
#define SECUMOD_NIEPR_DET0_ENABLE           (0x1u << 16)
#define SECUMOD_NIEPR_DET1_ENABLE           (0x1u << 17)
#define SECUMOD_NIEPR_DET2_ENABLE           (0x1u << 18)
#define SECUMOD_NIEPR_DET3_ENABLE           (0x1u << 19)
#define SECUMOD_NIEPR_DET4_ENABLE           (0x1u << 20)
#define SECUMOD_NIEPR_DET5_ENABLE           (0x1u << 21)
#define SECUMOD_NIEPR_DET6_ENABLE           (0x1u << 22)
#define SECUMOD_NIEPR_DET7_ENABLE           (0x1u << 23)

/* -------- SECUMOD_NIDPR -------- */
#define SECUMOD_NIDPR_SHLDM_DISABLE         (0x1u << 0)
#define SECUMOD_NIDPR_DBLFM_DISABLE         (0x1u << 1)
#define SECUMOD_NIDPR_TST_DISABLE           (0x1u << 2)
#define SECUMOD_NIDPR_JTAG_DISABLE          (0x1u << 3)
#define SECUMOD_NIDPR_MCKM_DISABLE          (0x1u << 5)
#define SECUMOD_NIDPR_TPML_DISABLE          (0x1u << 6)
#define SECUMOD_NIDPR_TPMH_DISABLE          (0x1u << 7)
#define SECUMOD_NIDPR_VDDBUL_DISABLE        (0x1u << 10)
#define SECUMOD_NIDPR_VDDBUH_DISABLE        (0x1u << 11)
#define SECUMOD_NIDPR_VDDCOREL_DISABLE      (0x1u << 12)
#define SECUMOD_NIDPR_VDDCOREH_DISABLE      (0x1u << 13)
#define SECUMOD_NIDPR_DET0_DISABLE          (0x1u << 16)
#define SECUMOD_NIDPR_DET1_DISABLE          (0x1u << 17)
#define SECUMOD_NIDPR_DET2_DISABLE          (0x1u << 18)
#define SECUMOD_NIDPR_DET3_DISABLE          (0x1u << 19)
#define SECUMOD_NIDPR_DET4_DISABLE          (0x1u << 20)
#define SECUMOD_NIDPR_DET5_DISABLE          (0x1u << 21)
#define SECUMOD_NIDPR_DET6_DISABLE          (0x1u << 22)
#define SECUMOD_NIDPR_DET7_DISABLE          (0x1u << 23)

/* -------- SECUMOD_NIMPR -------- */
#define SECUMOD_NIMPR_SHLDM                 (0x1u << 0)
#define SECUMOD_NIMPR_DBLFM                 (0x1u << 1)
#define SECUMOD_NIMPR_TST                   (0x1u << 2)
#define SECUMOD_NIMPR_JTAG                  (0x1u << 3)
#define SECUMOD_NIMPR_MCKM                  (0x1u << 5)
#define SECUMOD_NIMPR_TPML                  (0x1u << 6)
#define SECUMOD_NIMPR_TPMH                  (0x1u << 7)
#define SECUMOD_NIMPR_VDDBUL_               (0x1u << 10)
#define SECUMOD_NIMPR_VDDBUH_               (0x1u << 11)
#define SECUMOD_NIMPR_VDDCOREL              (0x1u << 12)
#define SECUMOD_NIMPR_VDDCOREH              (0x1u << 13)
#define SECUMOD_NIMPR_DET0                  (0x1u << 16)
#define SECUMOD_NIMPR_DET1                  (0x1u << 17)
#define SECUMOD_NIMPR_DET2                  (0x1u << 18)
#define SECUMOD_NIMPR_DET3                  (0x1u << 19)
#define SECUMOD_NIMPR_DET4                  (0x1u << 20)
#define SECUMOD_NIMPR_DET5                  (0x1u << 21)
#define SECUMOD_NIMPR_DET6                  (0x1u << 22)
#define SECUMOD_NIMPR_DET7                  (0x1u << 23)

/* -------- SECUMOD_WKPR -------- */
#define SECUMOD_WKPR_SHLDM                  (0x1u << 0)
#define SECUMOD_WKPR_DBLFM                  (0x1u << 1)
#define SECUMOD_WKPR_TST                    (0x1u << 2)
#define SECUMOD_WKPR_JTAG                   (0x1u << 3)
#define SECUMOD_WKPR_TPML                   (0x1u << 6)
#define SECUMOD_WKPR_TPMH                   (0x1u << 7)
#define SECUMOD_WKPR_VDDBUL_                (0x1u << 10)
#define SECUMOD_WKPR_VDDBUH_                (0x1u << 11)
#define SECUMOD_WKPR_DET0                   (0x1u << 16)
#define SECUMOD_WKPR_DET1                   (0x1u << 17)
#define SECUMOD_WKPR_DET2                   (0x1u << 18)
#define SECUMOD_WKPR_DET3                   (0x1u << 19)
#define SECUMOD_WKPR_DET4                   (0x1u << 20)
#define SECUMOD_WKPR_DET5                   (0x1u << 21)
#define SECUMOD_WKPR_DET6                   (0x1u << 22)
#define SECUMOD_WKPR_DET7                   (0x1u << 23)

#endif

