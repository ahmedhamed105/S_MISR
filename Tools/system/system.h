/*
************************************
*       Module : system.h          *
*       Name   : TF                *
*       Date   : 08-06-2017        *
************************************
*/
/*
System Pack V1.3
================
1. Add option "os_power_sleep_ak_pwake( )" to exit Power Sleep Mode by any key press (ECR17-0029).  "K_PowerSleepKeyPadPress" return for keypad key press to wake up.
2. Add os_tamper_unlock() security function.
3. Add K_AuthUnlock in os_auth to do authentication before os_tamper_unlock() is invoked.

System Pack V1.2
================
1. Add encrypt and decrypt mode in AES_GCM
2. Add new font display feature (ECR17-0011) :
(a) display characters without limitation (os_disp_anywhere_puts())
(b) show characters with transparent background (K_DispTextBodyOnlyEn & K_DispTextBodyOnlyDis)                  

System Pack V1.1
================
1. w_id is not used in T_AES_CONTEXT struct
2. Enhance tamper detection
3. Change printer darkness range from -7 to 7 to from 0 to 20
4. T_SHA_CONTEXT is updated to cater HMAC
5. add os_disp_get_font_len(void * a_str, DWORD a_font_type) to get display/print message length in display unit
6. add K_UFontVar
7. add K_SysRandFill function for SAM/MSR/CLICC drivers to fill buffer w/random number (PCI v4.x).
8. add K_MsrClear for application to clear the buffer w/random nmber (PCI v4.x).
9. Support GCM-AES and add T_GCM_AES_CONTEXT, os_gcm_aes_init, os_gcm_aes_process and os_gcm_aes_finish
10.Add 1ms thread for system use only
11.Support Atmel wifi ATWINC1500 and add wifi os and base functions for system use

Date : 20160308
1. Add auth API for DMK injection
2. Add true 16*8 English font in diaplay select by K_SelSupBigFont and in printing by ESC 'F' '9'
3. Add barcode config K_CF_Barcode. If it is set, product code will be ignored. If it is not set, it will use product code
   so that applicationtion can set it instead of using product code
4. add dtimer_latch_start, dtimer_latch_stop & dtimer_latch_time in device interface to be call by driver
5. add os_dtimer_latch_start, os_dtimer_latch_stop & os_dtimer_latch_time
6. Add 2-D Barcode sensitive setting in system config
7. Add Logo control system config
8. Add os_lpt_reset_count() and os_lpt_get_count()

*/
#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#include "common.h"

#ifndef _UTIL_
#include "app_info.h"
#include "at91sam9g45.h"
#include "sama5d2.h"

#endif

#define AT91C_MBASE_UHPHS_OHCI  ((AT91PS_UHPHS_OHCI)UHPHS_OHCI_ADDR) /* VA = PA = 0x400000 */

#define K_Trk2MaxLen          40
#define K_Trk1MaxLen          79    //107
#define K_Trk3MaxLen          107


// All 88K DMA memory is mapped with I/O
#define DMA_BUFFER_BASE       (0x210000)
#define DMA_BFFER_END         (DMA_BUFFER_BASE + 64*1024)

#define DMA_MSR_AREA          DMA_BUFFER_BASE
#define DMA_UHP_AREA          (DMA_MSR_AREA)
#define DMA_UHP_AREA_LEN      (4096 + 256)
#define DMA_LAN_AREA          (DMA_UHP_AREA+DMA_UHP_AREA_LEN)
#define DMA_LAN_RX_AREA       DMA_LAN_AREA
#define DMA_LAN_RX_ENTRY      (38*4)                              // original 39
#define DMA_LAN_RX_LEN        (DMA_LAN_RX_ENTRY*128)
#define DMA_LAN_TX_AREA       (DMA_LAN_RX_AREA+DMA_LAN_RX_LEN)
#define DMA_LAN_TX_ENTRY      2
#define DMA_LAN_TX_LEN        1536*DMA_LAN_TX_ENTRY
#define DMA_SAM_AREA          (DMA_LAN_TX_AREA+DMA_LAN_TX_LEN)    // 32-byte align
#define DMA_SAM_AREA_LEN      1024
#define DMA_GPRS_AREA         (DMA_SAM_AREA+DMA_SAM_AREA_LEN)
#define DMA_GPRS_AREA_LEN     1024
#define DMA_SPI_AREA          (DMA_GPRS_AREA+DMA_GPRS_AREA_LEN)
#define DMA_SPI_AREA_LEN      1024
#define DMA_LAN_RX_LIST       (DMA_SPI_AREA+DMA_SPI_AREA_LEN)
#define DMA_LAN_RX_LIST_LEN   (DMA_LAN_RX_ENTRY*8)
#define DMA_LAN_TX_LIST       (DMA_LAN_RX_LIST+DMA_LAN_RX_LIST_LEN)
#define DMA_LAN_TX_LIST_LEN   (DMA_LAN_TX_ENTRY*8)
#define DMA_MSR_TRK2_LEN      (K_Trk2MaxLen*5*3*4)
#define DMA_MSR_TRK1_LEN      (K_Trk1MaxLen*7*3*4)
#define DMA_MSR_TRK3_LEN      (K_Trk3MaxLen*5*3*4)
#define DMA_MSR2_BUFFER       (DMA_LAN_TX_LIST+DMA_LAN_TX_LIST_LEN)
#define DMA_MSR1_BUFFER       (DMA_MSR2_BUFFER+DMA_MSR_TRK2_LEN)
#define DMA_MSR3_BUFFER       (DMA_MSR1_BUFFER+DMA_MSR_TRK1_LEN)
#define MSR_AREA_LEN          (DMA_MSR_TRK2_LEN+DMA_MSR_TRK1_LEN+DMA_MSR_TRK3_LEN)
#define DMA_LAN_RX_LIST_DMY       (DMA_MSR3_BUFFER+DMA_MSR_TRK3_LEN)
#define DMA_LAN_RX_LIST_DMY_ENTRY (2)
#define DMA_LAN_RX_LIST_DMY_LEN   (DMA_LAN_RX_LIST_DMY_ENTRY * 8)
#define DMA_LAN_TX_LIST_DMY       (DMA_LAN_RX_LIST_DMY + DMA_LAN_RX_LIST_DMY_LEN)
#define DMA_LAN_TX_LIST_DMY_ENTRY (2)
#define DMA_LAN_TX_LIST_DMY_LEN   (DMA_LAN_TX_LIST_DMY_ENTRY * 8)
#if 0
#define DMA_DISP_LCD_DESC         (DMA_LAN_TX_LIST_DMY + DMA_LAN_TX_LIST_DMY_LEN)
#define DMA_DISP_LCD_DESC_ENTRY   (3)  // (K_DispMaxOverlay+1)
#define DMA_DISP_LCD_DESC_LEN     (DMA_DISP_LCD_DESC_ENTRY * 16)
#endif


// DMA Channel Assignment
enum {   // xdma0
K_Dma0uSDChn,
K_Dma0NandRWChn,
K_Dma0LanTxChn,
K_Dma0LanRxChn,
K_Dma0Com1TxChn,
K_Dma0Com1RxChn,
K_Dma0Com2TxChn,
K_Dma0Com2RxChn,
K_Dma0LcdRGBChn,
K_Dma0LcdImgChn,
K_Dma0GprsTxChn,
K_Dma0GprsRxChn,
K_Dma0SpiTxChn,
K_Dma0SpiRxChn,
K_Dma0WiFiTxChn,
K_Dma0WiFiRxChn
};

enum {  // xdma1
K_Dma1SamTxRxChn,
K_Dma1PmuTxRxChn,
K_Dma1ClassDTxChn,
K_Dma1MsrTrk2RxChn,
K_Dma1MsrTrk1RxChn,
K_Dma1MsrTrk3RxChn,
K_Dma1ImageSensorChn,
K_Dma1GenChn1,
K_Dma1GenChn2,
K_Dma1GenChn3,
K_Dma1GenChn4,
K_Dma1GenChn5,
K_Dma1GenChn6,
K_Dma1GenChn7,
K_Dma1GenChn8,
K_Dma1GenChn9,
};

#define K_Dma0ComChnOffset        K_Dma0Com1TxChn
#define K_DmaMsrChnOffset         K_Dma1MsrTrk2RxChn


extern DWORD os_call_app(DWORD a_appid,DWORD a_param1,DWORD a_param2,DWORD a_param3);
extern DWORD lib_app_call(DWORD a_appid,DWORD a_func_no,DWORD a_param1,DWORD a_param2, DWORD a_param3);
extern __pcs DWORD sys_func0(DWORD a_func_no);
extern __pcs DWORD sys_func1(DWORD a_func_no,DWORD a_param1);
extern __pcs DWORD sys_func2(DWORD a_func_no,DWORD a_param1,DWORD a_param2);
extern __pcs DWORD sys_func3(DWORD a_func_no,DWORD a_param1,DWORD a_param2,DWORD a_param3);


#define K_NoProcess             0xff
#define K_SysProc               0

// application ID
#define K_SysID                 0
#define K_ProdCode              1
#define K_DriverID              2
#define K_BaseID                3
#define K_Font                  4
#define K_FontSp                5
#define K_SamDriver             6
#define K_CliccDriver           7
#define K_BtLoader              8
#define K_MfgID                 9
#define K_InternalID            10
#define K_SredID                10
#define K_TamperUlID            12
#define K_BarDllID              13
#define K_EmvDllID              14
#define K_MidID                 15
#define K_ApmID                 16
#define K_UserID                17

#define K_MaxProcAllowed        100             // pid 1 to 100

// GPRS reqest ID used by system only
#define K_GprsCmuxTimeout       0x80
#define K_GprsCmuxSmsReq        111
#define K_GprsCmuxData          110
#define K_GprsCmuxWakeResp      109
#define K_GprsCmuxDLC0          108
#define K_GprsCmuxCmd           107
#define K_GprsCmuxSdlcCmd       106
#define K_GprsCmuxWakeReq       105
#define K_GprsCmuxMscDtrOn      104
#define K_GprsCmuxMscDtrOff     103
#define K_GprsCmuxSmsResult     102
#define K_GprsReset             101


// Product Code service
#define K_T1000ProductDescription    0
#define K_T1000HwConfig              1
#define K_T1000XHwConfig             2               /* Extended HW config for T1000 */

// font file interface
#define K_UFontScreenWidth      26              /* Max 26 col */
#define K_UFontScreenHeight     9               /* Max 9 row */

// multi-codepage
#define K_UserFontCodepage      0x10000000    /* bit28, when set = 0 -> Codepage0; 1 -> Codepage1, default is 0 */
// multi-codepage
#define K_UserFontEnhance       0x20000000

#define K_UFontBufferSize       192
typedef struct {
  WORD w_status;
  WORD w_temp;
  BYTE s_bitmap[K_UFontBufferSize];             /* up to 16x16 dots for enhanced font */
} T_USER_FONT;

//User Font return bit
#define K_UFontMask             0x07
#define K_UFontHeight8          0x00
#define K_UFontHeight16         0x01
#define K_UFontWidth8           0x00
#define K_UFontWidth16          0x02
#define K_UFont8x8              0x00
#define K_UFont8x16             0x01
#define K_UFont16x8             0x02
#define K_UFont16x16            0x03
#define K_UFont6x8              0x04
#define K_UFont12x16            0x05              // printer font
#define K_UFontVar              0x07
// multi-codepage
#define K_UFontCodePages        0x20              // support more than 1 codepage
// multi-codepage
#define K_UFontEnhance          0x40              // support enhance detail font for display
#define K_UFontReady            0x80

typedef struct {
  WORD  w_background;
  WORD  w_rfu1;
  WORD  w_rfu2;
  WORD  w_anntena;
  WORD  w_signal;
  WORD  w_tx_only;
  WORD  w_rx_only;
  WORD  w_tx_rx;
  WORD  w_banner_text;
} T_DISP_ICON_COLOR;

// RTC
typedef struct {
  BYTE  s_century[2];
  BYTE  s_year[2];
  BYTE  s_month[2];
  BYTE  s_day[2];
  BYTE  s_hour[2];
  BYTE  s_min[2];
  BYTE  s_sec[2];
} T_DTG;

// SSL
typedef struct {
  DWORD s_total[2];
  DWORD s_state[4];
  BYTE  s_buffer[64];
} T_MD5_CONTEXT;

#define T_MD4_CONTEXT           T_MD5_CONTEXT

typedef struct {
  DWORD d_x, d_y, s_m[256];
} T_RC4_STATE;

#define K_Sha1                (SHA_MR_ALGO_SHA1 >> SHA_MR_ALGO_Pos)
#define K_Sha256              (SHA_MR_ALGO_SHA256 >> SHA_MR_ALGO_Pos)
#define K_Sha384              (SHA_MR_ALGO_SHA384 >> SHA_MR_ALGO_Pos)
#define K_Sha512              (SHA_MR_ALGO_SHA512 >> SHA_MR_ALGO_Pos)
#define K_Sha224              (SHA_MR_ALGO_SHA224 >> SHA_MR_ALGO_Pos)
#define K_HmacSha1            (SHA_MR_ALGO_HMAC_SHA1 >> SHA_MR_ALGO_Pos)
#define K_HmacSha256          (SHA_MR_ALGO_HMAC_SHA256 >> SHA_MR_ALGO_Pos)
#define K_HmacSha384          (SHA_MR_ALGO_HMAC_SHA384 >> SHA_MR_ALGO_Pos)
#define K_HmacSha512          (SHA_MR_ALGO_HMAC_SHA512 >> SHA_MR_ALGO_Pos)
#define K_HmacSha224          (SHA_MR_ALGO_HMAC_SHA224 >> SHA_MR_ALGO_Pos)

#define K_Sha1DigestSize      ( 160 / 8)
#define K_Sha224DigestSize    ( 224 / 8)
#define K_Sha256DigestSize    ( 256 / 8)
#define K_Sha384DigestSize    ( 384 / 8)
#define K_Sha512DigestSize    ( 512 / 8)

#define K_Sha256BlockSize     ( 512 / 8)
#define K_Sha512BlockSize     (1024 / 8)
#define K_Sha384BlockSize     K_Sha512BlockSize
#define K_Sha224BlockSize     K_Sha256BlockSize
#define K_Sha1BlockSize       K_Sha256BlockSize

typedef struct {
  DWORD s_ikey_raw[K_Sha512BlockSize/4];
//  DWORD s_ikey[K_Sha512DigestSize/4];
  DWORD s_okey[K_Sha512DigestSize/4];
} T_HMAC_SHA_INIT;

typedef struct {
  DWORD d_id_type;
  DWORD d_tot_len;
  DWORD d_len;
  DWORD d_first;
  BYTE  s_block[K_Sha512BlockSize];
  union {
    DWORD s_h[K_Sha512DigestSize/4];
    T_HMAC_SHA_INIT s_hmac_init;
  };
} T_SHA_CONTEXT;

typedef T_SHA_CONTEXT      T_SHA512_CONTEXT;
typedef T_SHA_CONTEXT      T_SHA384_CONTEXT;
typedef T_SHA_CONTEXT      T_SHA256_CONTEXT;
typedef T_SHA_CONTEXT      T_SHA224_CONTEXT;
typedef T_SHA_CONTEXT      T_SHA1_CONTEXT;

// app header
typedef struct {
  DWORD p_signature;
  DWORD p_fntbl;
  DWORD p_virgin;
  DWORD p_appname;
  BYTE  b_version;
  BYTE  b_subver;
  BYTE  b_type;
  BYTE  b_appid;
  DWORD d_magic;
} T_APP_HDR;

// MMC
typedef struct {
  DWORD d_block_no;
  DWORD d_offset;
  BYTE *p_blk;
} T_MMC;

// ICC T0 & T1
typedef struct {
  BYTE    b_output;
  BYTE    b_class;
  BYTE    b_command;
  BYTE    b_p1;
  BYTE    b_p2;
  BYTE    b_p3;
  WORD    w_response;
  BYTE *  p_mblk;
} T_T0;

typedef struct {
  BYTE *  p_sad_buf;
  WORD    w_sad_len;
  BYTE    b_sad;
  BYTE    b_rfu1;
  BYTE *  p_dad_buf;
  WORD    w_dad_len;
  BYTE    b_dad;
  BYTE    b_rfu2;
} T_T1;

            /****** RSA define ******/

#define K_MaxKeyRsaModulusBits     4096     // Watch out it is double of K_MaxRsaModulusBits !!!!

// RSA public key define for EMV L2
typedef struct {
  DWORD  d_exponent;
  DWORD  d_keysize;
  DWORD  s_key[K_MaxKeyRsaModulusBits/32];
  DWORD  d_key_idx;
} T_KEY;

// RSA private key lengths
#define K_MaxRsaModulusBits     2048      // max bit length
#define K_MinRsaModulusBits     1024
#define K_KBMaxRsaKeyLen        256       // 2048-bit

#define K_MaxRsaModulusLen      ((K_MaxRsaModulusBits + 7) / 8)
#define K_MaxRsaPrimeBits      ((K_MaxRsaModulusBits + 1) / 2)
#define K_MaxRsaPrimeLen       ((K_MaxRsaPrimeBits + 7) / 8)

typedef struct {
  DWORD d_key_idx;
  DWORD d_bits;                                 /* length in bits of modulus */
  BYTE s_modulus[K_MaxRsaModulusLen];           /* modulus, right aligned with 0 filling */
  BYTE s_prime[2][K_MaxRsaPrimeLen];            /* prime factors, right aligned with 0 filling */
  BYTE s_prime_exponent[2][K_MaxRsaPrimeLen];   /* exponents for CRT, right aligned with 0 filling */
  BYTE s_coefficient[K_MaxRsaPrimeLen];         /* CRT coefficient, right aligned with 0 filling */
} T_RSA_PRIVATE_KEY;

typedef struct {
  DWORD  d_exponent;
  DWORD  d_keysize;
  DWORD  s_key[K_MaxRsaModulusBits/32];
} T_RSA_PUBLIC_KEY;

                /****** keydes DUKPT structure ******/

#define K_Dukpt                 0xFF
#define K_NotInjected           0xF1

#define K_NbrBaseKey            21

typedef struct {
  DWORD  d_good;
  BYTE   s_key[24];
} T_BASEKEY;

typedef struct {
  T_BASEKEY s_basekey[K_NbrBaseKey];
  DWORD     d_ECounter;
  DWORD     d_ShiftReg;
  DWORD     d_FutureKeyIdx;
  BYTE      s_KSN[10];
  WORD      w_key_len;      // 16 = double key len, 24 = triple key len, other = invalid
} T_DUKPT_KEYBANK;

                /****** TR-31 2010 similar key block ******/

typedef struct {              // make data blk multiple of 16
  BYTE s_deskey[24];
  DWORD d_des_pad;
} T_TDEA_KB;

typedef struct {              // make data blk multiple of 16
  T_RSA_PRIVATE_KEY  s_privkey;
  DWORD              d_priv_pad;
} T_RSA_PRIVATE_KB;

typedef struct {              // make data blk multiple of 16
  T_RSA_PUBLIC_KEY   s_pubkey;
  DWORD              d_pub_pad;
} T_RSA_PUBLIC_KB;

typedef struct {              // make data blk multiple of 16
  T_DUKPT_KEYBANK   s_dukptkey;
  DWORD             s_dukpt_pad[2];
} T_DUKPT_KB;

typedef struct {              // make data blk multiple of 16
  BYTE  s_aeskey[32];
  DWORD s_aes_pad[3];
} T_AES_KB;

typedef struct {              // make data blk multiple of 16
  BYTE  s_sm4key[16];
  DWORD s_sm4_pad[3];
} T_SM4_KB;

typedef struct {
  BYTE    s_kb_mac[8];      // cmac
  WORD    w_kb_len;
  WORD    w_key_idx;
  WORD    w_key_version;
  BYTE    b_algorithm;
  BYTE    b_usage;
  BYTE    s_kvc[2];         // used in K_KBAlgoritmTDea,K_KBAlgoritmAes & K_KBAlgoritmSm4 only, other should fill random
  WORD    w_hpad1;
  DWORD   d_hpad;
  WORD    w_key_len;
  WORD    w_dpad;           // DWORD align
  union {
    T_TDEA_KB         s_des_kb;
    T_DUKPT_KB        s_dukpt_kb;
    T_RSA_PRIVATE_KB  s_rsa_pri_kb;
    T_RSA_PUBLIC_KB   s_rsa_pub_kb;
    T_AES_KB          s_aes_kb;
    T_SM4_KB          s_sm4_kb;
  };
} T_KEY_BLOCK;

// T_KEY_BLOCK b_algorithm
enum {
    K_KBAlgorithmTDea,
    K_KBAlgorithmAes,
    K_KBAlgorithmSm4,
    K_KBAlgorithmDukpt,
    K_KBAlgorithmRsaPrivate,
    K_KBAlgorithmRsaPublic,
    K_KBAlgorithmEnd
};

// T_KEY_BLOCK b_usage
enum {
  K_KBEmpty,
  K_KBTmk,
  K_KBPin,
  K_KBMac,
  K_KBSignature,
  K_KBEncrypt,
  K_KBDecrypt,
  K_KBSslSignature    // this is for RSA SSL signatute only
  //K_KBBoth
};

// T_KEY_BLOCK key block size
#define K_KBDesLen      (sizeof(T_KEY_BLOCK) - sizeof(T_RSA_PRIVATE_KB) + sizeof(T_TDEA_KB))        // 56
#define K_KBAesLen      (sizeof(T_KEY_BLOCK) - sizeof(T_RSA_PRIVATE_KB) + sizeof(T_AES_KB))         // 72
#define K_KBSm4Len      (sizeof(T_KEY_BLOCK) - sizeof(T_RSA_PRIVATE_KB) + sizeof(T_SM4_KB))         // 56
#define K_KBDukptLen    (sizeof(T_KEY_BLOCK) - sizeof(T_RSA_PRIVATE_KB) + sizeof(T_DUKPT_KB))       // 648
#define K_KBRsaPrivLen  sizeof(T_KEY_BLOCK)                                                         // 936
#define K_KBRsaPubLen   (sizeof(T_KEY_BLOCK) - sizeof(T_RSA_PRIVATE_KB) + sizeof(T_RSA_PUBLIC_KB))  // 296

            /****** application header define ******/

typedef struct {
  DWORD d_cs32;
  DWORD d_disp_cs32;
  BYTE  s_agent_id[8];
  BYTE  s_app_name[13];
  BYTE  b_version;
  BYTE  b_subver;
  BYTE  b_type;
  WORD  w_appid;
} T_APP_INFO;

#define K_DllMask               0x01    // 0 = normal app, 1 = dll
#define K_Normal                0x00
#define K_Dll                   0x01

#define K_SignMask              0x06    // 00 = no sign
                                        // 01 = user sign
                                        // 10 = app sign
                                        // 11 = system sign
#define K_NoSign                0x00
#define K_UserSign              0x02
#define K_AppSign               0x04
#define K_SysSign               0x06

#define K_DevMask               0x08    // 0 = normal, 1 = device driver
#define K_DevDrv                0x08

                /****** rdll application interface structure define ******/
#define K_RdllResultError       0xff
#define K_RdllResultOK          0
#define K_RdllResultProcessing  1

#define K_RdllReqTypeMask       0x0F  // request type mask
#define K_RdllSysRequest        0x00
#define K_RdllAppRequest        0x01
#define K_RdllContMask          0xF0  // request control bits mask
#define K_RdllFlush             0x80

enum rdll_device_list
{
  K_RdllPppModem,
  K_RdllEthernet,
  K_RdllPppGprs,
  K_RdllWifi,
  K_RdllMax
};

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
typedef struct {
  BYTE  b_link_type;      // 0 = modem PPP; 1 = Ethernet; 2 = GPRS;
  BYTE  b_result;
  BYTE  b_d_type;         // bit 0 : will be filled by system
                          //   0 = system request; 1 = user app request
                          // bit 7 : filled by caller
                          //   0 = no delete temporary files before rdll start
                          //   1 = delete all temporary files before rdll start
  BYTE  b_app_type;
  BYTE  s_app_id[32];
  BYTE  s_phone1[35];
  BYTE  s_phone2[35];
  WORD  w_fport;          // in little endian
  DWORD d_fip;            // in big endian
  BYTE  s_ppp_user[16];
  BYTE  s_ppp_psw[16];
  BYTE  s_gprs_apn[32];
// currently not support at this moment
  BYTE  s_ftp_user[40];
  BYTE  s_ftp_psw[20];
  BYTE  s_ftp_dir[88];
  BYTE  s_wifi_ssid[32];
  BYTE  b_wifi_auth;
  BYTE  b_wifi_encr;
  BYTE  b_wifi_keyidx;
  BYTE  b_wifi_hex;
  BYTE  s_wifi_key[64];
  BYTE  s_rfu[200];
}
#ifndef __IAR_SYSTEMS_ICC__
__attribute__((packed))
#endif
T_RDLL_INF;
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

                /****** aux & mcom common define ******/

#define K_LinkStatus            0
//- error will be reset after reported and lower byte is the error
#define K_LinkTxEmpty           0x010000
#define K_LinkExist             0x008000
#define K_LinkOnLine            0x004000
#define K_LinkTxRdy             0x002000
#define K_LinkRxRdy             0x001000
#define K_LinkTxError           0x000800
#define K_LinkRxError           0x000400
#define K_LinkIntError          0x000200
#define K_LinkRxOverflow        0x000100

                /****** bufman define ******/
// used in manbuf alloc
#define K_SingleBuf             0
#define K_DoubleBuf             1
#define K_QuadBuf               2
#define K_SuperBuf              3

// actual status stored in buffer status
#define K_BufFree               0
#define K_BufSingle             1
#define K_BufDouble             2
#define K_BufQuad               3
#define K_BufSuper              4

#define K_MaxBuf                250//200//150
#define K_BufHdrLen             16
#define K_BufSize               760
#define K_BufOffset             64
#define K_TBufSize              (K_BufSize+K_BufOffset)
#define K_AppendSize            8
#define K_HeadSize              32
#define K_RfuSize               (K_HeadSize-K_BufHdrLen)
#define K_T2BufSize             (K_TBufSize*2+K_HeadSize+K_AppendSize)
#define K_T4BufSize             (K_TBufSize*4+(K_HeadSize+K_AppendSize)*3)
#define K_T8BufSize             (K_TBufSize*8+(K_HeadSize+K_AppendSize)*7)
#define K_2BufSize              (K_T2BufSize-K_BufOffset)
#define K_4BufSize              (K_T4BufSize-K_BufOffset)
#define K_8BufSize              (K_T8BufSize-K_BufOffset)

// b_type define
#define K_KeepTBuf              0x01

typedef struct {
  BYTE  b_status;
  BYTE  b_type;
  WORD  w_opt;
  WORD  w_offset;
  WORD  w_len;
  WORD  w_pid;                  // only in gprs cmux only
  WORD  w_rfu;
  DWORD d_next;
  BYTE  rfu[K_RfuSize];
  BYTE  s_buf[K_TBufSize+K_AppendSize];
} T_BUF;

/* opt define */
#define K_PppData               0x0001
/// bit0-3 store the nif number
#define K_BUFOPT_NIF_MASK   0x07

                /****** keydes mode ******/
#define K_SingleKeyLenDes       0
#define K_DoubleKeyLenDes       1
#define K_TripleKeyLenDes       2
#define K_PublicRsa             3
#define K_PrivateRsa            4

                /****** device driver define ******/

#define K_SwitchSystem          0
#define K_SwitchUser            1

// driver
enum {
  K_SdDevice,
  K_MsrDevice,
  K_IccDevice,
  K_LanDevice,
  K_LptDevice,
  K_CodecDevice,
  K_MifDevice,
  K_BarcodeDevice,
  K_NfcDevice,
  K_UsbFingerprintDevice,
  K_ImageSensorDevice,
  K_MAX_DEV
};
#define K_MaxDevice             (K_MAX_DEV+1)

// SD driver functions
enum {
  K_SdFOpen,
  K_SdFClose,
  K_SdFBlkSize,
  K_SdFSize,
  K_SdFBlkRead,
  K_SdFBlkWrite,
  K_SdFRead,
  K_SdFWrite,
  K_SdFPStatus,
  K_SdFHandler,
  K_SdFInserted,
  K_SdFEnabled
};

// lan driver functions
enum {
  K_LanFOpen,
  K_LanFClose,
  K_LanFRxRdy,
  K_LanFTxRdy,
  K_LanFRead,
  K_LanFSend,
  K_LanFStatus,
  K_LanFSetInstalled,
  K_LanFHandler1,
  K_LanFHandler2,
  K_LanFEnabled,
  K_LanFHandler3,
};

// Sam driver functions
enum {
  K_SamFOpen,
  K_SamFClose,
  K_SamFOn,
  K_SamFT1,
  K_SamFSelect,
  K_SamFOff,
  K_SamFChanged,
  K_SamFInserted,
  K_SamFActive
};

// msr driver functions
enum {
  K_MsrFOpen,
  K_MsrFClose,
  K_MsrFRxLen,
  K_MsrFGetc,
  K_MsrFRead,
  K_MsrFStatus,
  K_MsrFTrkHandler,
  K_MsrFTrkFull,
  K_MsrFTrackActive,
  K_MsrFEnabled,
  K_MsrTIOHandler,
};

// memory ICC driver functions
enum {
  K_IccFOpen,
  K_IccFClose,
  K_IccFIO,
  K_IccFHist,
  K_IccFEnabled
};

// lpt driver functions
enum {
  K_LptFOpen,
  K_LptFClose,
  K_LptFPutc,
  K_LptFPutdw,
  K_LptFStatus,
  K_LptFHandler,
  K_LptFShiftFinish,
  K_LptFEnabled,
  K_LptFResetCount,
  K_LptFGetCount
};

// codec driver functions
enum {
  K_CodecFActive,
  K_CodecFOpen,
  K_CodecFClose,
  K_CodecFPlay,
  K_CodecFPlayRam,
  K_CodecFStop,
  K_CodecFStatus,
  K_CodecFTxEnd,
  K_CodecFSetInstalled,
  K_CodecFHandler,
  K_CodecFEnabled
};

// Clicc driver functions
enum {
  K_CliccFInit,
  K_CliccFOpen,
  K_CliccFPoll,
  K_CliccFRemove,
  K_CliccFClose,
  K_CliccFSendCommand,
  K_CliccFHandler,
  K_CliccFCheckInstalled,
  K_CliccFEmvLv1,
  K_CliccFLedOpen,
  K_CliccFLedClose,
  K_CliccFLedStatus,
  K_CliccFLedSet,
  K_CliccFLedClear,
  K_CliccFLedOn,
  K_CliccFEnabled,
  K_CliccFCtrl,
  K_CliccFResumeIntr,
  K_CliccFDebug,
  K_CliccFNum,
};

// Mifare driver functions
enum {
  K_MIF_CHECK_INSTALLED = 0,
  K_MIF_HANDLER,
  K_MIF_INIT,
  K_MIF_OPEN,
  K_MIF_CLOSE,
  K_MIF_CANCEL,
  K_MIF_HALT,
  K_MIF_CARD_TYPE,
  K_MIF_TAG,
  K_MIF_STATUS,
  K_MIF_ANTICOLL,
  K_MIF_SELECT,
  K_MIF_LOAD_KEY,
  K_MIF_AUTH1,
  K_MIF_AUTH2,
  K_MIF_READ,
  K_MIF_WRITE,
  K_MIF_INC,
  K_MIF_DEC,
  K_MIF_RESTORE,
  K_MIF_TRANSFER,
  K_MIF_DEC_TRANSFER,
  K_MIF_SNO,
  K_MIF_SENDCOMMAND,
  K_MIF_GET,
  K_MIF_INFO_SET,
  K_MIF_INFO_GET,
  K_MIF_NO_OF_FUNC
};

// NFC Driver Function
enum{
  K_NFC_CHECK_INSTALLED,
  K_NFC_HANDLER,
  K_NFC_INIT,
  K_NFC_OPEN_FELICA,
  K_NFC_SEND_FELICA_COMMAND,
  K_NFC_CANCEL,
  K_NFC_CLOSE,
  K_NFC_GET_VAILD_RESP,
  K_NFC_INFO_GET,
  K_NFC_INFO_SET,
  K_NFC_IC_REG_DIRECT_WRITE,
  K_NFC_IC_REG_DIRECT_READ,
  K_NFC_NO_OF_FUNC
};

// Barcode driver functions
enum  {
  K_BarcodeFOpen,
  K_BarcodeFClose,
  K_BarcodeFCmd,
  K_BarcodeFScan,
  K_BarcodeFRead,
  K_BarcodeFAbort,
  K_BarcodeFStatus,
  K_BarcodeFEnabled,
  K_BarcodeNoOfFunc
};

// USB Fingerprint driver functions
enum  {
  K_UsbFprtOpen,
  K_UsbFprtClose,
  K_UsbFprtScan,
  K_UsbFprtInfoGet,
  K_UsbFprtInfoSet,
  K_UsbFprtNum
};

// Image Sensor driver functions
enum {
  K_ImgSensorOpen,
  K_ImgSensorGetFrame,
  K_ImgSensorGetFrameComplete,
  K_ImgSensorClose,
  K_ImgSensorLedOpen,
  K_ImgSensorIscHandler,
  K_ImgSensorDmaHandler,
  K_ImgSensorFreeze, // TBD
  K_ImgSensorInstalled,
  K_ImgSensorDbgRegWr,
  K_ImgSensorDbgRegRd,
  K_ImgSensorDbgFrameCnt,
  K_ImgSensorDbgVsyncCnt,
  K_ImgSensorDbgStatus,
  K_ImgSensorSetSysAddr,
  K_ImgSensorNoOfFunc
};

// system function for device driver
enum {
  K_SysDtimer0CheckExpired,
  K_SysDtimer0WaitExpired,
  K_SysDtimer0WatchSet,
  K_SysDtimer0WatchWait,
  K_SysDtimer0WatchCheck,
  K_SysDtimer1CheckExpired,
  K_SysDtimer1WaitExpired,
  K_SysDtimer1WatchSet,
  K_SysDtimer1WatchWait,
  K_SysDtimer1WatchCheck,
  K_SysInstall5ms,
  K_SysUninstall5ms,
  K_SysInstall10ms,
  K_SysUninstall10ms,
  K_SysInstall100ms,
  K_SysUninstall100ms,
  K_SysSpiRDequeue,
  K_SysSpiHandleFree,
  K_SysSpiNextTask,
  K_SysSpiHandleAlloc,
  K_SysSpiNextSeq,
  K_SysSpiSend,
  K_SysSpiREnqueue,
  K_SysSpiGetPtr,
  K_SysAddrAllow,
  K_SysVM2PhyAllocAddr,
  K_SysPhy2VMAllocAddr,
  K_SysSamActive,
  K_SysSwitchContext,
  K_SysSwitchMode,
  K_SysBufmanAlloc,
  K_SysBufmanFree,
  K_SysWdtReset,
  K_SysFontGet,
  K_SysFontUserGet,
  K_SysFontInfoGet,
  K_SysRtcGetRaw,
  K_SysMsrTrackActive,
  K_SysAdcEnable,
  K_SysAdcDisable,
  K_SysAdcRead,
  K_SysBattInstantLevel,
  K_SysBattLevel,
  K_SysGetContext,
  K_SysConfigRead,
  K_SysDispPutc,
  K_SysDispPutn,
  K_SysBattPauseCharging,
  K_SysGetDevConfig,
  K_SysSpiIdle,
  K_SysInvalidateData,
  K_SysExternalPower,
  K_SysDcrSet,
  K_SysDcrClear,
  K_SysDcrRead,
  K_SysDcrWrite,
  K_SysPmcPwrEnable,
  K_SysPmcPwrDisable,
  K_SysPmcBatteryCtrl,
  K_SysPmcBatteryStatus,
  K_SysSleEnabled,
  K_SysDtimerLatchStart,
  K_SysDtimerLatchStop,
  K_SysDtimerLatchTime,
  K_SysUsbHostSubmitBuffer,
  K_SysUsbHostSubmitControl,
  K_SysRandFill,
  K_SysGet5MsReminder,
  K_SysTwiRead,
  K_SysTwiWrite,
  K_SysDispHeoSetup,
  K_SysDispHeoEnable,
  K_SysDispHeoSetValidate,
  K_SysInvalidateAllocData,
  K_SysCleanAllocData,
  K_SysTwiInit,
  K_SysVM2PhyISCAddr,
  K_SysEnd
};

enum dev_config_list {
  K_DEV_CONFIG_RESERVED = 0,
  K_DEV_CONFIG_SAM,
  K_DEV_CONFIG_MDM,
  K_DEV_CONFIG_CLICC,
};

enum dev_config_sam_list {
  // SAM SLOT
  K_DEV_CONFIG_SAM_SLOT_SHIFT     = (0),
  K_DEV_CONFIG_SAM_SLOT_MASK      = (0x1f<<0),
  K_DEV_CONFIG_SAM_SLOT_0         = (0x01<<0),
  K_DEV_CONFIG_SAM_SLOT_1         = (0x02<<0),
  K_DEV_CONFIG_SAM_SLOT_2         = (0x04<<0),
  K_DEV_CONFIG_SAM_SLOT_3         = (0x08<<0),
  K_DEV_CONFIG_SAM_SLOT_4         = (0x10<<0),
  // SAM INV
  K_DEV_CONFIG_SAM_INV_SHIFT      = (5),
  K_DEV_CONFIG_SAM_INV_MASK       = (0x1f<<5),
  K_DEV_CONFIG_SAM_INV_0          = (0x01<<5),
  K_DEV_CONFIG_SAM_INV_1          = (0x02<<5),
  K_DEV_CONFIG_SAM_INV_2          = (0x04<<5),
  K_DEV_CONFIG_SAM_INV_3          = (0x08<<5),
  K_DEV_CONFIG_SAM_INV_4          = (0x10<<5),
};

enum dev_config_mdm_list {
  // MDM MODEL
  K_DEV_CONFIG_MDM_MODEL_SHIFT    = (0),
  K_DEV_CONFIG_MDM_MODEL_MASK     = (0x7<<0),
  // MDM MFG
  K_DEV_CONFIG_MDM_MFG_SHIFT      = (3),
  K_DEV_CONFIG_MDM_MFG_MASK       = (0x1<<3),
  K_DEV_CONFIG_MDM_MFG_CONEXANT   = (0x0<<3),
  K_DEV_CONFIG_MDM_MFG_SILAB      = (0x1<<3),
};

enum dev_config_clicc_list {
  // CLICC MODEL
  K_DEV_CONFIG_CLICC_MODEL_SHIFT          = (0),
  K_DEV_CONFIG_CLICC_MODEL_MASK           = (0x3<<0),
  K_DEV_CONFIG_CLICC_MODEL_NONE           = (0x0<<0),
  K_DEV_CONFIG_CLICC_MODEL_AS3911         = (0x1<<0),
  K_DEV_CONFIG_CLICC_MODEL_OM9663         = (0x2<<0),

  // CLICC BUS
  K_DEV_CONFIG_CLICC_BUS_SHIFT    = (2),
  K_DEV_CONFIG_CLICC_BUS_MASK     = (0x1<<2),
  K_DEV_CONFIG_CLICC_BUS_SPI      = (0x0<<2),
  K_DEV_CONFIG_CLICC_BUS_PARALLEL = (0x1<<2),
};

typedef struct {
  DWORD p_main;
  DWORD p_name;
  BYTE  b_version;
  BYTE  b_subver;
  BYTE  b_devid;
} T_DEV_HDR;


                /****** system hardware define ******/

#define K_MAIN_CLK        12000000
#define K_PCK_CLK         (498000000/3)         // peripheral basic clk
#define K_MCK_INITIAL     166000000
#define K_MCK_900_1800    166000000
#define K_MCK_850_1900    166000000
#define K_ICC_CLK         3580000

// cache define
#define K_CacheL                5
#define K_CacheLineLen          (1<<K_CacheL)

#define K_Pit1MS                (1000)
#define K_T1US                  (1)
#define K_T2US                  (2)
#define K_T5US                  (5)
#define K_T10US                 (10)
#define K_T20US                 (20)
#define K_T100US                (100)
#define K_T1MS                  (1000)
#define K_T5MS                  (5000)
#define K_T10MS                 (10000)

// Port redefine
#define PIOA_BASE               (&PIOA->PIO_IO_GROUP[0])
#define PIOB_BASE               (&PIOA->PIO_IO_GROUP[1])
#define PIOC_BASE               (&PIOA->PIO_IO_GROUP[2])
#define PIOD_BASE               (&PIOA->PIO_IO_GROUP[3])

#define pin_on(x,y)             ((x)->PIO_SODR = y)
#define pin_off(x,y)            ((x)->PIO_CODR = y)
#define pin_in(x,y)             ((x)->PIO_PDSR & (y))

#ifndef _UTIL_
// pin structure
typedef struct {
  PioIo_group *p_port_base;
  DWORD d_pos;
  DWORD d_peripheral;
} T_PIN;
#endif

// SPI0 command structure
#define K_SPIMask           0x07
#define K_SPILcd            0
#define K_SPILpt            1
  #define K_SPILptDummy     0x80
#define K_SPIClicc          2
#define K_SpiMax      3
#define K_SPIEmpty          0xff

// SPI1 command structure
#define K_SPIWifi           0

typedef struct {
  BYTE    b_idx;
  BYTE    b_state;
  WORD    w_seq;      // for caller use only
  DWORD   d_timeout;
  union {
    struct {
      BYTE *  p_cmd;
      BYTE *  p_sbuf;
      BYTE *  p_rbuf;
      WORD    w_slen;
      WORD    w_rlen;
      WORD    w_clen;
    };
    struct {
      DWORD   d_cmd;
      DWORD   d_arg;
    };
  };
} T_SPI;

enum {
 K_TWI_IDX_PMIC,
 K_TWI_IDX_IMG_SENS,
 K_TWI_IDX_NUM
};

// TWI command structure
#define K_TwiRWSuccess          0
#define K_TwiRWError            0x80000000
#define K_TwiBusy               0x40000000

#pragma pack(1)
typedef struct{
    BYTE dadr;
    BYTE iadr_len;
    DWORD iadr;
    BYTE data_len; // TODO only support data_len = 1
    BYTE data[1];
}T_TWI;
#pragma pack()

                /****** Touch Panel define ******/

typedef struct tpad_obj {
  WORD              w_hstart;
  WORD              w_vstart;
  WORD              w_hend;
  WORD              w_vend;
  FN                p_pen_detect;
  FN                p_pen_loss;
} T_TPAD_OBJ;

#define K_TPadDelAllObj       (-1)

#define K_TpadMul             1024
typedef struct {
  int     d_vval;
  int     d_hval;
  int     d_vslope;
  int     d_hslope;
  short   w_vpos;
  short   w_hpos;
} T_CAL_POINT;

                /****** system call list define ******/

enum system_call_list {
  OS_FN_GET_VERSION,                         /*   0 */
  OS_FN_RAND,                                /*   1 */
  OS_FN_RAND_UPDATE,                         /*   2 */
  OS_FN_KBD_OPEN,                            /*   3 */
  OS_FN_KBD_CLOSE,                           /*   4 */
  OS_FN_KBD_INKEY,                           /*   5 */
  OS_FN_KBD_SCODE,                           /*   6 */
  OS_FN_KBD_GETKEY,                          /*   7 */
  OS_FN_KBD_LOCK,                            /*   8 */
  OS_FN_KBD_UNLOCK,                          /*   9 */
  OS_FN_TIMER_OPEN,                          /*  10 */
  OS_FN_TIMER_CLOSE,                         /*  11 */
  OS_FN_TIMER_SET,                           /*  12 */
  OS_FN_TIMER_GET,                           /*  13 */
  OS_FN_BUZ_OPEN,                            /*  14 */
  OS_FN_BUZ_CLOSE,                           /*  15 */
  OS_FN_BUZ_BEEP,                            /*  16 */
  OS_FN_BUZ_STATE,                           /*  17 */
  OS_FN_RTC_GET,                             /*  18 */
  OS_FN_RTC_SET,                             /*  19 */
  OS_FN_DISP_OPEN,                           /*  20 */
  OS_FN_DISP_CLOSE,                          /*  21 */
  OS_FN_DISP_PUTC,                           /*  22 */
  OS_FN_DISP_PUTS,                           /*  23 */
  OS_FN_DISP_PUTN,                           /*  24 */
  OS_FN_DISP_GETG,                           /*  25 */
  OS_FN_DISP_PUTG,                           /*  26 */
  OS_FN_DISP_PUTP,                           /*  27 */
  OS_FN_DISP_GETP,                           /*  28 */
  OS_FN_DISP_LENGTH,                         /*  29 */
  OS_FN_DISP_GET_LCD_BUFFER,                 /*  30 */
  OS_FN_DISP_TEXTC,                          /*  31 */
  OS_FN_DISP_BACKC,                          /*  32 */
  OS_FN_DISP_BRIGHTNESS,                     /*  33 */
  OS_FN_DISP_BL_CONTROL,                     /*  34 */
  OS_FN_DISP_ICON_COLOR,                     /*  35 */
  OS_FN_DISP_ICON_PUTC,                      /*  36 */
  OS_FN_DISP_ICON_TEXTC,                     /*  37 */
  OS_FN_DISP_ICON_SET_SIGNAL_TYPE,           /*  38 */
  OS_FN_DISP_OVERLAY,                        /*  39 */
  OS_FN_COM1_OPEN,                           /*  40 */
  OS_FN_COM1_CLOSE,                          /*  41 */
  OS_FN_COM1_RXRDY,                          /*  42 */
  OS_FN_COM1_TXRDY,                          /*  43 */
  OS_FN_COM1_READ,                           /*  44 */
  OS_FN_COM1_SEND,                           /*  45 */
  OS_FN_COM1_STATUS,                         /*  46 */
  OS_FN_COM1_SREAD,                          /*  47 */
  OS_FN_COM1_SSEND,                          /*  48 */
  OS_FN_COM2_OPEN,                           /*  49 */
  OS_FN_COM2_CLOSE,                          /*  50 */
  OS_FN_COM2_RXRDY,                          /*  51 */
  OS_FN_COM2_TXRDY,                          /*  52 */
  OS_FN_COM2_READ,                           /*  53 */
  OS_FN_COM2_SEND,                           /*  54 */
  OS_FN_COM2_STATUS,                         /*  55 */
  OS_FN_COM2_SREAD,                          /*  56 */
  OS_FN_COM2_SSEND,                          /*  57 */
  OS_FN_COM3_OPEN,                           /*  58 */
  OS_FN_COM3_CLOSE,                          /*  59 */
  OS_FN_COM3_RXRDY,                          /*  60 */
  OS_FN_COM3_TXRDY,                          /*  61 */
  OS_FN_COM3_READ,                           /*  62 */
  OS_FN_COM3_SEND,                           /*  63 */
  OS_FN_COM3_STATUS,                         /*  64 */
  OS_FN_COM3_SREAD,                          /*  65 */
  OS_FN_COM3_SSEND,                          /*  66 */
  OS_FN_DESKEY,                              /*  67 */
  OS_FN_DES,                                 /*  68 */
  OS_FN_DES2,                                /*  69 */
  OS_FN_COM_DESKEY,                          /*  70 */
  OS_FN_COM_DES,                             /*  71 */
  OS_FN_COM_DES2,                            /*  72 */
  OS_FN_NEW_DES,                             /*  73 */
  OS_FN_NEW_DES2,                            /*  74 */
  OS_FN_RSA,                                 /*  75 */
  OS_FN_RSA_PRIVATE,                         /*  76 */
  OS_FN_AES_SETUP_KEY,                       /*  77 */
  OS_FN_AES_ENCRYPT_BLOCK,                   /*  78 */
  OS_FN_AES_DECRYPT_BLOCK,                   /*  79 */
  OS_FN_SHA1,                                /*  80 */
  OS_FN_MD5_INIT,                            /*  81 */
  OS_FN_MD5_PROCESS,                         /*  82 */
  OS_FN_MD5_FINISH,                          /*  83 */
  OS_FN_SHA_INIT,                            /*  84 */
  OS_FN_SHA_PROCESS,                         /*  85 */
  OS_FN_SHA_FINISH,                          /*  86 */
  OS_FN_RC4_SETUP,                           /*  87 */
  OS_FN_RC4_ENCRYPT,                         /*  88 */
  OS_FN_MD4_PROCESS,                         /*  89 */
  OS_FN_MD4_FINISH,                          /*  90 */
  OS_FN_SM2_SET_CURVE,                       /*  91 */
  OS_FN_SM2_FREE_CURVE,                      /*  92 */
  OS_FN_SM2_GET_SIGNATURE,                   /*  93 */
  OS_FN_SM2_VERIFY_SIGNATURE,                /*  94 */
  OS_FN_SM2_ENCRYPT,                         /*  95 */
  OS_FN_SM2_DECRYPT,                         /*  96 */
  OS_FN_SM2_GET_MESS,                        /*  97 */
  OS_FN_SM3_ASCII,                           /*  98 */
  OS_FN_SM3,                                 /*  99 */
  OS_FN_SM4_SET_KEY,                         /* 100 */
  OS_FN_SM4_ENCRYPT,                         /* 101 */
  OS_FN_SM4_DECRYPT,                         /* 102 */
  OS_FN_KEY_SAVE,                            /* 103 */
  OS_FN_KEY_SAVE_TMK,                        /* 104 */
  OS_FN_KEY_DES,                             /* 105 */
  OS_FN_KEY_RSA,                             /* 106 */
  OS_FN_KEY_FLUSH,                           /* 107 */
  OS_FN_KEY_DUKPT_ENCRYPT,                   /* 108 */
  OS_FN_KEY_DUKPT_MAC_CAL,                   /* 109 */
  OS_FN_KEY_CUP_MAC,                         /* 110 */
  OS_FN_KEY_CBC_MAC,                         /* 111 */
  OS_FN_KEY_HASH,                            /* 112 */
  OS_FN_KEY_SAVE_NC,                         /* 113 */
  OS_FN_KEY_SAVE_TMK_NC,                     /* 114 */
  OS_FN_KEY_SAVE_TMK_CBC,                    /* 115 */
  OS_FN_KEY_SAVE_TMK_CBC_NC,                 /* 116 */
  OS_FN_PCI_DSS_GET_KVC,                     /* 117 */
  OS_FN_PCI_DSS_REGEN_KEY,                   /* 118 */
  OS_FN_PCI_DSS,                             /* 119 */
  OS_FN_THREAD_OPEN,                         /* 120 */
  OS_FN_THREAD_CLOSE,                        /* 121 */
  OS_FN_THREAD_CREATE,                       /* 122 */
  OS_FN_THREAD_DELETE,                       /* 123 */
  OS_FN_THREAD_LOCK,                         /* 124 */
  OS_FN_THREAD_UNLOCK,                       /* 125 */
  OS_FN_THREAD_PUSH,                         /* 126 */
  OS_FN_THREAD_POP,                          /* 127 */
  OS_FN_THREAD_RETURN,                       /* 128 */
  OS_FN_BUFMAN_ALLOC,                        /* 129 */
  OS_FN_BUFMAN_FREE,                         /* 130 */
  OS_FN_BUFMAN_REALLOC,                      /* 131 */
  OS_FN_BUFMAN_SHRINK,                       /* 132 */
  OS_FN_LED_OPEN,                            /* 133 */
  OS_FN_LED_CLOSE,                           /* 134 */
  OS_FN_LED_STATUS,                          /* 135 */
  OS_FN_LED_SET,                             /* 136 */
  OS_FN_LED_CLEAR,                           /* 137 */
  OS_FN_LED_ON,                              /* 138 */
  OS_FN_GET_CHALLENGE,                       /* 139 */
  OS_FN_AUTH,                                /* 140 */
  OS_FN_APP_SEARCH_ID,                       /* 141 */
  OS_FN_APP_SEARCH_NAME,                     /* 142 */
  OS_FN_APP_GET_VIRGIN,                      /* 143 */
  OS_FN_APP_CLR_VIRGIN,                      /* 144 */
  OS_FN_APP_INFO,                            /* 145 */
  OS_FN_MALLOC,                              /* 146 */
  OS_FN_REALLOC,                             /* 147 */
  OS_FN_FREE,                                /* 148 */
  OS_FN_FILE_CREATE_OWNER,                   /* 149 */
  OS_FN_FILE_OPEN,                           /* 150 */
  OS_FN_FILE_CREATE,                         /* 151 */
  OS_FN_FILE_CLOSE,                          /* 152 */
  OS_FN_FILE_EXIST,                          /* 153 */
  OS_FN_FILE_RENAME,                         /* 154 */
  OS_FN_FILE_DELETE,                         /* 155 */
  OS_FN_FILE_FLUSHED,                        /* 156 */
  OS_FN_FILE_SEEK,                           /* 157 */
  OS_FN_FILE_READ,                           /* 158 */
  OS_FN_FILE_WRITE,                          /* 159 */
  OS_FN_FILE_PUTC,                           /* 160 */
  OS_FN_FILE_GETC,                           /* 161 */
  OS_FN_FILE_LENGTH,                         /* 162 */
  OS_FN_FILE_END,                            /* 163 */
  OS_FN_FILE_SEARCH,                         /* 164 */
  OS_FN_FILE_DSEARCH,                        /* 165 */
  OS_FN_FILE_DSEARCH_ALL,                    /* 166 */
  OS_FN_FILE_DEACTIVATE,                     /* 167 */
  OS_FN_FILE_ACTIVATE,                       /* 168 */
  OS_FN_FILE_GET_NAME,                       /* 169 */
  OS_FN_FILE_TRUNCATE,                       /* 170 */
  OS_FN_FILE_AUTH,                           /* 171 */
  OS_FN_FILE_SET_LOCK,                       /* 172 */
  OS_FN_FILE_GET_FREE_SPACE,                 /* 173 */
  OS_FN_FILE_FREESPACE,                      /* 174 */
  OS_FN_FILE_GET_SYSTEMINFO,                 /* 175 */
  OS_FN_FILE_FTRUNCATE,                      /* 176 */
  OS_FN_FILE_INIT_SEARCH,                    /* 177 */
  OS_FN_FILE_READDIR,                        /* 178 */
  OS_FN_FILE_MKDIR,                          /* 179 */
  OS_FN_FILE_RMDIR,                          /* 180 */
  OS_FN_FILE_GET_LONGNAME,                   /* 181 */
  OS_FN_FILE_SYNC,                           /* 182 */
  OS_FN_FILE_IOCTL,                          /* 183 */
  OS_FN_CONFIG_LENGTH,                       /* 184 */
  OS_FN_CONFIG_READ,                         /* 185 */
  OS_FN_CONFIG_WRITE,                        /* 186 */
  OS_FN_CONFIG_UPDATE,                       /* 187 */
  OS_FN_SAM_OPEN,                            /* 188 */
  OS_FN_SAM_CLOSE,                           /* 189 */
  OS_FN_SAM_SELECT,                          /* 190 */
  OS_FN_SAM_INSERTED,                        /* 191 */
  OS_FN_SAM_ON,                              /* 192 */
  OS_FN_SAM_OFF,                             /* 193 */
  OS_FN_SAM_T1,                              /* 194 */
  OS_FN_SAM_CHANGE,                          /* 195 */
  OS_FN_MMC_OPEN,                            /* 196 */
  OS_FN_MMC_CLOSE,                           /* 197 */
  OS_FN_MMC_BLK_SIZE,                        /* 198 */
  OS_FN_MMC_SIZE,                            /* 199 */
  OS_FN_MMC_PSTATUS,                         /* 200 */
  OS_FN_MMC_BLK_READ,                        /* 201 */
  OS_FN_MMC_BLK_WRITE,                       /* 202 */
  OS_FN_MMC_READ,                            /* 203 */
  OS_FN_MMC_WRITE,                           /* 204 */
  OS_FN_MMC_INSERTED,                        /* 205 */
  OS_FN_SLE_OPEN,                            /* 206 */
  OS_FN_SLE_CLOSE,                           /* 207 */
  OS_FN_SLE_HIST,                            /* 208 */
  OS_FN_SLE_IO,                              /* 209 */
  OS_FN_LAN_OPEN,                            /* 210 */
  OS_FN_LAN_CLOSE,                           /* 211 */
  OS_FN_LAN_RXRDY,                           /* 212 */
  OS_FN_LAN_TXRDY,                           /* 213 */
  OS_FN_LAN_READ,                            /* 214 */
  OS_FN_LAN_SEND,                            /* 215 */
  OS_FN_LAN_STATUS,                          /* 216 */
  OS_FN_MSR_OPEN,                            /* 217 */
  OS_FN_MSR_CLOSE,                           /* 218 */
  OS_FN_MSR_RXLEN,                           /* 219 */
  OS_FN_MSR_GETC,                            /* 220 */
  OS_FN_MSR_READ,                            /* 221 */
  OS_FN_MSR_STATUS,                          /* 222 */
  OS_FN_LPT_OPEN,                            /* 223 */
  OS_FN_LPT_CLOSE,                           /* 224 */
  OS_FN_LPT_PUTC,                            /* 225 */
  OS_FN_LPT_PUTDW,                           /* 226 */
  OS_FN_LPT_STATUS,                          /* 227 */
  OS_FN_USBSER_OPEN,                         /* 228 */
  OS_FN_USBSER_CLOSE,                        /* 229 */
  OS_FN_USBSER_READ,                         /* 230 */
  OS_FN_USBSER_WRITE,                        /* 231 */
  OS_FN_USBSER_STATUS,                       /* 232 */
  OS_FN_USBSER_ENABLE,                       /* 233 */
  OS_FN_FL_INFO,                             /* 234 */
  OS_FN_FL_STATUS,                           /* 235 */
  OS_FN_FL_PAGE_READ,                        /* 236 */
  OS_FN_FL_PAGE,                             /* 237 */
  OS_FN_FL_PGM,                              /* 238 */
  OS_FN_FL_PGM_START,                        /* 239 */
  OS_FN_FL_WRITE,                            /* 240 */
  OS_FN_FL_PGM_END,                          /* 241 */
  OS_FN_FL_ERASE_START,                      /* 242 */
  OS_FN_FL_ERASE_END,                        /* 243 */
  OS_FN_FL_COPYB_START,                      /* 244 */
  OS_FN_FL_COPYB_PGM,                        /* 245 */
  OS_FN_FL_COPYB_END,                        /* 246 */
  OS_FN_FL_CREAD_RAND,                       /* 247 */
  OS_FN_FL_CREAD,                            /* 248 */
  OS_FN_FL_CREAD_END,                        /* 249 */
  OS_FN_FL_COPYBACK,                         /* 250 */
  OS_FN_MDM_OPEN,                            /* 251 */
  OS_FN_MDM_CLOSE,                           /* 252 */
  OS_FN_MDM_DIAL,                            /* 253 */
  OS_FN_MDM_DISC,                            /* 254 */
  OS_FN_MDM_CID,                             /* 255 */
  OS_FN_MDM_STATUS,                          /* 256 */
  OS_FN_MDM_LAST_CONN_STAT,                  /* 257 */
  OS_FN_MCOM_OPEN,                           /* 258 */
  OS_FN_MCOM_CLOSE,                          /* 259 */
  OS_FN_MCOM_PERMIT,                         /* 260 */
  OS_FN_MCOM_RXRDY,                          /* 261 */
  OS_FN_MCOM_TXRDY,                          /* 262 */
  OS_FN_MCOM_SREAD,                          /* 263 */
  OS_FN_MCOM_SSEND,                          /* 264 */
  OS_FN_MCOM_STATUS,                         /* 265 */
  OS_FN_MCOM_READ,                           /* 266 */
  OS_FN_MCOM_SEND,                           /* 267 */
  OS_FN_BATT_LEVEL,                          /* 268 */
  OS_FN_BATT_TEMP,                           /* 269 */
  OS_FN_BATT_CONTROL,                        /* 270 */
  OS_FN_BATT_CHARGE_MODE,                    /* 271 */
  OS_FN_ADC_CH_ENABLE,                       /* 272 */
  OS_FN_ADC_CH_DISABLE,                      /* 273 */
  OS_FN_ADC_CH_READ,                         /* 274 */
  OS_FN_GPRS_SIGNAL,                         /* 275 */
  OS_FN_GPRS_STATUS,                         /* 276 */
  OS_FN_GPRS_AT_CMD,                         /* 277 */
  OS_FN_GPRS_AT_RESP,                        /* 278 */
  OS_FN_GPRS_SEND,                           /* 279 */
  OS_FN_GPRS_READ,                           /* 280 */
  OS_FN_GPRS_RESET,                          /* 281 */
  OS_FN_GPRS_CONTROL,                        /* 282 */
  OS_FN_GPRS_GET_CID,                        /* 283 */
  OS_FN_GPRS_READ_USSD,                      /* 284 */
  OS_FN_GPRS_SMS_DECODE,                     /* 285 */
  OS_FN_GPRS_SMS_ACK,                        /* 286 */
  OS_FN_GPRS_SMS_SEND,                       /* 287 */
  OS_FN_GPRS_SMS_CONTROL,                    /* 288 */
  OS_FN_GPRS_SMS_READ,                       /* 289 */
  OS_FN_TPAD_OPEN,                           /* 290 */
  OS_FN_TPAD_CLOSE,                          /* 291 */
  OS_FN_TPAD_ADD_OBJ,                        /* 292 */
  OS_FN_TPAD_DEL_OBJ,                        /* 293 */
  OS_FN_TPAD_CPOS,                           /* 294 */
  OS_FN_TPAD_ALERT_RETURN,                   /* 295 */
  OS_FN_TPAD_CALIBRATE,                      /* 296 */
  OS_FN_TPAD_RAW_POS,                        /* 297 */
  OS_FN_CODEC_OPEN,                          /* 298 */
  OS_FN_CODEC_CLOSE,                         /* 299 */
  OS_FN_CODEC_PLAY ,                         /* 300 */
  OS_FN_CODEC_PLAY_RAM,                      /* 301 */
  OS_FN_CODEC_STOP,                          /* 302 */
  OS_FN_CODEC_STATUS,                        /* 303 */
  OS_FN_CLICC_OPEN,                          /* 304 */
  OS_FN_CLICC_POLL,                          /* 305 */
  OS_FN_CLICC_REMOVE,                        /* 306 */
  OS_FN_CLICC_CLOSE,                         /* 307 */
  OS_FN_CLICC_SENDCOMMAND,                   /* 308 */
  OS_FN_CLICC_CTRL,                          /* 309 */
  OS_FN_CLICC_EMV_LV1,                       /* 310 */
  OS_FN_CLICC_LED_OPEN,                      /* 311 */
  OS_FN_CLICC_LED_CLOSE,                     /* 312 */
  OS_FN_CLICC_LED_STATUS,                    /* 313 */
  OS_FN_CLICC_LED_SET,                       /* 314 */
  OS_FN_CLICC_LED_CLEAR,                     /* 315 */
  OS_FN_CLICC_LED_ON,                        /* 316 */
  OS_FN_USBHOST_SUBMIT_BUFFER,               /* 317 */
  OS_FN_USBHOST_UNLINK_BUFFER,               /* 318 */
  OS_FN_USBHOST_RESET_DEVICE,                /* 319 */
  OS_FN_USBHOST_STATUS,                      /* 320 */
  OS_FN_USBHOST_RHPORT_STATUS,               /* 321 */
  OS_FN_USBHOST_DEVINFO,                     /* 322 */
  OS_FN_USBHOST_DEVICEMAP,                   /* 323 */
  OS_FN_DRIVER_REGISTER,                     /* 324 */
  OS_FN_DRIVER_UNREGISTER,                   /* 325 */
  OS_FN_DEVICE_ALLOC,                        /* 326 */
  OS_FN_DEVICE_REGISTER,                     /* 327 */
  OS_FN_DEVICE_UNREGISTER,                   /* 328 */
  OS_FN_DEVICE_ALLOC_MEDIA,                  /* 329 */
  OS_FN_DEVICE_IOCTL,                        /* 330 */
  OS_FN_DEVICE_SEARCH_INIT,                  /* 331 */
  OS_FN_DEVICE_SEARCH_NEXT,                  /* 332 */
  OS_FN_DEVICE_OPEN,                         /* 333 */
  OS_FN_DEVICE_MOUNT,                        /* 334 */
  OS_FN_DEVICE_UNMOUNT,                      /* 335 */
  OS_FN_INTERRUPT_REGISTER,                  /* 336 */
  OS_FN_INTERRUPT_UNREGISTER,                /* 337 */
  OS_FN_BUS_START,                           /* 338 */
  OS_FN_BUS_STOP,                            /* 339 */
  OS_FN_USBOTG_GET_STATE,                    /* 340 */
  OS_FN_NETDEV_OPEN,                         /* 341 */
  OS_FN_NETDEV_CLOSE,                        /* 342 */
  OS_FN_NETDEV_GETLINK,                      /* 343 */
  OS_FN_NETDEV_EVENT,                        /* 344 */
  OS_FN_NETDEV_IOCTL,                        /* 345 */
  OS_FN_USBKBD_OPEN,                         /* 346 */
  OS_FN_USBKBD_CLOSE,                        /* 347 */
  OS_FN_USBKBD_INKEY,                        /* 348 */
  OS_FN_USBKBD_GETKEY,                       /* 349 */
  OS_FN_TAMPER_ON,                           /* 350 */
  OS_FN_TAMPER_GET_STATUS,                   /* 351 */
  OS_FN_TAMPER_RESET_STATUS,                 /* 352 */
  OS_FN_MIF_OPEN,                            /* 353 */
  OS_FN_MIF_CLOSE,                           /* 354 */
  OS_FN_MIF_CANCEL,                          /* 355 */
  OS_FN_MIF_HALT,                            /* 356 */
  OS_FN_MIF_CARD_TYPE,                       /* 357 */
  OS_FN_MIF_TAG,                             /* 358 */
  OS_FN_MIF_STATUS,                          /* 359 */
  OS_FN_MIF_ANTICOLL,                        /* 360 */
  OS_FN_MIF_SELECT,                          /* 361 */
  OS_FN_MIF_LOAD_KEY,                        /* 362 */
  OS_FN_MIF_AUTH1,                           /* 363 */
  OS_FN_MIF_AUTH2,                           /* 364 */
  OS_FN_MIF_READ,                            /* 365 */
  OS_FN_MIF_WRITE,                           /* 366 */
  OS_FN_MIF_INC,                             /* 367 */
  OS_FN_MIF_DEC,                             /* 368 */
  OS_FN_MIF_RESTORE,                         /* 369 */
  OS_FN_MIF_TRANSFER,                        /* 370 */
  OS_FN_MIF_DEC_TRANS,                       /* 371 */
  OS_FN_MIF_SNO,                             /* 372 */
  OS_FN_MIF_SENDCMD,                         /* 373 */
  OS_FN_MIF_GET,                             /* 374 */
  OS_FN_MIF_INFO_SET,                        /* 375 */
  OS_FN_MIF_INFO_GET,                        /* 376 */
  OS_FN_BARCODE_OPEN,                        /* 377 */
  OS_FN_BARCODE_CLOSE,                       /* 378 */
  OS_FN_BARCODE_CMD,                         /* 379 */
  OS_FN_BARCODE_SCAN,                        /* 380 */
  OS_FN_BARCODE_READ,                        /* 381 */
  OS_FN_BARCODE_ABORT,                       /* 382 */
  OS_FN_BARCODE_STATUS,                      /* 383 */
  OS_FN_BARCODE_ENABLED,                     /* 384 */
  OS_FN_WIFI_CONTROL,                        /* 385 */
  OS_FN_WIFI_SEND_COMMAND,                   /* 386 */
  OS_FN_WIFI_RECV_RESPONSE,                  /* 387 */
  OS_FN_WIFI_STATUS,                         /* 388 */
  OS_FN_WIFI_PROFILES_READ,                  /* 389 */
  OS_FN_WIFI_PROFILES_WRITE,                 /* 390 */
  OS_FN_NFC_OPEN_FELICA,                     /* 391 */
  OS_FN_NFC_SEND_FELICA_COMMAND,             /* 392 */
  OS_FN_NFC_CANCEL,                          /* 393 */
  OS_FN_NFC_CLOSE,                           /* 394 */
  OS_FN_NFC_GET_VALID_RESP,                  /* 395 */
  OS_FN_NFC_INFO_GET,                        /* 396 */
  OS_FN_NFC_INFO_SET,                        /* 397 */
  OS_FN_NFC_IC_REG_DIRECT_WRITE,             /* 398 */
  OS_FN_NFC_IC_REG_DIRECT_READ,              /* 399 */
  OS_FN_GPS_ON,                              /* 400 */
  OS_FN_GPS_OFF,                             /* 401 */
  OS_FN_GPS_GET_INFO,                        /* 402 */
  OS_FN_GPRS_GET_GPS_INFO,                   /* 403 */
  OS_FN_GPS_CMD,                             /* 404 */
  OS_FN_GPS_RESP,                            /* 405 */
  OS_FN_GPS_RESET,                           /* 406 */
  OS_FN_GPS_ENABLED,                         /* 407 */
  OS_FN_BREG_READ,                           /* 408 */
  OS_FN_BREG_WRITE,                          /* 409 */
  OS_FN_APPDL_WRITE_APP,                     /* 410 */
  OS_FN_GET_CALLID,                          /* 411 */
  OS_FN_SLEEP,                               /* 412 */
  OS_FN_POWER_SLEEP,                         /* 413 */
  OS_FN_RESET,                               /* 414 */
  OS_FN_FREE_RUN,                            /* 415 */
  OS_FN_HD_CONFIG,                           /* 416 */
  OS_FN_RDLL_APP,                            /* 417 */
  OS_FN_DUMMY,                               /* 418 */
  OS_FN_OFF,                                 /* 419 */
  OS_FN_DMK_KVC_READ,                        /* 420 */
  OS_FN_DLL_REQ,                             /* 421 */
  OS_FN_GET_MCLK,                            /* 422 */
  OS_FN_RMB_CONTROL,                         /* 423 */
  OS_FN_APPCALL_RETURN,                      /* 424 */
  OS_FN_POWER,                               /* 425 */
  OS_FN_CLICC_DEBUG,                         /* 426 */
  OS_FN_DMK_INJECT,                          /* 427 */
  OS_FN_DTIMER_LATCH_START,                  /* 428 */
  OS_FN_DTIMER_LATCH_STOP,                   /* 429 */
  OS_FN_DTIMER_LATCH_TIME,                   /* 430 */
  OS_FN_LPT_RESET_COUNT,                     /* 431 */
  OS_FN_LPT_GET_COUNT,                       /* 432 */
  OS_FN_FPRT_OPEN,                           /* 433 */
  OS_FN_FPRT_CLOSE,                          /* 434 */
  OS_FN_FPRT_SCAN_IMG,                       /* 435 */
  OS_FN_FPRT_INFO_SET,                       /* 436 */
  OS_FN_FPRT_INFO_GET,                       /* 437 */
  OS_FN_ECC_KEYPAIR_GENERATE,                /* 438 */
  OS_FN_ECC_PUBKEY_VERIFY,                   /* 439 */
  OS_FN_ECC_SCALAR_MULTIPLY,                 /* 440 */
  OS_FN_GCM_AES_OPEN,                        /* 441 */
  OS_FN_GCM_AES_INIT,                        /* 442 */
  OS_FN_GCM_AES_PROCESS_AAD,                 /* 443 */
  OS_FN_GCM_AES_PROCESS,                     /* 444 */
  OS_FN_GCM_AES_FINISH,                      /* 445 */
  OS_FN_GCM_AES_CLOSE,                       /* 446 */
  OS_FN_SYS_THREAD_CREATE,                   /* 447 */  
  OS_FN_WIFI_SOCKET_CONNECT,                 /* 448 */
  OS_FN_WIFI_SOCKET_DISCONNECT,              /* 449 */
  OS_FN_WIFI_SOCKET_GET_STATE,               /* 450 */
  OS_FN_WIFI_SOCKET_SEND_TCP,                /* 451 */
  OS_FN_WIFI_SOCKET_SEND_UDP,                /* 452 */
  OS_FN_WIFI_GET_DEVICE_STATE,               /* 453 */
  OS_FN_WIFI_SOCKET_GET_STATUS,              /* 454 */
  OS_FN_WIFI_SOCKET_LISTEN,                  /* 455 */  
  OS_FN_IMG_SENSOR_OPEN,                     /* 456 */
  OS_FN_IMG_SENSOR_GET_FRAME,                /* 457 */
  OS_FN_IMG_SENSOR_GET_FRAME_COMPLETE,       /* 458 */
  OS_FN_IMG_SENSOR_CLOSE,                    /* 459 */
  OS_FN_GPRS_IS_HL6RD,                       /* 460 */  // HL6RD|HL6 compatible
  OS_FN_GPRS_IS_HL6RD_OLD_FRM,               /* 461 */  // HL6RD|HL6 compatible
  OS_FN_GPRS_SET_PIN_STATUS,                 /* 462 */
  OS_FN_IMG_SENSOR_LED_OPEN,                 /* 463 */
  OS_FN_DISP_PUTS_ANYWHERE,                  /* 464 */  // ECR17-0011: disp anywhere
  OS_FN_BATT_CHARGE_NOW,                     /* 465 */
  OS_FN_WIFI_GET_REGULATORY_DOMAIN,          /* 467 */
  OS_FN_WIFI_SET_REGULATORY_DOMAIN,          /* 468 */  
  OS_FN_TAMPER_UNLOCK,                       /* 469 */
  END_OF_OS_FN_LIST
};

                /****** system func routine ******/

// BYTE * os_get_version(BYTE * a_ptr);
#define os_get_version(x)         sys_func1(OS_FN_GET_VERSION,(DWORD)(x))

                /****** keyboard routine ******/

#define K_Key0                  '0'
#define K_Key1                  '1'
#define K_Key2                  '2'
#define K_Key3                  '3'
#define K_Key4                  '4'
#define K_Key5                  '5'
#define K_Key6                  '6'
#define K_Key7                  '7'
#define K_Key8                  '8'
#define K_Key9                  '9'
#define K_KeyEnter              0x3a
#define K_KeyBackSp             0x3b
#define K_KeyClr                K_KeyBackSp
#define K_KeyCancel             0x3c
#define K_KeyLeft               0x3d
#define K_KeyAlpha              K_KeyLeft
#define K_KeyRight              0x3e
#define K_KeyCapital            K_KeyRight
#define K_KeyDown               0x3f
#define K_KeyUp                 0x40
#define K_KeySharp              0x41
#define K_KeyAsterisk           0x42
#define K_KeyPower              0x43
#define K_KeyNull               0


// BOOLEAN os_kbd_open(void * a_init_str);
#define os_kbd_open(x)            sys_func1(OS_FN_KBD_OPEN,(DWORD)(x))

// void os_kbd_close();
#define os_kbd_close()            sys_func0(OS_FN_KBD_CLOSE)

// BYTE os_kbd_inkey();
#define os_kbd_inkey()            sys_func0(OS_FN_KBD_INKEY)

// BYTE os_kbd_scode();
#define os_kbd_scode()            sys_func0(OS_FN_KBD_SCODE)

// BYTE os_kbd_getkey();
#define os_kbd_getkey()           sys_func0(OS_FN_KBD_GETKEY)

// DWORD os_kbd_lock();   // security function
#define os_kbd_lock()             (int)sys_func0(OS_FN_KBD_LOCK)

// void os_kbd_unlock();  // security function
#define os_kbd_unlock()           sys_func0(OS_FN_KBD_UNLOCK)

                /****** timer routine ******/

#define K_NoTimerAvailable        0xff
// DWORD os_timer_open();
#define os_timer_open()           sys_func0(OS_FN_TIMER_OPEN)

// void os_timer_close(DWORD a_idx);
#define os_timer_close(x)         sys_func1(OS_FN_TIMER_CLOSE,(DWORD)(x))

// void os_timer_set(DWORD a_idx,DWORD a_value);
#define os_timer_set(x,y)         sys_func2(OS_FN_TIMER_SET,(DWORD)(x),(DWORD)(y))

// DWORD os_timer_get(DWORD a_idx);
#define os_timer_get(x)           sys_func1(OS_FN_TIMER_GET,(DWORD)(x))

                /****** beeper routine ******/

// BOOLEAN os_beep_open(void);
#define os_beep_open()            sys_func0(OS_FN_BUZ_OPEN)

// void os_beep_close();
#define os_beep_close()           sys_func0(OS_FN_BUZ_CLOSE)

// void os_beep_sound(WORD a_ontime,WORD a_offtime,WORD a_count);
#define os_beep_sound(x,y,z)      sys_func3(OS_FN_BUZ_BEEP,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// void os_beep_state(DWORD a_state);
#define os_beep_state(x)          sys_func1(OS_FN_BUZ_STATE,(DWORD)(x))

                /****** rtc routine ******/

// BOOLEAN os_rtc_get(BYTE * a_dtg);
#define os_rtc_get(x)             sys_func1(OS_FN_RTC_GET,(DWORD)(x))

// BOOLEAN os_rtc_set(BYTE * a_dtg);
#define os_rtc_set(x)             sys_func1(OS_FN_RTC_SET,(DWORD)(x))

                /****** display routine ******/

#define K_DispBlOnAlways        0xff        // always on
#define K_DispBlOffAlways       0           // always off
#define K_DispBlOnNow           0x100       // on now
#define K_DispBrightnessDef     5
#define K_DispBrightnessMax     7

// For color display
#define K_DispTimeoutDef        10          // default LCD bl off time
#define K_DispBlOffNow          0x101       // off now
#define K_DispWidthInPixel      320
#define K_DispHeightInPixel     240
#define K_DispMemorySize        (K_DispWidthInPixel*K_DispHeightInPixel*2)
#define K_LogoSize              (K_DispWidthInPixel*(K_DispHeightInPixel-24)*2+8)
#define K_LandZoneSize          (200*120*2+8)
#define K_DispDefBackC          0xF656      // blue grey color same as CREON with white color LED
#define K_DispVerticalOffset    24

// For BW display
#define K_DispTimeoutDefBW      15          // default LCD bl off time
#define K_DispContOff           0
#define K_DispContDef           18
#define K_DispContMax           63

#define K_CurRight              1
#define K_CurLeft               2
#define K_CurDown               3
#define K_CurUp                 4
#define K_DelEol                5
#define K_EnableMoreLine        6
#define K_DisableMoreLine       0x19
#define K_Bell                  7
#define K_BackSp                8
#define K_NewLine               10
#define K_CurHome               11
#define K_ClrHome               12
#define K_CReturn               13
#define K_PushCursor            14
#define K_PopCursor             15
#define K_Esc                   27
#define K_RevOn                 0x1C
#define K_RevOff                0x1D
#define K_SelSmFont             0x10
#define K_SelBigFont            0x11
#define K_SelSupBigFont         0x09
#define K_SelSpFont             0x12
#define K_SelUser8x16           0x13
#define K_SelUser8x8            0x14
#define K_SelUser6x8            0x15
#define K_SelUser8x16T          0x16
#define K_SetTextColor          0x17
#define K_SetTextBackColor      0x18
#define K_RefreshIcon           0x19      // mfg only
#define K_DisableEnhance        0x19      // when not mfg
#define K_EnableEnhance         0x1A
#define K_GetInfo               0xFF

// Line No
#define K_Line1                 0x01
#define K_Line2                 0x02
#define K_Line3                 0x03
#define K_Line4                 0x04
#define K_Line5                 0x05
#define K_Line6                 0x06
#define K_Line7                 0x07
#define K_Line8                 0x08
#define K_Line9                 0x09
#define K_LineMax               K_Line9
#define K_Line10                0x0A
#define K_Line11                0x0B
#define K_Line12                0x0C
#define K_Line13                0x0D
#define K_MoreLineMax           K_Line13


// Column no
#define K_Column1               0x01
#define K_Column2               0x02
#define K_Column3               0x03
#define K_Column4               0x04
#define K_Column5               0x05
#define K_Column6               0x06
#define K_Column7               0x07
#define K_Column8               0x08
#define K_Column9               0x09
#define K_Column10              0x0A
#define K_Column11              0x0B
#define K_Column12              0x0C
#define K_Column13              0x0D
#define K_Column14              0x0E
#define K_Column15              0x0F
#define K_Column16              0x10
#define K_Column17              0x11
#define K_Column18              0x12
#define K_Column19              0x13
#define K_Column20              0x14
#define K_Column21              0x15
#define K_Column22              0x16
#define K_Column23              0x17
#define K_Column24              0x18
#define K_Column25              0x19
#define K_Column26              0x1A

// Line No Pos with column1 offset
#define K_PosLine1              ((0x01<<8)+K_Column1)
#define K_PosLine2              ((0x02<<8)+K_Column1)
#define K_PosLine3              ((0x03<<8)+K_Column1)
#define K_PosLine4              ((0x04<<8)+K_Column1)
#define K_PosLine5              ((0x05<<8)+K_Column1)
#define K_PosLine6              ((0x06<<8)+K_Column1)
#define K_PosLine7              ((0x07<<8)+K_Column1)
#define K_PosLine8              ((0x08<<8)+K_Column1)
#define K_PosLine9              ((0x09<<8)+K_Column1)     // max for normal
#define K_PosLine10             ((0x0A<<8)+K_Column1)
#define K_PosLine11             ((0x0B<<8)+K_Column1)
#define K_PosLine12             ((0x0C<<8)+K_Column1)
#define K_PosLine13             ((0x0D<<8)+K_Column1)     // max for more line mode

#define K_MaxSmPerLine          20
#define K_MaxSpPerLine          26

// set pos byte seq = K_ESC + Line no + Column no
// set text color seq = K_SetTextColor + high-byte color + low-byte color
// set text background colot seq = K_SetTextBackColor + high-byte color + low-byte color
// cursor = high-byte is row and low byte is column

typedef struct {
  WORD  w_hstart; // start pixel horizontal start position
  WORD  w_vstart; // start pixel vertical start position
  WORD  w_hwidth; // horizontal pixel width, x
  WORD  w_vwidth; // vertical pixel width, y
  WORD  p_data[1];   // pointer to graphical data to be saved or displayed, len = x * y WORD
} T_LCDG;


enum{
  LCDC_YUV_422,
  LCDC_YUV_422_Semiplanar,
  LCDC_YUV_422_Planar,
};

typedef struct{
    WORD  src_w;
    WORD  src_h;
    WORD  mode;
    WORD  sw_rot;
    BYTE* addr[3];
}T_LCD_BUFF_SRC;

/*
  horizontal 320 pixels
  vertical   216 pixels
  2-byte pixels
    bit 5 -   intensity bit which will be appended at the lsb of each 5-bit color to make up 6-bit per
              primary color.
    bit 0-4   Blue
    bit 6-10  Green
    bit 11-15 Red
*/

// BOOLEAN os_disp_open();
#define os_disp_open()            sys_func0(OS_FN_DISP_OPEN)

// void os_disp_close();
#define os_disp_close()           sys_func0(OS_FN_DISP_CLOSE)

// BYTE os_disp_putc(DWORD a_c);
#define os_disp_putc(x)           sys_func1(OS_FN_DISP_PUTC,(DWORD)(x))

// void os_disp_puts(void * a_str);
#define os_disp_puts(x)           sys_func1(OS_FN_DISP_PUTS,(DWORD)(x))

// DWORD os_disp_get_font_len(void * a_str, DWORD a_font_type);
#define os_disp_get_font_len(x,y) sys_func2(OS_FN_DISP_PUTS,(DWORD)(x),(DWORD)(y))

// void os_disp_putn(void * a_str,DWORD a_count);
#define os_disp_putn(x,y)         sys_func2(OS_FN_DISP_PUTN,(DWORD)(x),(DWORD)(y))

// BOOLEAN os_disp_getg(T_LCDG * a_lcdg);
#define os_disp_getg(x)           sys_func1(OS_FN_DISP_GETG,(DWORD)(x))

// BOOLEAN os_disp_get_lcd_buffer(T_LCDG * a_lcdg);
#define os_disp_get_lcd_buffer(x) sys_func1(OS_FN_DISP_GET_LCD_BUFFER,(DWORD)(x))

// BOOLEAN os_disp_putg(T_LCDG * a_lcdg);
#define os_disp_putg(x)           sys_func1(OS_FN_DISP_PUTG,(DWORD)(x))

#define K_DispOverlayColorModeOffset 8
enum
{
  OVERLAY_EXTRA_PARAM_LEN,
  OVERLAY_EXTRA_RGB_CR_KEY_LB,
  OVERLAY_EXTRA_RGB_CR_KEY_HB,
};
// BOOLEAN os_disp_overlay(T_LCDG * a_lcdg, DWORD a_control, BYTE *a_extra_params);
#define os_disp_overlay(x,y,z)    sys_func3(OS_FN_DISP_OVERLAY,(DWORD)(x),(DWORD)(y),(DWORD)(z))
#define K_DispOverlay1            0
#define K_DispOverlay2            1 // On top of K_DispOverlay1
#define K_DispOverlayEn           0x80
#define K_DispOverlayARGB4444     (0<<K_DispOverlayColorModeOffset)
#define K_DispOverlayRGB565       (1<<K_DispOverlayColorModeOffset)
#define K_DispOverlayCRKeyExist   (1<<12)
#define K_DispOverlayFullScr      (1<<13)

// DWORD os_disp_brightness(DWORD a_brightness); // 0-7, 7 = brightest
#define os_disp_brightness(x)     sys_func1(OS_FN_DISP_BRIGHTNESS,(DWORD)(x))

/*
channel icon image bit pattern
bit 0 - com2 tx, red solid circle
bit 1 - com2 rx, green solid circle
bit 2 - com1 tx, red hollow triangle
bit 3 - com1 rx, green hollow triangle
bit 4 - tcp channel 1 tx, red solid star
bit 5 - tcp channel 1 rx, green solid star
bit 6 - tcp channel 2 tx, red hollow diamond
bit 7 - tcp channel 2 rx, green hollow diamond
bit 8 - tcp channel 3 tx, red solid square
bit 9 - tcp channel 3 rx, green solid square
bit 10- tcp channel 4 tx, red hollow down triangle
bit 11- tcp channel 4 rx, green hollow down triangle
bit 12- udp channel 1 tx, red solid down triangle
bit 13- udp channel 1 rx, green solid down triangle
bit 14- udp channel 2 tx, red hollow square
bit 15- udp channel 2 rx, green hollow square
bit 16- usb tx, red solid diamond (OTG that can be host or slave)
bit 17- usb rx, green solid diamond (OTG that can be host or slave)
bit 18- ppp tx, red hollow star
bit 19- ppp rx, green hollow star
bit 20- com3 tx, red solid triangle (GPRS tx)
bit 21- com3 rx, green solid triangle (GPRS rx)
bit 22- mcom modem tx, red hollow circle
bit 23- mcom modem rx, green hollow circle

*/
#define K_DispIconSignal0     0x0001
#define K_DispIconSignal1     0x0002
#define K_DispIconSignal2     0x0004
#define K_DispIconSignal3     0x0008
#define K_DispIconSignal4     0x0010
#define K_DispIconAntenna     0x0020
#define K_DispIconBFrame      0x0100
#define K_DispIconBLevel1     0x0200
#define K_DispIconBLevel2     0x0400
#define K_DispIconBLevel3     0x0800
#define K_DispIconBLevel4     0x1000

// DWORD os_disp_icon_set_signal_type(DWORD type)
#define os_disp_icon_set_signal_type(x)  sys_func1(OS_FN_DISP_ICON_SET_SIGNAL_TYPE,(DWORD)(x))

// display icon signal type
enum {
    DISP_SIGNAL_TYPE_GPRS,    //!< default for gprs/3g only, gprs/3g + wifi terminal
    DISP_SIGNAL_TYPE_WIFI,  //!< default for wifi only terminal
};

#define K_DispTextBodyOnlyEn  (DWORD)(-1)    // setting will output text body only, no background will be output
#define K_DispTextBodyOnlyDis (DWORD)(-2)    // turn this feature

// void os_disp_textc(DWORD a_color);
#define os_disp_textc(x)         sys_func1(OS_FN_DISP_TEXTC,(DWORD)(x))

// void os_disp_backc(DWORD a_color);
#define os_disp_backc(x)          sys_func1(OS_FN_DISP_BACKC,(DWORD)(x))

// DWORD os_disp_putp(DWORD a_pos); bit 0-7 column, bit 8-15 row
#define os_disp_putp(x)           sys_func1(OS_FN_DISP_PUTP,(DWORD)(x))

// DWORD os_disp_getp(void);
#define os_disp_getp()            sys_func0(OS_FN_DISP_GETP)

// DWORD os_disp_bl_control(DWORD a_time); // 0 = off now, 0xff = on now
#define os_disp_bl_control(x)     sys_func1(OS_FN_DISP_BL_CONTROL,(DWORD)(x))

// void os_disp_icon_color(T_DISP_ICON_COLOR  *a_icon_color);
#define os_disp_icon_color(x)     sys_func1(OS_FN_DISP_ICON_COLOR,(DWORD)(x))

#define K_MaxIconSmNormal             28
#define K_MaxIconSpNormal             19
#define K_MaxIconSm                   K_MaxIconSmNormal
#define K_MaxIconSp                   K_MaxIconSpNormal
#define K_MaxIconSmWhole              40        // enhanced font will be 20
#define K_MaxIconSpWhole              26

// DWORD os_disp_icon_putc(DWORD a_c);
#define os_disp_icon_putc(x)      sys_func1(OS_FN_DISP_ICON_PUTC,(DWORD)(x))

// void os_disp_icon_textc(DWORD a_color);
#define os_disp_icon_textc(x)     sys_func1(OS_FN_DISP_ICON_TEXTC,(DWORD)(x))

// DWORD os_disp_length();
#define os_disp_length()          sys_func0(OS_FN_DISP_LENGTH)
// a_hstart = start pixel horizontal start position
// a_vstart =  start pixel vertical start position
// same as that in T_LCDG
// DWORD os_disp_anywhere_puts(void * a_str,DWORD a_hstart,DWORD a_vstart);
#define os_disp_anywhere_puts(x,y,z) sys_func3(OS_FN_DISP_PUTS_ANYWHERE,(DWORD)(x),(DWORD)(y),(DWORD)(z))

          /****** com1 routine ******/

/*
  Offset 0: length of data list (BYTE)
  Offset 1: Communication mode
            RS232 :
              0x00 = AUTO protocol for STX or ENAX STX
              0x01 = STX protocol - STX SEQ DATA ETX LRC
              0x02 = ENAC STX - STX LEN1 LEN2 DATA ETX LRC
              0x03 = SOH-LRC
              0x04 = SOH-CRC
              0x05 = ASYNC protocol with no parity
              0x06 = ASYNC protocol with even parity
              0x07 = ASYNC protocol with odd parity
              0x08 = transparent stream mode
  Offset 2: Communication speed
              8 = 230400 bps
              7 = 115200 bps
              6 = 57600 bps
              5 = 38400 bps
              4 = 28800 bps
              3 = 19200 bps
              2 = 4800 bps
              1 = 2400 bps
              0 = 9600 bps (default)
            bit 4 -
              0 = 1 byte length field
              1 = 2 byte length field
  Offset 3: qualify time in 10ms for ASYNC,rx gap
  Offset 4: response gap
  Offset 5: tx gap
  Offset 6: retry min 1
*/
// BOOLEAN os_com1_open(void * a_init_str);
#define os_com1_open(x)         sys_func1(OS_FN_COM1_OPEN,(DWORD)(x))
#define os_aux1_open            os_com1_open

// DWORD os_com1_close(void);
#define os_com1_close()         sys_func0(OS_FN_COM1_CLOSE)
#define os_aux1_close           os_com1_close

// BOOLEAN os_com1_rxrdy(void);
#define os_com1_rxrdy()         sys_func0(OS_FN_COM1_RXRDY)
#define os_aux1_rxrdy           os_com1_rxrdy

// BOOLEAN os_com1_txrdy(void);
#define os_com1_txrdy()         sys_func0(OS_FN_COM1_TXRDY)
#define os_aux1_txrdy           os_com1_txrdy

// DWORD os_com1_read(BYTE *a_buf); // app only
#define os_com1_read(x)         sys_func1(OS_FN_COM1_READ,(DWORD)(x))
#define os_aux1_read            os_com1_read

// BOOLEAN os_com1_send(BYTE *a_buf,DWORD a_len); // app only
#define os_com1_send(x,y)       sys_func2(OS_FN_COM1_SEND,(DWORD)(x),(DWORD)(y))
#define os_aux1_send            os_com1_send

/*
  - error will be reset after reported
  bit 15        K_LinkExist
  bit 14        K_LinkOnLine
  bit 13        K_LinkTxRdy
  bit 12        K_LinkRxRdy
  bit 11        K_LinkTxError
  bit 10        K_LinkRxError
  bit 8         K_LinkRxOverflow
*/
// DWORD os_com1_status();
#define os_com1_status()        sys_func0(OS_FN_COM1_STATUS)
#define os_aux1_status          os_com1_status

// T_BUF * os_com1_sread(void);  // system only
#define os_com1_sread()         ((T_BUF *)sys_func0(OS_FN_COM1_SREAD))
#define os_aux1_sread           os_com1_sread

// BOOLEAN os_com1_ssend(T_BUF * a_ptr); // system only
#define os_com1_ssend(x)        sys_func1(OS_FN_COM1_SSEND,(DWORD)(x))
#define os_aux1_ssend           os_com1_ssend

          /****** com 2 routine ******/

/*
  Offset 0: length of data list (BYTE)
  Offset 1: Communication mode
            RS232 :
              0x00 = AUTO protocol for STX or ENAX STX
              0x01 = STX protocol - STX SEQ DATA ETX LRC
              0x02 = ENAC STX - STX LEN1 LEN2 DATA ETX LRC
              0x03 = SOH-LRC
              0x04 = SOH-CRC
              0x05 = ASYNC protocol with no parity
              0x06 = ASYNC protocol with even parity
              0x07 = ASYNC protocol with odd parity
              0x08 = transparent stream mode
              0x09 = router mode, system use only
  Offset 2: Communication speed
              8 = 230400 bps
              7 = 115200 bps
              6 = 57600 bps
              5 = 38400 bps
              4 = 28800 bps
              3 = 19200 bps
              2 = 4800 bps
              1 = 2400 bps
              0 = 9600 bps (default)
            bit 4 -
              0 = 1 byte length field
              1 = 2 byte length field
  Offset 3: qualify time in 10ms for ASYNC,rx gap
  Offset 4: response gap
  Offset 5: tx gap
  Offset 6: retry min 1
*/
// BOOLEAN os_com2_open(void * a_init_str);
#define os_com2_open(x)           sys_func1(OS_FN_COM2_OPEN,(DWORD)(x))
#define os_auxdbg_open            os_com2_open

// DWORD os_com2_close(void);
#define os_com2_close()           sys_func0(OS_FN_COM2_CLOSE)
#define os_auxdbg_close           os_com2_close

// BOOLEAN os_com2_rxrdy(void);
#define os_com2_rxrdy()           sys_func0(OS_FN_COM2_RXRDY)
#define os_auxdbg_rxrdy           os_com2_rxrdy

// BOOLEAN os_com2_txrdy(void);
#define os_com2_txrdy()           sys_func0(OS_FN_COM2_TXRDY)
#define os_auxdbg_txrdy           os_com2_txrdy

// DWORD os_com2_read(BYTE * a_buf); // app only
#define os_com2_read(x)           sys_func1(OS_FN_COM2_READ,(DWORD)(x))
#define os_auxdbg_read            os_com2_read

//BOOLEAN os_com2_send(BYTE * a_buf,DWORD a_len); // app only
#define os_com2_send(x,y)         sys_func2(OS_FN_COM2_SEND,(DWORD)(x),(DWORD)(y))
#define os_auxdbg_send            os_com2_send

/*
  - error will be reset after reported
  bit 15        K_LinkExist
  bit 14        K_LinkOnLine
  bit 13        K_LinkTxRdy
  bit 12        K_LinkRxRdy
  bit 11        K_LinkTxError
  bit 10        K_LinkRxError
  bit 8         K_LinkRxOverflow
*/
// DWORD os_com2_status(void);
#define os_com2_status()          sys_func0(OS_FN_COM2_STATUS)
#define os_auxdbg_status          os_com2_status

// T_BUF * os_com2_sread(void);  // system only
#define os_com2_sread()           ((T_BUF *)sys_func0(OS_FN_COM2_SREAD))
#define os_auxdbg_sread           os_com2_sread

// BOOLEAN os_com2_ssend(T_BUF * a_ptr); // system only
#define os_com2_ssend(x)          sys_func1(OS_FN_COM2_SSEND,(DWORD)(x))
#define os_auxdbg_ssend           os_com2_ssend

          /****** com 3 routine not support ******/

/*
  Offset 0: length of data list (BYTE)
  Offset 1: Communication mode
            RS232 :
              0x00 = AUTO protocol for STX or ENAX STX
              0x01 = STX protocol - STX SEQ DATA ETX LRC
              0x02 = ENAC STX - STX LEN1 LEN2 DATA ETX LRC
              0x03 = SOH-LRC
              0x04 = SOH-CRC
              0x05 = ASYNC protocol woth no parity
              0x06 = ASYNC protocol with even parity
              0x07 = ASYNC protocol with odd parity
  Offset 2: Communication speed
              8 = 230400 bps
              7 = 115200 bps
              6 = 57600 bps
              5 = 38400 bps
              4 = 28800 bps
              3 = 19200 bps
              2 = 4800 bps
              1 = 2400 bps
              0 = 9600 bps (default)
            bit 4 -
              0 = 1 byte length field
              1 = 2 byte length field
  Offset 3: qualify time in 10ms for ASYNC,rx gap
  Offset 4: response gap
  Offset 5: tx gap
  Offset 6: retry min 1
*/
// BOOLEAN os_com3_open(void * a_init_str);
#define os_com3_open(x)           sys_func1(OS_FN_COM3_OPEN,(DWORD)(x))
#define os_aux2_open              os_com3_open

// DWORD os_com3_close();
#define os_com3_close()           sys_func0(OS_FN_COM3_CLOSE)
#define os_aux2_close             os_com3_close

// BOOLEAN os_com3_rxrdy();
#define os_com3_rxrdy()           sys_func0(OS_FN_COM3_RXRDY)
#define os_aux2_rxrdy             os_com3_rxrdy

// BOOLEAN os_com3_txrdy();
#define os_com3_txrdy()           sys_func0(OS_FN_COM3_TXRDY)
#define os_aux2_txrdy             os_com3_txrdy

// DWORD os_com3_read(BYTE * a_buf); // app only
#define os_com3_read(x)           sys_func1(OS_FN_COM3_READ,(DWORD)(x))
#define os_aux2_read              os_com3_read

//BOOLEAN os_com3_send(BYTE * a_buf,DWORD a_len); // app only
#define os_com3_send(x,y)         sys_func2(OS_FN_COM3_SEND,(DWORD)(x),(DWORD)(y))
#define os_aux2_send              os_com3_send

/*
  - error will be reset after reported
  bit 15        K_LinkExist
  bit 14        K_LinkOnLine
  bit 13        K_LinkTxRdy
  bit 12        K_LinkRxRdy
  bit 11        K_LinkTxError
  bit 10        K_LinkRxError
  bit 8         K_LinkRxOverflow
*/
// DWORD os_com2_status(); not support
#define os_com3_status()          sys_func0(OS_FN_COM3_STATUS)
#define os_aux2_status            os_com3_status

// T_BUF * os_com3_sread();  // system only
#define os_com3_sread()           ((T_BUF *)sys_func0(OS_FN_COM3_SREAD))
#define os_aux2_sread             os_com3_sread

// BOOLEAN os_com3_ssend(T_BUF * a_ptr); // system only
#define os_com3_ssend(x)          sys_func1(OS_FN_COM3_SSEND,(DWORD)(x))
#define os_aux2_ssend             os_com3_ssend

                /****** encryption routine ******/

// void os_deskey(void * a_key);
#define os_deskey(x)              sys_func2(OS_FN_DESKEY,(DWORD)(x),1)
// void os_3deskey(void * a_key);
#define os_3deskey(x)             sys_func2(OS_FN_DESKEY,(DWORD)(x),2)
// void os_3kdeskey(void * a_key);
#define os_3kdeskey(x)            sys_func2(OS_FN_DESKEY,(DWORD)(x),3)

// void os_des(void * a_msg);
#define os_des(x)                 sys_func1(OS_FN_DES,(DWORD)(x))
// void os_3des(void * a_msg);
#define os_3des(x)                sys_func1(OS_FN_DES,(DWORD)(x))
// void os_3kdes(void * a_msg);
#define os_3kdes(x)               sys_func1(OS_FN_DES,(DWORD)(x))

// void os_des2(void * a_msg);
#define os_des2(x)                sys_func1(OS_FN_DES2,(DWORD)(x))
// void os_3des2(void * a_msg);
#define os_3des2(x)               sys_func1(OS_FN_DES2,(DWORD)(x))
// void os_3kdes2(void * a_msg);
#define os_3kdes2(x)              sys_func1(OS_FN_DES2,(DWORD)(x))

// void os_com_deskey(void * a_key);
#define os_com_deskey(x)          sys_func2(OS_FN_COM_DESKEY,(DWORD)(x),1)
// void os_com_3deskey(void * a_key);
#define os_com_3deskey(x)         sys_func2(OS_FN_COM_DESKEY,(DWORD)(x),2)
// void os_com_3kdeskey(void * a_key);
#define os_com_3kdeskey(x)        sys_func2(OS_FN_COM_DESKEY,(DWORD)(x),3)

// void os_com_des(void * a_msg);
#define os_com_des(x)             sys_func1(OS_FN_COM_DES,(DWORD)(x))
// void os_com_3des(void * a_msg);
#define os_com_3des(x)            sys_func1(OS_FN_COM_DES,(DWORD)(x))
// void os_com_3kdes(void * a_msg);
#define os_com_3kdes(x)           sys_func1(OS_FN_COM_DES,(DWORD)(x))

// void os_com_des2(void * a_msg);
#define os_com_des2(x)            sys_func1(OS_FN_COM_DES2,(DWORD)(x))
// void os_com_3des2(void * a_msg);
#define os_com_3des2(x)           sys_func1(OS_FN_COM_DES2,(DWORD)(x))
// void os_com_3kdes2(void * a_msg);
#define os_com_3kdes2(x)          sys_func1(OS_FN_COM_DES2,(DWORD)(x))

// void os_new_des(void * a_key,void * a_msg);
#define os_new_des(x,y)           sys_func3(OS_FN_NEW_DES,(DWORD)(x),(DWORD)(y),1)
// void os_new_3des(void * a_key,void * a_msg);
#define os_new_3des(x,y)          sys_func3(OS_FN_NEW_DES,(DWORD)(x),(DWORD)(y),2)
// void os_new_3kdes(void * a_key,void * a_msg);
#define os_new_3kdes(x,y)         sys_func3(OS_FN_NEW_DES,(DWORD)(x),(DWORD)(y),3)

// void os_new_des2(void * a_key,void * a_msg);
#define os_new_des2(x,y)          sys_func3(OS_FN_NEW_DES2,(DWORD)(x),(DWORD)(y),1)
// void os_new_3des2(void * a_key,void * a_msg);
#define os_new_3des2(x,y)         sys_func3(OS_FN_NEW_DES2,(DWORD)(x),(DWORD)(y),2)
// void os_new_3kdes2(void * a_key,void * a_msg);
#define os_new_3kdes2(x,y)        sys_func3(OS_FN_NEW_DES2,(DWORD)(x),(DWORD)(y),3)

// BOOLEAN os_rsa(void * a_msg,T_KEY * a_key);
#define os_rsa(x,y)               sys_func2(OS_FN_RSA,(DWORD)(x),(DWORD)(y))

// void os_sha1(void * a_omsg,void * a_imsg,DWORD a_len);
#define os_sha1(x,y,z)            sys_func3(OS_FN_SHA1,(DWORD)(y),(DWORD)(z),(DWORD)(x))

// void os_md5_init(T_MD5_CONTEXT * a_c);
#define os_md5_init(x)            sys_func1(OS_FN_MD5_INIT,(DWORD)x)

// void os_md5_process(T_MD5_CONTEXT * a_c, BYTE* a_input, DWORD a_length);
#define os_md5_process(x,y,z)     sys_func3(OS_FN_MD5_PROCESS,(DWORD)x,(DWORD)y,z)

// void os_md5_finish(T_MD5_CONTEXT * a_c, BYTE* a_digest);
#define os_md5_finish(x,y)        sys_func2(OS_FN_MD5_FINISH,(DWORD)x,(DWORD)y)

#define K_ShaTypeLen                4
#define K_ShaTypeMask               (0xff>>(8-K_ShaTypeLen))
// void os_sha_init(T_SHA1_CONTEXT * a_c,DOWRD a_klen_type,BYTE *a_key);
#define os_sha_init(x,y,z)        sys_func3(OS_FN_SHA_INIT,(DWORD)x,(DWORD)y,(DWORD)z)
// void os_shaxxx_init(T_SHA1_CONTEXT * a_c)
#define os_sha1_init(x)              os_sha_init(x,K_Sha1,0)
#define os_sha224_init(x)            os_sha_init(x,K_Sha224,0)
#define os_sha256_init(x)            os_sha_init(x,K_Sha256,0)
#define os_sha384_init(x)            os_sha_init(x,K_Sha384,0)
#define os_sha512_init(x)            os_sha_init(x,K_Sha512,0)
// void os_hmac_shaxxx_init(T_SHA1_CONTEXT * a_c,DWORD a_key_len,BYTE *a_key);
#define os_hmac_sha1_init(x,y,z)     os_sha_init(x,(y<<K_ShaTypeLen)|K_HmacSha1,(DWORD)z)
#define os_hmac_sha224_init(x,y,z)   os_sha_init(x,(y<<K_ShaTypeLen)|K_HmacSha224,(DWORD)z)
#define os_hmac_sha256_init(x,y,z)   os_sha_init(x,(y<<K_ShaTypeLen)|K_HmacSha256,(DWORD)z)
#define os_hmac_sha384_init(x,y,z)   os_sha_init(x,(y<<K_ShaTypeLen)|K_HmacSha384,(DWORD)z)
#define os_hmac_sha512_init(x,y,z)   os_sha_init(x,(y<<K_ShaTypeLen)|K_HmacSha512,(DWORD)z)

// void os_sha_process(T_SHA_CONTEXT * a_c, BYTE* a_input, DWORD a_length);
#define os_sha_process(x,y,z)     sys_func3(OS_FN_SHA_PROCESS,(DWORD)x,(DWORD)y,z)
#define os_sha1_process           os_sha_process
#define os_sha224_process         os_sha_process
#define os_sha256_process         os_sha_process
#define os_sha384_process         os_sha_process
#define os_sha512_process         os_sha_process
#define os_hmac_sha1_process      os_sha_process
#define os_hmac_sha224_process    os_sha_process
#define os_hmac_sha256_process    os_sha_process
#define os_hmac_sha384_process    os_sha_process
#define os_hmac_sha512_process    os_sha_process

// void os_sha_finish(T_SHA_CONTEXT * a_c, BYTE * a_digest);
#define os_sha_finish(x,y)        sys_func2(OS_FN_SHA_FINISH,(DWORD)x,(DWORD)y)
#define os_sha1_finish            os_sha_finish
#define os_sha224_finish          os_sha_finish
#define os_sha256_finish          os_sha_finish
#define os_sha384_finish          os_sha_finish
#define os_sha512_finish          os_sha_finish
#define os_hmac_sha1_finish       os_sha_finish
#define os_hmac_sha224_finish     os_sha_finish
#define os_hmac_sha256_finish     os_sha_finish
#define os_hmac_sha384_finish     os_sha_finish
#define os_hmac_sha512_finish     os_sha_finish

// void os_rc4_setup(T_RC4_STATE * a_c, BYTE * a_input, DWORD a_length);
#define os_rc4_setup(x,y,z)       sys_func3(OS_FN_RC4_SETUP,(DWORD)x,(DWORD)y,z)

// void os_rc4_encrypt(T_RC4_STATE_T * a_c, BYTE * a_input, DWORD a_length);
#define os_rc4_encrypt(x,y,z)      sys_func3(OS_FN_RC4_ENCRYPT,(DWORD)x,(DWORD)y,(DWORD)(z))

#define   os_md4_init             os_md5_init

// void os_md4_process(T_MD4_CONTEXT * a_c, BYTE * a_input, DWORD a_length);
#define os_md4_process(x,y,z)     sys_func3(OS_FN_MD4_PROCESS,(DWORD)x,(DWORD)y,z)

// void os_md4_finish(T_MD4_CONTEXT * a_c, BYTE * a_digest);
#define os_md4_finish(x,y)        sys_func2(OS_FN_MD4_FINISH,(DWORD)x,(DWORD)y)

//void os_rsa_private(BYTE *a_msg,T_RSA_PRIVATE_KEY *a_private_key);
#define os_rsa_private(x,y)       sys_func2(OS_FN_RSA_PRIVATE,(DWORD)(x),(DWORD)(y))

                /****** HSM security routine ******/

// DWORD os_key_save_nc(T_KEY_BLOCK * a_kb);
#define os_key_save_nc(x)         (int)sys_func1(OS_FN_KEY_SAVE_NC,(DWORD)(x))

// DWORD os_key_save_tmk_nc(T_KEY_BLOCK * a_kb,T_KEY_BLOCK * a_tmk_ekb,DWORD a_non_pci);
#define os_key_save_tmk_nc(x,y,z) (int)sys_func3(OS_FN_KEY_SAVE_TMK_NC,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_key_save_tmk_cbc_nc(T_KEY_BLOCK * a_kb,T_KEY_BLOCK * a_tmk_ekb,BYTE *a_iv);
#define os_key_save_tmk_cbc_nc(x,y,z) (int)sys_func3(OS_FN_KEY_SAVE_TMK_CBC_NC,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_key_save(T_KEY_BLOCK * a_kb);
#define os_key_save(x)            (int)sys_func1(OS_FN_KEY_SAVE,(DWORD)(x))

// DWORD os_key_save_tmk(T_KEY_BLOCK * a_kb,T_KEY_BLOCK * a_tmk_ekb,DWORD a_non_pci);
#define os_key_save_tmk(x,y,z)    (int)sys_func3(OS_FN_KEY_SAVE_TMK,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_key_save_tmk_cbc(T_KEY_BLOCK * a_kb,T_KEY_BLOCK * a_tmk_ekb,BYTE *a_iv);
#define os_key_save_tmk_cbc(x,y,z) (int)sys_func3(OS_FN_KEY_SAVE_TMK_CBC,(DWORD)(x),(DWORD)(y),(DWORD)(z))

#define K_DesCbcMode              0x80000000
#define K_DesDecrypt              0x40000000
// DWORD os_key_des(T_KEY_BLOCK * a_ekb,BYTE * a_data,DWORD a_len);
// to encrypt data using HMK encrypt DES/AES/SM4 key
// a_len bit 31
//         = 0 => ECB mode
//         = 1 => CBC mode
// a_len bit 30
//         = 0 => encrypt
//         = 1 => decrypt
//       bit29-0
//         = length of data, must be multiple of 8/16 for des or AES/SM4 respectively
#define os_key_des(x,y,z)         (int)sys_func3(OS_FN_KEY_DES,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_key_flush(void);        // system use only
#define os_key_flush()            sys_func0(OS_FN_KEY_FLUSH)

// DWORD os_key_dukpt_encrypt(T_KEY_BLOCK *a_ekb,BYTE *a_pinblock,BYTE *a_ksn);
#define os_key_dukpt_encrypt(x,y,z) (int)sys_func3(OS_FN_KEY_DUKPT_ENCRYPT,(DWORD)(x),(DWORD)(y),(DWORD)(z))

#define K_DukptMacMode0         0       // using left key single DES and final one use triple DES
#define K_DukptMacMode1         1       // alternate left and right key single DES and final one use triple DES
#define K_DukptMacMode2         2       // use triple DES only

typedef struct {
  BYTE  s_mac[8];   // fill up to initial value
  DWORD d_len;      // max 1K
  BYTE  s_ksn[10];  // return current KSN
  BYTE  b_mode;
} T_DUKPT_MAC;

#define K_DukptMaxMacLen        1024

// DWORD os_key_dukpt_mac_cal(T_KEY_BLOCK *a_ekb,BYTE *a_msg,T_DUKPT_MAC *a_ksn_mlen);
#define os_key_dukpt_mac_cal(x,y,z) (int)sys_func3(OS_FN_KEY_DUKPT_MAC_CAL,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_key_rsa(BYTE *a_data,T_KEY_BLOCK * a_ekb,DWORD a_len);
#define os_key_rsa(x,y,z)         (int)sys_func3(OS_FN_KEY_RSA,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_key_cup_mac(BYTE *a_data,T_KEY_BLOCK * a_ekb);
#define os_key_cup_mac(x,y)       (int)sys_func2(OS_FN_KEY_CUP_MAC,(DWORD)(x),(DWORD)(y))

// DWORD os_key_cbc_mac(BYTE *a_data,T_KEY_BLOCK * a_ekb, DWORD a_mode_len);
// bit31-24 = mode
// bit23-bit0 = data length
#define os_key_cbc_mac(x,y,z)     (int)sys_func3(OS_FN_KEY_CBC_MAC,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_key_hash(void);
#define os_key_hash()             (int)sys_func0(OS_FN_KEY_HASH)

                /****** thread routine ******/

// BOOLEAN os_thread_open(); // system use only
#define os_thread_open()          sys_func0(OS_FN_THREAD_OPEN)

// BOOLEAN os_thread_close(); // system use only
#define os_thread_close()         sys_func0(OS_FN_THREAD_CLOSE)

// BOOLEAN os_thread_create(DWORD a_address,DWORD a_resolution);
#define os_thread_create(x,y)     sys_func2(OS_FN_THREAD_CREATE,(DWORD)(x),(DWORD)(y))

// BOOLEAN os_thread_delete(DWORD a_address);
#define os_thread_delete(x)       sys_func1(OS_FN_THREAD_DELETE,(DWORD)(x))

//void os_thread_lock();
#define os_thread_lock()          sys_func0(OS_FN_THREAD_LOCK)

//void os_thread_unlock()
#define os_thread_unlock()        sys_func0(OS_FN_THREAD_UNLOCK)

//void os_thread_push()
#define os_thread_push()          sys_func0(OS_FN_THREAD_PUSH)

//void os_thread_pop()
#define os_thread_pop()           sys_func0(OS_FN_THREAD_POP)

//void os_thread_return()
#define os_thread_return()        sys_func0(OS_FN_THREAD_RETURN)

// BOOLEAN os_sys_thread_create(DWORD a_address,DWORD a_resolution);  system only and resolution in unit of 1ms
#define os_sys_thread_create(x,y) sys_func2(OS_FN_SYS_THREAD_CREATE,(DWORD)(x),(DWORD)(y))

                /****** buffer manipulation routine for system use only ******/

//T_BUF * os_bufman_alloc(DWORD a_size);
#define os_bufman_alloc(x)      (T_BUF *)sys_func1(OS_FN_BUFMAN_ALLOC,(DWORD)(x))

//void os_bufman_free(T_BUF ** a_ptr);
#define os_bufman_free(x)       sys_func1(OS_FN_BUFMAN_FREE,(DWORD)(x))

//T_BUF * os_bufman_realloc(T_BUF ** a_ptr,DWORD a_size);
#define os_bufman_realloc(x,y)  (T_BUF *)sys_func2(OS_FN_BUFMAN_REALLOC,(DWORD)(x),(DWORD)(y))

//void os_bufman_shrink(T_BUF * a_ptr);
#define os_bufman_shrink(x)     sys_func1(OS_FN_BUFMAN_SHRINK,(DWORD)(x))

                /****** pseudo led routine ******/
/*
channel icon image bit pattern
bit 0 - com2 tx, red solid circle
bit 1 - com2 rx, green solid circle
bit 2 - com1 tx, red hollow triangle
bit 3 - com1 rx, green hollow triangle
bit 4 - tcp channel 1 tx, red solid star
bit 5 - tcp channel 1 rx, green solid star
bit 6 - tcp channel 2 tx, red hollow diamond
bit 7 - tcp channel 2 rx, green hollow diamond
bit 8 - tcp channel 3 tx, red solid square
bit 9 - tcp channel 3 rx, green solid square
bit 10- tcp channel 4 tx, red hollow down triangle
bit 11- tcp channel 4 rx, green hollow down triangle
bit 12- udp channel 1 tx, red solid down triangle
bit 13- udp channel 1 rx, green solid down triangle
bit 14- udp channel 2 tx, red hollow square
bit 15- udp channel 2 rx, green hollow square
bit 16- usb tx, red solid diamond
bit 17- usb rx, green solid diamond
bit 18- ppp tx, red hollow star
bit 19- ppp rx, green hollow star
bit 20- com3 tx, red solid triangle (GPRS)
bit 21- com3 rx, green solid triangle (GPRS)
bit 22- mcom modem tx, red hollow circle
bit 23- mcom modem rx, green hollow circle

signal icon image bit pattern
bit 0 - signal 0
bit 1 - signal 1
bit 2 - signal 2
bit 3 - signal 3
bit 4 - signal 4
bit 5 - antenna
bit 8 - battery frame
bit 9 - battery level 1
bit 10- battery level 2
bit 11- battery level 3
bit 12- battery level 4
*/
#define K_Com2Tx                0
#define K_Aux1Tx                K_Com2Tx
#define K_Com2Rx                1
#define K_Aux1Rx                K_Com2Rx
#define K_Com1Tx                2
#define K_AuxDbgTx              K_Com1Tx
#define K_Com1Rx                3
#define K_AuxDbgRx              K_Com1Rx
#define K_Tcp1Tx                4
#define K_Tcp1Rx                5
#define K_Tcp2Tx                6
#define K_Tcp2Rx                7
#define K_Tcp3Tx                8
#define K_Tcp3Rx                9
#define K_Tcp4Tx                10
#define K_Tcp4Rx                11
#define K_Udp1Tx                12
#define K_Udp1Rx                13
#define K_Udp2Tx                14
#define K_Udp2Rx                15
#define K_UsbTx                 16
#define K_UsbRx                 17
#define K_PppTx                 18
#define K_PppRx                 19
#define K_Com3Tx                20
#define K_com3Tx                K_Com3Tx
#define K_Com3Rx                21
#define K_com3rx                K_Com3rx
#define K_MdmTx                 22
#define K_MdmRx                 23

#define K_LedMax                (K_MdmRx-K_Aux1Tx+1)
#define K_AllLed                (DWORD)-1

#define K_LedDelayMin           4

// void os_led_open(void); system only
#define os_led_open()           sys_func0(OS_FN_LED_OPEN)

// void os_led_close(void); system only
#define os_led_close()          sys_func0(OS_FN_LED_CLOSE)

// DWORD os_led_status(DWORD a_which);
#define os_led_status(x)        sys_func1(OS_FN_LED_STATUS,(DWORD)(x))

// void os_led_set(DWORD a_which);system only
#define os_led_set(x)           sys_func1(OS_FN_LED_SET,(DWORD)(x))

// void os_led_clear(DWORD a_which);system only
#define os_led_clear(x)         sys_func1(OS_FN_LED_CLEAR,(DWORD)(x))

// void os_led_on(DWORD a_which,DWORD a_delay);system only
#define os_led_on(x,y)          sys_func2(OS_FN_LED_ON,(DWORD)(x),(DWORD)(y))

                /****** app security routine ******/

// void os_get_challenge(BYTE * a_dest);
#define os_get_challenge(x)     sys_func1(OS_FN_GET_CHALLENGE,(DWORD)(x))

#define   K_AuthAppSec          0
#define   K_AuthAppDll          1
#define   K_AuthUnlock          2           // can be used by tamperdll with appid = 12 only

// BOOLEAN os_auth(DWORD a_auth_type,BYTE * a_cryptogram);
#define os_auth(x,y)            sys_func2(OS_FN_AUTH,(DWORD)(x),(DWORD) (y))

// int os_tamper_unlock(void);  // security api and need K_AuthUnlock authentication
// return either tamper status or -1
// -1 => authentication error
// 0 = tamper reset ok
// other => tamper path tampered
#define os_tamper_unlock()      (int)sys_func0(OS_FN_TAMPER_UNLOCK)    // must do K_AuthUnlock first

                /****** app search routine ******/

// BOOLEAN os_app_search_id(DWORD a_appid);
#define os_app_search_id(x)     sys_func1(OS_FN_APP_SEARCH_ID,(DWORD)(x))

// BOOLEAN os_app_search_name(BYTE * a_name,DWORD a_control);
#define os_app_search_name(x,y) sys_func2(OS_FN_APP_SEARCH_NAME,(DWORD)(x),(DWORD)(y))

// BOOLEAN os_app_get_virgin(DWORD a_appid);
#define os_app_get_virgin(x)     sys_func1(OS_FN_APP_GET_VIRGIN,(DWORD)(x))

// void os_app_clear_virgin(DWORD a_appid);
#define os_app_clear_virgin(x)  sys_func1(OS_FN_APP_CLR_VIRGIN,(DWORD)(x))

                /****** memory allocate routine ******/

// void * os_malloc(DWORD a_size);
#define os_malloc(x)            (BYTE*)sys_func1(OS_FN_MALLOC,(DWORD)(x))

// void * os_realloc(void * a_bptr,DWORD a_size);
#define os_realloc(x,y)         (BYTE*)sys_func2(OS_FN_REALLOC,(DWORD)(x),(DWORD)(y))

// void os_free(void * a_bptr);
#define os_free(x)              sys_func1(OS_FN_FREE,(DWORD)(x))

                /****** file system routine ******/

//==========================================================================
#define     K_ERR_FS_NOT_MOUNT              -2
#define     K_ERR_FS_STILL_MOUNT            -3
#define     K_ERR_FS_NOT_OPEN               -4
#define     K_ERR_FS_ACCESS_DENIED          -5
#define     K_ERR_FS_INVALID_VALUE          -6
#define     K_ERR_FS_TOO_MANY_OPENED        -7
#define     K_ERR_FS_TOO_MANY_FILES         -8
#define     K_ERR_FS_FILE_NOT_FOUND         -9
#define     K_ERR_FS_FILE_ALREADY_OPENED    -10
#define     K_ERR_FS_FILE_TOO_LARGE         -11
#define     K_ERR_FS_READ_ERROR             -12
#define     K_ERR_FS_WRITE_ERROR            -13
#define     K_ERR_FS_DEVICE_FULL            -14
#define     K_ERR_FS_ENDIAN_ERROR           -15
#define     K_ERR_FS_TOO_FEW_BLOCKS         -16
#define     K_ERR_FS_MEMORY_ALLOCATE        -17
#define     K_ERR_FS_FILE_LOCKED            -18
#define     K_ERR_FS_PATH_NOT_FOUND         -19
#define     K_ERR_FS_DIR_NOT_EMPTY          -20
#define     K_ERR_FS_NOT_SUPPORT            -21
//==========================================================================

//------------------------------------------------------------------------------
//      Error code for device
//------------------------------------------------------------------------------
#define K_EINVAL          51
#define K_ENODEV          52
#define K_ENOENT          53
#define K_EINPROGRESS     54
#define K_ENOEXEC         55
#define K_EMSGSIZE        56
#define K_ENOTCONN        57
#define K_EBUSY           58
#define K_EPERM           59
#define K_EOPNOTSUPP      60
#define K_EAGAIN          61
#define K_ETIMEDOUT       62
#define K_ENOMEM          63
#define K_EPROTO          64
#define K_EPIPE           65
#define K_EHOSTUNREACH    66
#define K_ESHUTDOWN       67
#define K_EIDRM           68  // id removed
#define K_ENOTSUPP        69
#define K_EEXIST          70
#define K_ECONNRESET      71
#define K_EIO             72
#define K_EISDIR          73
#define K_EOVERFLOW       74
#define K_EILSEQ          75
#define K_EREMOTEIO       76
#define K_ECOMM           77
#define K_ENOSR           78
#define K_EALREADY        79
#define K_ETIME           80
#define K_ENOLINK         81
//==========================================================================

// File Open mode bits
#define K_O_RDONLY              0x0000
#define K_O_WRONLY              0x0001
#define K_O_RDWR                0x0002
#define K_O_CREAT               0x0040
#define K_O_TRUNC               0x0200
#define K_O_APPEND              0x0400
#define K_O_SHARE               0x0800

// bit definitions of file attributes
#define K_FILE_OTH_WRITE        0x02    // others have write permission
#define K_FILE_OTH_READ         0x04    // others have read permission
#define K_FILE_USR_CTRL         0x08    // data file cannot be deleted in boot system, but in app only
#define K_FILE_USR_WRITE        0x10    // user has write permission
#define K_FILE_USR_READ         0x20    // user has read permission
#define K_FILE_ACTIVE           0x40    // active
#define K_FILE_VIRGIN           0x80    // virgin flags
#define K_FILE_EXECUTABLE       0x100   // this is a executable file
#define K_FILE_ENCRYPTED        0x200   // file encrypted
#define K_FILE_LOCKED           0x400   // indicate file is locked by s_auth_key
#define K_FILE_SYSTEM           0x800   // indicate system file create is requested when created with owner
#define K_FILE_DIRECTORY        0x1000  // directory
#define K_DIR_CREATE            0x2000  // allows create file in this dir
#define K_DIR_DELETE            0x4000  // allows delete file in this dir

// for file seek
#define K_FilePosHome           0
#define K_FileCurrPos           (DWORD)-2
#define K_FilePosEnd            (DWORD)-1

// file search control for search and dsearch
// for dsearch, only owner file will be returned
#define K_SearchNext            0
#define K_SearchReset           1

// use for search only, dsearch has no effect
#define K_SearchAll             0x1e
#define K_SearchData            0x02
#define K_SearchApp             0x04
#define K_SearchActivate        0x08
#define K_SearchDeactivate      0x10

// for dsearch only
#define K_SearchPub             2

//#define K_FlushAll              ((DWORD)-1) not support

// T_FSINFO constants
// device type (bit0-7 of d_device)
#define K_FSDEV_NAND            1
#define K_FSDEV_NOR             2
#define K_FSDEV_DATAFLASH       3
#define K_FSDEV_USB             11
#define K_FSDEV_SD              12
#define K_FSDEV_REMOVEABLE      0x100
#define K_FSDEV_READONLY        0x200

// file system format ID
#define K_FSFORMAT_YAFFS        1
#define K_FSFORMAT_FAT32        2
#define K_FSFORMAT_FAT16        3

// file system information
typedef struct {
  BYTE b_devname[32];
  BYTE b_device;
  BYTE b_format;
  BOOLEAN b_case_sensitive;
  BYTE rfu;
  DWORD d_version;
  DWORD d_capacity;
  DWORD d_sectorsize;
} T_FSINFO;

typedef struct {
  BYTE  s_name[20];
  union {
    BYTE  s_owner[13];
    BYTE  s_agent_id[8];
  };
  DWORD d_attr;
  DWORD d_filelen;
  BYTE  s_create_date[7];
  BYTE  s_last_update[7];
} T_FILE;

typedef struct {
  BYTE  s_name[256];
  BYTE  s_owner[16];
  DWORD d_attr;
  DWORD d_filelen;
  DWORD d_occupied_size;
  BYTE  s_create_date[7];
  BYTE  rfu1;
  BYTE  s_last_update[7];
  BYTE  rfu2;
} T_DirEntry;

typedef struct
{
  DWORD marker;           // valid mark
  int d_index;            // index of next entry
  void *v_dirObj;         // pointer to directory object
  void *v_nextEntry;      // pointer to next entry
  void *v_fs;             // pointer to file system
  DWORD magic;            // magic code
  T_DirEntry entry;
} T_Dir;

// int os_file_create_with_owner(BYTE * a_name,DWORD a_attribut,BYTE * a_owner); // system use only
#define os_file_create_with_owner(x,y,z) ((int)sys_func3(OS_FN_FILE_CREATE_OWNER,(DWORD)(x),(DWORD)(y),(DWORD)(z)))

// int os_file_open(BYTE * a_name,DWORD a_mode);
#define os_file_open(x,y)       ((int)sys_func2(OS_FN_FILE_OPEN,(DWORD)(x),(DWORD)(y)))

// int os_file_create(BYTE * a_name,DWORD a_attribut);
#define os_file_create(x,y)     ((int)sys_func2(OS_FN_FILE_CREATE,(DWORD)(x),(DWORD)(y)))

// int os_file_close(DWORD a_fid);
#define os_file_close(x)       ((int)sys_func1(OS_FN_FILE_CLOSE,(DWORD)(x)))

// BOOLEAN os_file_exist(BYTE * a_name);
#define os_file_exist(x)        sys_func1(OS_FN_FILE_EXIST,(DWORD)(x))

// int os_file_rename(BYTE * a_newname, BYTE * a_oldname);
#define os_file_rename(x,y)     ((int)sys_func2(OS_FN_FILE_RENAME,(DWORD)(x),(DWORD)(y)))

// int os_file_delete(BYTE * a_name);
#define os_file_delete(x)       ((int)sys_func1(OS_FN_FILE_DELETE,(DWORD)(x)))

// BOOLEAN os_file_flushed(DWORD a_fid);
#define os_file_flushed(x)      sys_func1(OS_FN_FILE_FLUSHED,(DWORD)(x))

// int os_file_seek(DWORD a_fid,DWORD a_pos);
#define os_file_seek(x,y)       ((int)sys_func2(OS_FN_FILE_SEEK,(DWORD)(x),(DWORD)(y)))

// int os_file_read(DWORD a_fid,BYTE * a_buffer,DWORD a_len);
#define os_file_read(x,y,z)     ((int)sys_func3(OS_FN_FILE_READ,(DWORD)(x),(DWORD)(y),(DWORD)(z)))

// int os_file_write(DWORD a_fid,BYTE * a_buffer,DWORD a_len);
#define os_file_write(x,y,z)    ((int)sys_func3(OS_FN_FILE_WRITE,(DWORD)(x),(DWORD)(y),(DWORD)(z)))

// int os_file_putc(DWORD a_fid,DWORD c);
#define os_file_putc(x,y)       ((int)sys_func2(OS_FN_FILE_PUTC,(DWORD)(x),(DWORD)(y)))

// int os_file_getc(DWORD a_fid);
#define os_file_getc(x)         ((int)sys_func1(OS_FN_FILE_GETC,(DWORD)(x)))

// int os_file_length(DWORD a_fid);
#define os_file_length(x)       ((int)sys_func1(OS_FN_FILE_LENGTH,(DWORD)(x)))

// BOOLEAN os_file_end(DWORD a_fid);
#define os_file_end(x)          sys_func1(OS_FN_FILE_END,(DWORD)(x))

//T_FILE * os_file_search(DWORD a_control) // system only
#define os_file_search(x)       (T_FILE *)sys_func1(OS_FN_FILE_SEARCH,(DWORD)(x))

//T_FILE * os_file_search_data(DWORD a_control, T_FILE *a_file)
#define os_file_search_data(x,y)  (T_FILE *)sys_func2(OS_FN_FILE_DSEARCH,(DWORD)(x), (DWORD)(y))

//T_FILE * os_file_dsearch_all(DWORD a_control, T_FILE *a_file)
#define os_file_dsearch_all(x,y)  (T_FILE *)sys_func2(OS_FN_FILE_DSEARCH_ALL,(DWORD)(x), (DWORD)(y))

//BOOLEAN os_file_deactivate(BYTE * a_name)
#define os_file_deactivate(x)   sys_func1(OS_FN_FILE_DEACTIVATE,(DWORD)(x))

//BOOLEAN os_file_activate(BYTE * a_name)
#define os_file_activate(x)     sys_func1(OS_FN_FILE_ACTIVATE,(DWORD)(x))

//int os_file_get_name(DWORD a_fid,BYTE * a_name)
#define os_file_get_name(x,y)   ((int)sys_func2(OS_FN_FILE_GET_NAME,(DWORD)(x),(DWORD)(y)))

//int os_file_truncate(BYTE * a_name,DWORD a_len)
#define os_file_truncate(x,y)   ((int)sys_func2(OS_FN_FILE_TRUNCATE,(DWORD)(x),(DWORD)(y)))

//int os_file_auth(DWORD a_fid, BYTE *a_hash)
#define os_file_auth(x,y)       ((int)sys_func2(OS_FN_FILE_AUTH,(DWORD)(x),(DWORD)(y)))

//int os_file_set_lock(DWORD a_fid, BYTE *a_key)
#define os_file_set_lock(x,y)   ((int)sys_func2(OS_FN_FILE_SET_LOCK,(DWORD)(x),(DWORD)(y)))

//unsigned int os_file_get_free_space()
#define os_file_get_free_space() (sys_func0(OS_FN_FILE_GET_FREE_SPACE))

//unsigned int os_file_freeSpace(BYTE *path)
#define os_file_freeSpace(x)    (sys_func1(OS_FN_FILE_FREESPACE, (DWORD)(x)))

//int os_file_get_systeminfo(const char *path, T_FSINFO *fsinfo);
#define os_file_systeminfo(x,y) ((int)(sys_func2(OS_FN_FILE_GET_SYSTEMINFO, (DWORD)(x), (DWORD)(y))))

//int os_file_ftruncate(DWORD a_fid, DWORD a_len);
#define os_file_ftruncate(x,y)  ((int)(sys_func2(OS_FN_FILE_FTRUNCATE, (DWORD)(x), (DWORD)(y))))

//int os_file_init_search(const char *path, T_Dir *dir);
#define os_file_init_search(x,y)  ((int)(sys_func2(OS_FN_FILE_INIT_SEARCH, (DWORD)(x), (DWORD)(y))))

//T_DirEntry *os_file_readdir( T_Dir *dir);
#define os_file_readdir(x)      ((T_DirEntry *)sys_func1(OS_FN_FILE_READDIR, (DWORD)(x)))

//int os_file_mkdir(const char *path, unsigned int attr);
#define os_file_mkdir(x,y)      ((int)(sys_func2(OS_FN_FILE_MKDIR, (DWORD)(x), (DWORD)(y))))

//int os_file_rmdir(const char *path);
#define os_file_rmdir(x)        ((int)(sys_func1(OS_FN_FILE_RMDIR, (DWORD)(x))))

//int os_file_get_longname(DWORD a_fid, BYTE *a_name, DWORD a_maxlen);
#define os_file_get_longname(x,y,z) ((int)(sys_func3(OS_FN_FILE_GET_LONGNAME, (DWORD)(x), (DWORD)(y), (DWORD)(z))))

//void os_file_sync(void);
#define os_file_sync()          (sys_func0(OS_FN_FILE_SYNC))

                /****** config routine ******/
#define K_LcdBannerNormal         1
#define K_LcdBannerWholeLine      2

#define   K_CF1_End           K_CF_Netmask
enum {
  K_CF_Version,               // cf1, system only, 2-byte
  K_CF_DebugMode,             // cf1, system only, 4-byte
  K_CF_SerialNo,              // cf1, read only,   8-byte
  K_CF_Tid,                   // cf1, r/w only,    8-byte
  K_CF_AppKekKvc,             // cf1, system only, 2-byte
  K_CF_EtherID,               // cf1, read only,   6-byte
  K_CF_PppFixIP,              // cf1, r/w,         4-byte
  K_CF_IP,                    // cf1, r/w,         4-byte
  K_CF_DssMkKvc,              // cf1, system only  2-byte
  K_CF_Tpad,                  // cf1, system only, sizeof T_CAL_POINT only
  K_CF_TpadOffset,            // cf1, system only, sizeof T_CAL_POINT only
  K_CF_SnPrefix,              // cf1, read only,   16-byte, unused space will be right filled with 0xff
  K_CF_Netmask,               // cf2, r/w,         4-byte
  K_CF_Gateway,               // cf2, r/w,         4-byte
  K_CF_DNS1,                  // cf2, r/w,         4-byte
  K_CF_DNS2,                  // cf2, r/w,         4-byte
  K_CF_UDPPort,               // cf2, r/w,         2-byte
  K_CF_TCPPort,               // cf2, r/w,         2-byte
  K_CF_DHCPEnable,            // cf2, r/w,         1-byte
  K_CF_TamperOn,              // cf2, system only, 8-byte
  K_CF_Tampered,              // cf2, r/w,         8-byte 0xffffffffffffffff = not tampered, other value tampered
  K_CF_BSysPsw,               // cf2, system only, 8-byte
  K_CF_DllPsw,                // cf2, system only, 8-byte
  K_CF_SetupPsw,              // cf2, system only, 8-byte
  K_CF_TamperPsw,             // cf2, system only, 8-byte
  K_CF_EnforcePsw,            // cf2, system only, 1-byte
  K_CF_DMK,                   // cf2, system only, 32-byte updated 22-3-2012
  K_CF_TimeStamp,             // cf2, system only, 8-byte
  K_CF_ParsePhrase,           // cf2, system only, 8-byte
  K_CF_BattChgMode,           // cf2, r/w,         1-byte T1000 only
  K_CF_BattError,             // cf2, r/w,         1-byte T1000 only
  K_CF_LcdBrightness,         // cf2, read only,   1-byte
  K_CF_LcdTimeout,            // cf2, read only,   1-byte
  K_CF_SimSelect,             // cf2, r/w,         1-byte T1000 only
  K_CF_IncomingCall,          // cf2, r/w,         1-byte T1000 only
  K_CF_SmsEnable,             // cf2, read only,   1-byte T1000 only
  K_CF_GprsPowerOnOff,        // cf2, r/w,         1-byte T1000 only
  K_CF_MdmCountry,            // cf2, r/w,         1-byte
  K_CF_MdmTxPwr,              // cf2, r/w,         1-byte
  K_CF_MdmHandshakeDelay,     // cf2, r/w,         1-byte
  K_CF_HwConfig,              // cf2, read only,   K_ProdCodeMax, this is product code
  K_CF_KbdBL,                 // cf2, r/w,         1-byte
  K_CF_RtcControlOff,         // cf2, r/w,         1-byte
  K_CF_SNControlOff,          // cf2, r/w,         1-byte
  K_CF_LcdBannerEnable,       // cf2, r/w,         1-byte
  K_CF_LcdBOffset,            // cf2, system only, 1-byte
  K_CF_Logo,                  // cf2, system only, For T1000 it is K_LogoSize
  K_CF_PublicKey,             // cf2, system only, sizeof T_SKEY
  K_CF_CurRdllInf,            // cf2, system only, sizeof T_RDLL_INF
  K_CF_SysRdllInf,            // cf2, system only, sizeof T_RDLL_INF
  K_CF_RdllInf,               // cf2, r/w,         sizeof T_RDLL_INF
  K_CF_SensitiveBlock,        // cf2, system only, 1 byte
  K_CF_PswError,              // cf2, system only, 4 byte
  K_CF_LandingZone,           // cf2, r/w,         For T1000, K_LandZoneSize
  K_CF_PingEnable,            // cf2, r/w,         1-byte, default off
  K_CF_TcpHostEnable,         // cf2, r/w,         1-byte, default off
  K_CF_ARPMac,                // cf2, r/w,         6-byte
  K_CF_ARPIp,                 // cf2, r/w,         4-byte
  K_CF_BattStatData,          // cf2, system only, 96-byte T1000 only
  K_CF_BattStatCount,         // cf2, system only, 1-byte  T1000 only
  K_CF_BattStatValid,         // cf2, system only, 1-byte  T1000 only
  K_CF_LptStamp,              // cf2, read only, 4-byte,
  K_CF_Settled,               // cf2, r/w,         1-byte, default 0xff => settled, non-zero=>settled
  K_CF_ReqSettleMsg,          // cf2, r/w,         64-byte, default message "Please settle first", each message must be null terminated. SM font will be used and display start at line 3
  K_CF_FreqBand,              // cf2, system only, 2-byte, default 0, used to save GPRS band for CPU freq selection T1000 only
  K_CF_UserSignKeyLimit,      // cf2, r/w once,    1-byte, default 0, allow multiple injection and system down grade to lower version; non-zero, allow user key injection once and system upgrad only.
  K_CF_ParamTerm2Term,        // cf2, r/w,         1-byte, default 0, allow application parameter files to be transfered during term2term
  K_CF_NotDelAppData,         // cf2, read only,   1-byte, default 0xff. Non-zero => not delete app data files; zero => delete app data files
  K_CF_MirrorApp,             // cf2, r/w,         1-byte, default 0xff. 0xff => no mirror. 1=> mirror applications, other values reserved.
  K_CF_MdmHypercomAnsToneTo,  // cf2, r/w,        1-byte
  K_CF_MdmAutoTxPwr,          // cf2, r/w,         1-byte
  K_CF_MdmDTPwr,              // cf2, r/w,         1-byte
  K_CF_MdmDTMFPwr,            // cf2, r/w,         2-byte
  K_CF_MdmU80,                // cf2, r/w,         2-byte
  K_CF_MdmMinAnsToneDur,      // cf2, r/w,         2-byte
  K_CF_AbecsEnable,           // cf2, read only,   1-byte, default 0, not enable ABECS applicaiton deletion
  K_CF_DllControlEnable,      // cf2, read only,   1-byte, default 0, not enable dll control
  K_CF_AgentLogged,           // cf2, read only,   8-byte, default all FF => no agent logged
  K_CF_ArpAlways,             // cf2, r/w,         1-byte, default 0, not ARP always when TCP or UDP is opened
  K_CF_LCDC_ALWAYS_ON,        // cf2, r/w,         1-byte, default 0, LCD controller off when backlight off, 1:LCD controller always on
  K_CF_SysRebootEnable,       // cf2, system only, 1-byte
  K_CF_GPRS_StkiRespMsgOn,    // cf2, r/w, 1-byte, on(0)-off(ff) switch, default off(0), for ECR15-0011
  K_CF_BUpDllControl,         // cf2, system only, 1-byte  0 = off, 1 = on
  K_CF_GPS_PowerOnOff,        // cf2, r/w,         1-byte
  K_CF_WIFI_IP_CONFIG,        // cf2, r/w, 1-byte, 0 = static IP, 1 = DHCP (default)
  K_CF_WIFI_IP_ADDR,          // cf1, r/w, 4-bytes
  K_CF_WIFI_IP_MASK,          // cf2, r/w, 4-bytes
  K_CF_WIFI_GATEWAY,          // cf2, r/w, 4-bytes
  K_CF_WIFI_DNS1,             // cf2, r/w, 4-bytes
  K_CF_WIFI_DNS2,             // cf2, r/w, 4-bytes
  K_CF_CLICC_CFG,             // cf2, r/w, 64-bytes
  K_CF_MIF_CFG,               // cf2, r/w, 64-bytes
  K_CF_NFC_CFG,               // cf2, r/w, 64-bytes
  K_CF_Barcode,               // cf2, r/w, 1-byte 0= no barcode, 1 = 1D, 2 = 2D
  K_CF_Bc2DSensitivity,       // cf2, r/w, 1-byte, 2D sensitivity, 0-20, 0 is the most sensitive. Default is 0xff which uses BC default 0
  K_CF_LogoDeactivate,        // cf2, r/w, 1-byte, default = 0 => logo activated
  K_CF_WIFI_AP_PROFILE,       // cf2, r/w, 1000bytes
  K_CF_WIFI_DEFAULT_AP,       // cf2, r/w, 100bytes
  K_CF_WIFI_SCAN_REGION,      // cf2, , r/w, 1-byte. wifi scan region. s_cf_default(0). 1 = Europe, 2 = US, 3 = ASIA  
  K_CF_Max
};

#define CLICC_CFG_BLK_SIZE          64
enum{
  K_CLICC_As3911_CFG_Identity_0,
  K_CLICC_As3911_CFG_Identity_1,
  K_CLICC_As3911_CFG_ModDepB,
  K_CLICC_As3911_CFG_NorLvDef,
  K_CLICC_As3911_CFG_RedNorLvDef,
  K_CLICC_As3911_CFG_RxGainA,
  K_CLICC_As3911_CFG_RxGainB,
  K_CLICC_As3911_CFG_AntCalCtrlA,
  K_CLICC_As3911_CFG_AntCalCtrlB,
  K_CLICC_As3911_CFG_ChannelA,
  K_CLICC_As3911_CFG_ChannelB,
  K_CLICC_As3911_CFG_RxGain23A,
  K_CLICC_As3911_CFG_RxGain23B,
  K_CLICC_As3911_CFG_RfCfg1A,
  K_CLICC_As3911_CFG_RfCfg1B,
  K_CLICC_As3911_CFG_ModTable_0,
  K_CLICC_As3911_CFG_ModTable_1,
  K_CLICC_As3911_CFG_ModTable_2,
  K_CLICC_As3911_CFG_ModTable_3,
  K_CLICC_As3911_CFG_ModTable_4,
  K_CLICC_As3911_CFG_ModTable_5,
  K_CLICC_As3911_CFG_ModTable_6,
  K_CLICC_As3911_CFG_ModTable_7,
  K_CLICC_As3911_CFG_ModTable_8,
  K_CLICC_As3911_CFG_ModTable_9,
  K_CLICC_As3911_CFG_ModTable_10,
  K_CLICC_As3911_CFG_ModTable_11,
  K_CLICC_As3911_CFG_ModTable_12,
  K_CLICC_As3911_CFG_ModTable_13,
  K_CLICC_As3911_CFG_ModTable_14,
  K_CLICC_As3911_CFG_ModTable_15,
  K_CLICC_As3911_CFG_Nor2RedThre,
  K_CLICC_As3911_CFG_Red2NorThre,
  K_CLICC_As3911_CFG_SquelchA,
  K_CLICC_As3911_CFG_SquelchB,
  K_CLICC_As3911_CFG_AgcA,
  K_CLICC_As3911_CFG_AgcB,
  K_CLICC_As3911_CFG_Red_ModTable_0,
  K_CLICC_As3911_CFG_Red_ModTable_1,
  K_CLICC_As3911_CFG_Red_ModTable_2,
  K_CLICC_As3911_CFG_Red_ModTable_3,
  K_CLICC_As3911_CFG_Red_ModTable_4,
  K_CLICC_As3911_CFG_Red_ModTable_5,
  K_CLICC_As3911_CFG_Red_ModTable_6,
  K_CLICC_As3911_CFG_Red_ModTable_7,
  K_CLICC_As3911_CFG_Red_ModTable_8,
  K_CLICC_As3911_CFG_Red_ModTable_9,
  K_CLICC_As3911_CFG_Red_ModTable_10,
  K_CLICC_As3911_CFG_Red_ModTable_11,
  K_CLICC_As3911_CFG_Red_ModTable_12,
  K_CLICC_As3911_CFG_Red_ModTable_13,
  K_CLICC_As3911_CFG_Red_ModTable_14,
  K_CLICC_As3911_CFG_Red_ModTable_15,
  K_CLICC_As3911_CFG_NUM
};

enum{
  K_Mif_As3911_CFG_Identity_0,
  K_Mif_As3911_CFG_Identity_1,
  K_Mif_As3911_CFG_ModLvDefB,
  K_Mif_As3911_CFG_ModDepB,
  K_Mif_As3911_CFG_NorLvDefA,
  K_Mif_As3911_CFG_NorLvDefB,
  K_Mif_As3911_CFG_RxGainA,
  K_Mif_As3911_CFG_RxGainB,
  K_Mif_As3911_CFG_AntCalCtrlA,
  K_Mif_As3911_CFG_AntCalCtrlB,
  K_Mif_As3911_CFG_ChannelA,
  K_Mif_As3911_CFG_ChannelB,
  K_Mif_As3911_CFG_RxGain23A,
  K_Mif_As3911_CFG_RxGain23B,
  K_Mif_As3911_CFG_RfCfg1A,
  K_Mif_As3911_CFG_RfCfg1B,
  K_Mif_As3911_CFG_NUM
};

enum{
  K_Nfc_As3911_CFG_Identity_0,
  K_Nfc_As3911_CFG_Identity_1,
  K_Nfc_As3911_CFG_ModLvDefB,
  K_Nfc_As3911_CFG_ModDepB,
  K_Nfc_As3911_CFG_NorLvDefA,
  K_Nfc_As3911_CFG_NorLvDefB,
  K_Nfc_As3911_CFG_RxGainA,
  K_Nfc_As3911_CFG_RxGainB,
  K_Nfc_As3911_CFG_AntCalCtrlA,
  K_Nfc_As3911_CFG_AntCalCtrlB,
  K_Nfc_As3911_CFG_ChannelA,
  K_Nfc_As3911_CFG_ChannelB,
  K_Nfc_As3911_CFG_RxGain23A,
  K_Nfc_As3911_CFG_RxGain23B,
  K_Nfc_As3911_CFG_RfCfg1A,
  K_Nfc_As3911_CFG_RfCfg1B,
  K_Nfc_As3911_CFG_NUM
};

#define   K_ProdCodeMax       30

// Battery charging mode
#define   K_BattDeskTopMode   1
#define   K_BattMobileMode    0

// Battery error status
#define   K_BattOk            0
#define   K_BattPreChgError   1
#define   K_BattChgTimeError  2
#define   K_BattTempOverError 3

// DWORD config_length(DWORD a_idx);
#define os_config_length(x)     sys_func1(OS_FN_CONFIG_LENGTH,(DWORD)(x))

// BOOLEAN config_read(DWORD a_idx,BYTE * a_dest);
#define os_config_read(x,y)     sys_func2(OS_FN_CONFIG_READ,(DWORD)(x),(DWORD)(y))

// BOOLEAN config_write(DWORD a_idx,BYTE * a_src);
#define os_config_write(x,y)    sys_func2(OS_FN_CONFIG_WRITE,(DWORD)(x),(DWORD)(y))

// BOOLEAN config_update(void);
#define os_config_update()      sys_func0(OS_FN_CONFIG_UPDATE)

                /****** sam routine ******/

// void os_sam_open(DWORD a_select);
// bit 0 - select ICC operate voltage
//     0 = 5V
//     1 = 3V3
//     2 = 1V8
#define K_Sam5V         0
#define K_Sam3V3        1
#define K_Sam1V8        2
#define os_sam_open(x)          sys_func1(OS_FN_SAM_OPEN,(DWORD)(x))

// void os_sam_close(void);
#define os_sam_close()          sys_func0(OS_FN_SAM_CLOSE)

// void os_sam_select(DWORD a_index_mode);
#define K_WarmAfterCold 0x2000    // only warm reset, no cold reset
#define K_MandatoryWarm 0x1000    // mandatory warm reset after cold reset
#define K_PbocT0Atr     0x0800
#define K_AtrByte       0x0400
#define K_DesfireHighSp 0x0200
#define K_JsbCert       0x0100
#define K_EmvCompliant  0x0080
#define K_FixB38400     0x0040
#define K_NoPts         0x0020
#define K_NoIfs         0x0010
#define K_SamMask       7

#define os_sam_select(x)        sys_func1(OS_FN_SAM_SELECT,(DWORD)(x))

// BOOLEAN os_sam_inserted(DWORD a_index);
#define os_sam_inserted(x)      sys_func1(OS_FN_SAM_INSERTED,(DWORD)(x))

// DWORD os_sam_on(BYTE * a_history);
#define os_sam_on(x)            sys_func1(OS_FN_SAM_ON,(DWORD)(x))

// void os_sam_off(void);
#define os_sam_off()            sys_func0(OS_FN_SAM_OFF)

// BOOLEAN os_sam_t1(T_T1 * a_ptr);
#define os_sam_t1(x)            sys_func1(OS_FN_SAM_T1,(DWORD)(x))

// BOOLEAN os_sam_change(void);
#define os_sam_change()         sys_func0(OS_FN_SAM_CHANGE)

                /****** mmc routine ******/
#define K_MMCOpenError          ((DWORD)-1)
#define K_NoCard                0
#define K_MMC                   1
#define K_SD                    2         // < 4G
#define K_SDHC                  3         // up to 4T

// DWORD os_mmc_open(void);
#define os_mmc_open()           sys_func0(OS_FN_MMC_OPEN)

// void os_mmc_close(void);
#define os_mmc_close()          sys_func0(OS_FN_MMC_CLOSE)

// WORD os_mmc_block_size(void);
#define os_mmc_block_size()     sys_func0(OS_FN_MMC_BLK_SIZE)

// DWORD os_mmc_size(void); // in unit 512-byte or block size
#define os_mmc_size()           sys_func0(OS_FN_MMC_SIZE)

// BOOLEAN os_mmc_pstatus(void);
#define os_mmc_pstatus()        sys_func0(OS_FN_MMC_PSTATUS)

// BOOLEAN os_mmc_block_read(T_MMC * a_ptr);
#define os_mmc_block_read(x)    sys_func1(OS_FN_MMC_BLK_READ,(DWORD)(x))

// BOOLEAN os_mmc_block_write(T_MMC * a_ptr);
#define os_mmc_block_write(x)   sys_func1(OS_FN_MMC_BLK_WRITE,(DWORD)(x))

// BOOLEAN os_mmc_read(T_MMC * a_ptr,DWORD a_len);
#define os_mmc_read(x,y)        sys_func2(OS_FN_MMC_READ,(DWORD)(x),(DWORD)(y))

// BOOLEAN os_mmc_write(T_MMC * a_ptr,DWORD a_len);
#define os_mmc_write(x,y)       sys_func2(OS_FN_MMC_WRITE,(DWORD)(x),(DWORD)(y))

// BOOLEAN os_mmc_inserted(void);
#define os_mmc_inserted()       sys_func0(OS_FN_MMC_INSERTED)

                /****** sle routine ******/

/* SLE type return by os_sle_open() */
#define K_TypeNone                0
#define K_TypeSLE4442             1
#define K_TypeSLE4428             2
#define K_TypeSLE4406             3
#define K_TypeAT88SC153           4

/* Siemens memory card AT88SC153 */
#define K_Cmd153VerifyPassword    0x30
#define K_Cmd153RdBytes           0x31
#define K_Cmd153RdFuse            0x32
#define K_Cmd153WdBytes           0x33
#define K_Cmd153WdFuse            0x34
#define K_Cmd153Auth              0x35
#define K_Cmd153AtmelF2Init       0x36
#define K_Cmd153AtmelF2           0x37

/* Siemens memory card SLE4406 */
#define K_Cmd06WdBit              0x30
#define K_Cmd06WdBits             0x31
#define K_Cmd06WdByte             0x32
#define K_Cmd06WdBytes            0x33
#define K_Cmd06RdBits             0x34
#define K_Cmd06RdBytes            0x35
#define K_Cmd06RdCount            0x36
#define K_Cmd06DecCount           0x37
#define K_Cmd06SubCount           0x38
#define K_Cmd06PscVerify          0x39
#define K_Cmd06Personalize        0x40

/* Siemens memory card SLE6636 */
#define K_Cmd36WdBit              0x30
#define K_Cmd36WdBits             0x31
#define K_Cmd36WdByte             0x32
#define K_Cmd36WdBytes            0x33
#define K_Cmd36RdBits             0x34
#define K_Cmd36RdBytes            0x35
#define K_Cmd36RdCount            0x36
#define K_Cmd36DedCount           0x37
#define K_Cmd36SubCount           0x38
#define K_Cmd36Auth               0x39

/* Siemens memory card SLE4442 */
#define K_Cmd42RdMainMem          0x30
#define K_Cmd42UpMainMem          0x38
#define K_Cmd42RdProtMem          0x34
#define K_Cmd42WrProtMem          0x3C
#define K_Cmd42RdSecuMem          0x31
#define K_Cmd42UpSecuMem          0x39
#define K_Cmd42PscVerify          0x33

/* Siemens memory card SLE4428 */
#define K_Cmd28Rd8B               0x0E
#define K_Cmd28Rd9B               0x0C
#define K_Cmd28Wr8B               0x33
#define K_Cmd28Wr9B               0x31
#define K_Cmd28PscVerify          0x30

// DWORD os_sle_open(void);
#define os_sle_open()           sys_func0(OS_FN_SLE_OPEN)

// void os_sle_close(void);
#define os_sle_close()          sys_func0(OS_FN_SLE_CLOSE)

// void os_sle_hist(BYTE * hist);
#define os_sle_hist(x)          sys_func1(OS_FN_SLE_HIST,(DWORD)(x))

// BOOLEAN os_sle_io(T_T0 * a_ptr);
#define os_sle_io(x)            sys_func1(OS_FN_SLE_IO,(DWORD)(x))

                /****** ethernet routine system only ******/

//BOOLEAN os_lan_open(void);
#define os_lan_open()           sys_func0(OS_FN_LAN_OPEN)

//void os_lan_close(void);
#define os_lan_close()          sys_func0(OS_FN_LAN_CLOSE)

//BOOLEAN os_lan_rxrdy(void);
#define os_lan_rxrdy()          sys_func0(OS_FN_LAN_RXRDY)

//BOOLEAN os_lan_txrdy(void);
#define os_lan_txrdy()          sys_func0(OS_FN_LAN_TXRDY)

//T_BUF * os_lan_read();
#define os_lan_read()           ((T_BUF *)sys_func0(OS_FN_LAN_READ))

//BOOLEAN os_lan_send(void * a_ptr);
#define os_lan_send(x)          sys_func1(OS_FN_LAN_SEND,(DWORD)(x))

//DWORD os_lan_status(DWORD a_select);
//a_select = 0 => read and clear status
//a_select = 1 => read status only
#define K_LanRCStatus       0
#define K_LanRStatus        1
#define os_lan_status(x)        sys_func1(OS_FN_LAN_STATUS,(x))

                /****** msr routine ******/

#define K_MsrReset              0x80
#define K_MsrClear              0x40
#define K_AllTracks             0x07
#define K_Track2                0x01
#define K_Track1                (K_Track2 << 1)
#define K_Track3                (K_Track2 << 2)
#define K_Trk1BufLen            158 // 79
#define K_Trk2BufLen            80  // 40
#define K_Trk3BufLen            214 // 107

#define K_T2Ok                  (K_Track2)
#define K_T2Error               (K_Track2 << 8)
#define K_T1Ok                  (K_Track1)
#define K_T1Error               (K_Track1 << 8)
#define K_T3Ok                  (K_Track3)
#define K_T3Error               (K_Track3 << 8)

/*
  BYTE  b_len;
  BYTE  b_msr_mode;   (BIT 0 = TRK 2; BIT 1 = TRK 1; BIT 2 = TRK 3)
  BYTE  b_x1;         qalify time in step of 5 bytes. 0-4 and default is 0
                      qualify byte from 10-30 bytes
  BYTE  b_x2;         (not used)
  WORD  w_buffer_size;((K_Trk1BufLen+1) + (K_Trk2BufLen+1)
                       + (K_Trk3BufLen+1) + max(BUFLEN + 1)) (not used)
  BYTE   * p_buffer_address; (not used)
*/
// BOOLEAN os_msr_open(void * a_init_str);
#define os_msr_open(x)          sys_func1(OS_FN_MSR_OPEN,(DWORD)(x))

// void os_msr_close(void);
#define os_msr_close()          sys_func0(OS_FN_MSR_CLOSE)

// WORD os_msr_rxlen(void);
#define os_msr_rxlen()          sys_func0(OS_FN_MSR_RXLEN)

// BYTE os_msr_getc(void);
#define os_msr_getc()           sys_func0(OS_FN_MSR_GETC)

// WORD os_msr_read(void * a_ptr,DWORD a_max_len);
#define os_msr_read(x,y)        sys_func2(OS_FN_MSR_READ,(DWORD)(x),(DWORD)(y))

/*
  bit0 - 1 => track 2 ok
  bit1 - 1 => track 1 ok
  bit2 - 1 => track 3 ok
  bit8 - 1 => track 2 error and start sentinel found
  bit9 - 1 => track 1 error and start sentinel found
  bit10- 1 => track 3 error and start sentinel found
*/
// WORD os_msr_status(DWORD a_select);
#define os_msr_status(x)        sys_func1(OS_FN_MSR_STATUS,(DWORD)(x))

                /****** printer rountine ******/

/* printer status define */
#define K_LptOk                 0
#define K_LptErrTempHigh        1
#define K_LptErrVoltLow         2
#define K_LptErrPaperout        3
#define K_LptErrPrint           4
#define K_LptErrTime            5

#define K_LptTempHigh           0x100
#define K_LptPaperout           0x200
#define K_LptLowVolt            0x400
#define K_LptPrinting           0x800
#define K_LptFinish             0x1000

#define K_LptNormalMode         0
#define K_LptBarcodeMode        1
// a_init_str
//    byte 0 = data length follow 0-4 bytes
//    byte 1 = power in strobe width adjustment, sign byte 0-20, +ve = increase strobe width
//    byte 2 = battery mode strobe width adjustment, sign byte 0-20, +ve = increase strobe width
//    byte 3 = strobe width contrast for power in, 0-4
//    byte 4 = strobe width contrast for battery, 0-4
// BOOLEAN os_lpt_open(BYTE * a_init_str);
#define os_lpt_open(x)          sys_func1(OS_FN_LPT_OPEN,(DWORD)(x))

// void os_lpt_close(void);
#define os_lpt_close()          sys_func0(OS_FN_LPT_CLOSE)

/*
  ESC B 0        - bold off
  ESC B 1        - bold on
  ESC U 0        - underline off
  ESC U 1        - underline on
  ESC W 0        - double width off
  ESC W 1        - double width on
  ESC H 0        - double height off
  ESC H 1        - double height on
  ESC F n        - select font type
                   0 = select 16 * 12 English
                   1 = select 16 * 8 English
                   2 = select 16(H) x 12(W) English-Enlarged (32 char/line)
                   3 = select special small font of default size (8W*16H)
                   4 = select special normal font, size returned by  SPFONT
                   5 = select special normal T font size returned by SPFONT
                   6 = select special normal SP font size returned by SPFONT
                   9 = select 16 * 8 English-Enlarged
  ESC J n        - feed paper n lines
  ESC K x y data - print graphic with x is hieght in lines and y is width in byte
                   data = x * y bytes
  ESC @          - reset printer
  ESC D n        - set half dot mode
                   1 = set
                   0 = reset
  ESC R n        - RMB and back slash control, default RMB
                   0 = print back slash
                   1 = print RMB
*/
// BYTE os_lpt_putc(DWORD a_c);
#define os_lpt_putc(x)          sys_func1(OS_FN_LPT_PUTC,(DWORD)(x))

// DWORD os_lpt_putdw(DWORD a_dw);
#define os_lpt_putdw(x)         sys_func1(OS_FN_LPT_PUTDW,(DWORD)(x))

/*
  select = 0
    bit0-bit7 = printer printing status
                K_LptrOk                0
                K_ErrTempHigh           1
                K_ErrVoltLow            2
                K_ErrPaperout           3
                K_ErrPrint              4
                K_LptErrTime            5
    bit8  - temperature high
    bit9  - paper out
    bit10 - print voltage low
    bit11 - printing
    bit12 - finish
  select = 1
    restart printer if error
  select = 2
    reset printer if error
  select = 3
    reset printer
  select = 4
    read back print head temperature in ADC mV
  select = 5
    read back how much print time is left in unit of 10ms which is meaningful to new printer only
    for old printer will return -1 that means no limit
    if printer is not open it will return 0;
*/
// DWORD os_lpt_status(DWORD a_select);
#define K_LptStatusRead         0
#define K_LptStatusRestart      1
#define K_LptStatusResetError   2
#define K_LptStatusReset        3
#define K_LptStatusTemperature  4
#define K_LptStatusPrintTmLeft  5         // for new printer only

#define os_lpt_status(x)        sys_func1(OS_FN_LPT_STATUS,(DWORD)(x))

// void os_lpt_reset_count(void)
#define os_lpt_reset_count()    sys_func0(OS_FN_LPT_RESET_COUNT)

// DWORD os_lpt_get_count(void)
#define os_lpt_get_count()      sys_func0(OS_FN_LPT_GET_COUNT)


                /****** app routine ******/

#define K_APP_INFO_SEARCH_ACTIVATE (0x00000000)
#define K_APP_INFO_SEARCH_ALL      (0x80000000)
#define K_APP_INFO_SEARCH_MASK     (0x80000000)
#define K_APP_DEACTIVATED          (0x80)

// DWORD os_app_info(DWORD a_appid,T_APP_INFO * a_pdptr);
#define os_app_info(x,y)        sys_func2(OS_FN_APP_INFO,(DWORD)(x),(DWORD)(y))

// void os_appcall_return(DWORD a_ret);
#define os_appcall_return(x)    sys_func1(OS_FN_APPCALL_RETURN,(DWORD)(x))

                /****** USB routine  ******/

// ----------- USB Serial Status -------------
// bit0-7: Port status, bit8-15: USB state, bit16-23: error code, bit24-31: reserved
#define K_USB_RXRDY       0x01
#define K_USB_TXRDY       0x02
#define K_USB_DISABLED    0x80
// state
#define K_USBD_NOTATTACHED      0x0000
#define K_USBD_SUSPENDED        0x0100
#define K_USBD_ATTACHED         0x0200
#define K_USBD_POWERED          0x0300
#define K_USBD_DEFAULT          0x0400
#define K_USBD_ADDRESS          0x0500
#define K_USBD_CONFIGURED       0x0600
#define K_USBD_MASK             0x0f00
// error code
#define K_USB_LOCKED      0x010000  // end point busy or not available
#define K_USB_ABORTED     0x020000  // transfer aborted
#define K_USB_RESET       0x030000  // end point or device reset

// DWORD os_usbser_open()
#define os_usbser_open()        sys_func0(OS_FN_USBSER_OPEN)

// DWORD os_usbser_close()
#define os_usbser_close()       sys_func0(OS_FN_USBSER_CLOSE)

// DWORD os_usbser_read(x,y)
#define os_usbser_read(x,y)     sys_func2(OS_FN_USBSER_READ,(DWORD)(x),(DWORD)(y))

// DWORD os_usbser_write(x,y);
#define os_usbser_write(x,y)    sys_func2(OS_FN_USBSER_WRITE,(DWORD)(x),(DWORD)(y))

// DWORD os_usbser_status();
#define os_usbser_status()      sys_func0(OS_FN_USBSER_STATUS)

// DWORD os_usbser_enable(x);
#define os_usbser_enable(x)     sys_func1(OS_FN_USBSER_ENABLE,(DWORD)(x))

enum {
  K_USBDEV_MODE_USBSER    = 0,
  K_USBDEV_MODE_USBDKBD,
  K_USBDEV_MODE_NONE      = 0xFF,
};

// DWORD os_usbdev_open(DWORD a_mode)
#define os_usbdev_open(x)       sys_func1(OS_FN_USBSER_OPEN, (DWORD)(x))

// DWORD os_usbdev_close()
#define os_usbdev_close()       sys_func0(OS_FN_USBSER_CLOSE)

// DWORD os_usbdev_read(x,y)
#define os_usbdev_read(x,y)     sys_func2(OS_FN_USBSER_READ,(DWORD)(x),(DWORD)(y))

// DWORD os_usbdev_write(x,y);
#define os_usbdev_write(x,y)    sys_func2(OS_FN_USBSER_WRITE,(DWORD)(x),(DWORD)(y))

// DWORD os_usbdev_status();
#define os_usbdev_status()      sys_func0(OS_FN_USBSER_STATUS)

// DWORD os_usbdev_enable(x);
#define os_usbdev_enable(x)     sys_func1(OS_FN_USBSER_ENABLE,(DWORD)(x))

                /****** flash routine for system use only *****/

// status return by os_fl_status
#define K_FlashFail           0x01
#define K_FlashEdcError       0x02      // for EDC status only
#define K_FlashEdcValid       0x04      // for EDC status only
#define K_FlashIdle           0x20
#define K_FlashReady          0x40

// status return by os_fl_copyback_start
#define K_FlCopybackCmdError  0
#define K_FlCopybackFail      1
#define K_FlCopybackEdcError  2
#define K_FlCopybackEdcOk     3

// flash command option
#define K_FlCopybackReadSupport   0x01
#define K_FlEdcSupport            0x02
#define K_FlCacheReadSupport      0x04
#define K_FlBufferReadSupport     0x08
#define K_FlSmallBlockCmdSet      0x10

#define K_16MFlash            1
#define K_32MFlash            2
#define K_64MFlash            4
#define K_128MFlash           8
#define K_512MFlash           32

#define K_NAND128W3A          0x2073          // Numonyx 16M
#define K_HY27US08281A        0xAD73          // Hynix 16M
#define K_NAND256W3A          0x2075          // Numonyx 32M
#define K_HY27US08561A        0xAD75          // Hynix 32M
#define K_HY27UF084G2B        0xADDC          // Hynix 512M
#define K_NAND04GW3B2D        0x20DC          // Numonyx 512M
#define K_HY27UF081G2A        0xADF1          // Hynix 128M
#define K_K9F1G08U0D          0xECF1          // Samsung 128M
#define K_EN27LN1G08          0x92F1          // Eon Silicon 128M
#define K_PSU4GA30BT          0xC8DC          // Mira 512M
#define K_MX30LF1G18AC        0xC2F1          // Macronix 128M
#define K_MX30LF4G18AC        0xC2DC          // Macronix 512M
#define K_W29N01GV            0xEFF1          // Winbond 128M

// DWORD os_fl_info(void); // system only
// return 0        = error
//        bit0-7   = size in unit of 16M
//        bit8-15  = command option
//        bit16-31 = MCode | DCcode
#define os_fl_info()            sys_func0(OS_FN_FL_INFO)

// DWORD os_fl_status(void); // system only
#define os_fl_status()          sys_func0(OS_FN_FL_STATUS)

// DWORD os_fl_read_page(BYTE * a_buffer,DWORD a_address,DWORD a_len); // system only
#define os_fl_read_page(x,y,z)  sys_func3(OS_FN_FL_PAGE_READ,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_fl_read(BYTE * a_buffer,DWORD a_offset,DWORD a_len);; // system only
#define os_fl_read(x,y,z)       sys_func3(OS_FN_FL_PAGE,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_fl_pgm(BYTE *a_buffer,DWORD a_address,DWORD a_len); // system only
#define os_fl_pgm(x,y,z)        sys_func3(OS_FN_FL_PGM,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_fl_pgm_start(BYTE *a_buffer,DWORD a_address,DWORD a_len); // system only
#define os_fl_pgm_start(x,y,z)  sys_func3(OS_FN_FL_PGM_START,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_fl_write(BYTE * a_buffer,DWORD a_offset,DWORD a_len); // system only
#define os_fl_write(x,y,z)      sys_func3(OS_FN_FL_WRITE,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_fl_pgm_end(void); // system only
#define os_fl_pgm_end()         sys_func0(OS_FN_FL_PGM_END)

// DWORD os_fl_berase_start(DWORD a_address); // system only
#define os_fl_berase_start(x)   sys_func1(OS_FN_FL_ERASE_START,(DWORD)(x))

// DWORD os_fl_berase_end(void); // system only
#define os_fl_berase_end()      sys_func0(OS_FN_FL_ERASE_END)

// DWORD os_fl_copyb_start(BYTE *a_buffer,DWORD a_src_page,DWORD a_len); // system only
#define os_fl_copyb_start(x,y,z) sys_func3(OS_FN_FL_COPYB_START,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_fl_copyb_pgm(DWORD a_dest_page); // system only
#define os_fl_copyb_pgm(x,y,z)  sys_func1(OS_FN_FL_COPYB_PGM,(DWORD)(x))

// DWORD os_fl_copyb_end(BYTE *a_buffer,DWORD a_offset,DWORD a_len); // system only
#define os_fl_copyb_end(x,y,z)  sys_func3(OS_FN_FL_COPYB_END,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_fl_cread_rand(BYTE *a_buffer,DWORD a_offset,DWORD a_len); // system only
#define os_fl_cread_rand(x,y,z) sys_func3(OS_FN_FL_CREAD_RAND,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_fl_cread(BYTE *a_buffer,DWORD a_len); // system only
#define os_fl_cread(x,y)        sys_func2(OS_FN_FL_CREAD,(DWORD)(x),(DWORD)(y))

// DWORD os_fl_cread_end(BYTE *a_buffer,DWORD a_len); // system only
#define os_fl_cread_end(x,y)    sys_func2(OS_FN_FL_CREAD_END,(DWORD)(x),(DWORD)(y))

// DWORD os_fl_copyback(DWORD a_dest_page,DWORD a_src_page); system only
#define os_fl_copyback(x,y)     sys_func2(OS_FN_FL_COPYBACK,(DWORD)(x),(DWORD)(y))

                /****** mdm routine ******/

#define K_Albania               0x01
#define K_Algeria               0x02
#define K_Andorra               0xE0
#define K_Angola                0x43
#define K_Anguilla              0x05
#define K_AntiguaBarbuda        0x06
#define K_Argentina             0x07
#define K_Aruba                 0xE1
#define K_Ascension             0x08
#define K_Australia             0x09
#define K_Austria               0x0A
#define K_Bahamas               0x0B
#define K_Bahrain               0x0C
#define K_Bangladesh            0x0D
#define K_Barbados              0x0E
#define K_Belarus               0x1E
#define K_Belgium               0x0F
#define K_Belize                0x10
#define K_Benin                 0x11
#define K_Bermuda               0x12
#define K_Bhutan                0x13
#define K_BosniaHerzegovina     0x14
#define K_Botswana              0x15
#define K_Brazil                0x16
#define K_BritishVirginIsland   0x19
#define K_BruneiDarussalam      0x1A
#define K_Bulgaria              0x1B
#define K_BurkinaFaso           0xB6
#define K_Burundi               0x1D
#define K_Cameroon              0x1F
#define K_Canada                0x20
#define K_CaymanIslands         0x22
#define K_CentralAfricanRep     0x23
#define K_ChannelIslands        0x5D
#define K_Chile                 0x25
#define K_China                 0x26
#define K_Columbia              0x27
#define K_Comoros               0x28
#define K_CostaRica             0x2B
#define K_CoteDIvoire           0x5A
#define K_Croatia               0xFA
#define K_Cuba                  0x2C
#define K_Cyprus                0x2D
#define K_CzechRepublic         0x2E
#define K_Denmark               0x31
#define K_DiegoGarcia           0xE2
#define K_Djibouti              0x32
#define K_Dominica              0x34
#define K_DmoinicanRep          0x33
#define K_Ecuador               0x35
#define K_Egypt                 0x36
#define K_ElSalvador            0x37
#define K_Estonia               0xF9
#define K_Ethiopia              0x38
#define K_FalklandIslands       0x3A
#define K_FaroeIslands          0xE3
#define K_Fiji                  0x3B
#define K_Finland               0x3C
#define K_France                0x3D
#define K_FrenchPolynesia       0x3E
#define K_Gabon                 0x40
#define K_Gambia                0x41
#define K_Germany               0x42
#define K_Ghana                 0x44
#define K_Gibraltar             0x45
#define K_Greece                0x46
#define K_Greenland             0xE4
#define K_Grenada               0x47
#define K_Guadeloupe            0xE5
#define K_Guatemala             0x49
#define K_Guernsey              0x4A
#define K_Guiana                0xE6
#define K_Guinea                0x4B
#define K_Guayana               0x4D
#define K_Honduras              0x4F
#define K_HongKong              0x50 //   modem has problem so USA profile will be used instead
#define K_Hungary               0x51
#define K_Iceland               0x52
#define K_India                 0x53
#define K_Indonesia             0x54
#define K_Iran                  0x55
#define K_Iraq                  0x56
#define K_Ireland               0x57
#define K_Israel                0x58
#define K_Italy                 0x59
#define K_Jamaica               0x5B
#define K_Japan                 0x00
#define K_Jersey                0x5D
#define K_Jordan                0x5E
#define K_Kenya                 0x5F
#define K_Kiribati              0x60
#define K_Kuwait                0x62
#define K_Kyrgyxstan            0xE7
#define K_Lao                   0x63
#define K_Latvia                0xF8
#define K_Lebanon               0x64
#define K_Liberia               0x66
#define K_Libya                 0x67
#define K_Litechtenstein        0x68
#define K_Lithuania             0xF7
#define K_Luxembourg            0x69
#define K_Macau                 0x6A
#define K_Macedonia             0xE8
#define K_Madagascar            0x6B
#define K_Malawi                0x6D
#define K_Malaysia              0x6C
#define K_Maldives              0x6E
#define K_Mali                  0x6F
#define K_Malta                 0x70
#define K_ManIsleOf             0xE9
#define K_Martinique            0xEA
#define K_Mauritania            0x71
#define K_Mauritius             0x72
#define K_Mayotte               0xEB
#define K_Mexico                0x73
#define K_Moldova               0xEC
#define K_Monaco                0x74
#define K_Montserrat            0x76
#define K_Morocco               0x77
#define K_Mozambique            0x78
#define K_Namibia               0xED
#define K_Nauru                 0x79
#define K_Nepal                 0x7A
#define K_Netherlands           0x7B
#define K_NewCaledonia          0x7D
#define K_NewZealand            0x7E
#define K_Nicaragua             0x7F
#define K_Niger                 0x80
#define K_Nigeria               0x81
#define K_Norway                0x82
#define K_Oman                  0x83
#define K_Pakistan              0x84
#define K_Palestine             0xEE
#define K_Panama                0x85
#define K_PupuaNewGuinea        0x86
#define K_Paraguay              0x87
#define K_Peru                  0x88
#define K_Philippines           0x89
#define K_Poland                0x8A
#define K_Portugal              0x8B
#define K_PuertoRico            0x8C
#define K_Qatar                 0x8D
#define K_Reunion               0xEF
#define K_Romania               0x8E
#define K_Russia                0xB8
#define K_Rwanda                0x8F
#define K_SHelena               0x92
#define K_SKittsNevis           0x90
#define K_SLucia                0x93
#define K_SPierreMiquelon       0xF0
#define K_SVincentGrenadines    0x97
#define K_SanMarino             0x94
#define K_SaoTomePrincipe       0x96
#define K_SaudiArabia           0x98
#define K_Senegal               0x99
#define K_SerbiaMontenegro      0xF1
#define K_Seychelles            0x9A
#define K_SierraLeone           0x9B
#define K_Singapore             0x9C
#define K_Slovakia              0xC5
#define K_Slovenia              0xC6
#define K_Solomon               0x9D
#define K_SouthAfrica           0x9F
#define K_SouthKorea            0x61
#define K_Spain                 0xA0
#define K_SriLanka              0xA1
#define K_Suriname              0xA3
#define K_Swaziland             0xA4
#define K_Sweden                0xA5
#define K_Switzerland           0xA6
#define K_Syria                 0xA7
#define K_Taiwan                0xFE
#define K_Tajikistan            0xF2
#define K_Tanzania              0xA8
#define K_Thailand              0xA9
#define K_TrinidadTobago        0xAC
#define K_Tunisia               0xAD
#define K_Turkey                0xAE
#define K_Turkmenistan          0xF3
#define K_TurksCaicosIslands    0xAF
#define K_Uganda                0xB1
#define K_Ukraine               0xB2
#define K_Uae                   0xB3
#define K_UnitedKingdom         0xB4
#define K_Usa                   0xB5
#define K_Uruguay               0xB7
#define K_Vanuatu               0xB9
#define K_Vietnam               0xBC
#define K_WallisFutuna          0xBD
#define K_WesternSamoa          0xBE
#define K_Yemen                 0xC0
#define K_Yugoslavia            0xC1
#define K_Zambia                0xC3
#define K_Zimbabwe              0xC4

#define K_ConnectSpeed300       0
#define K_ConnectSpeed1200      1
#define K_ConnectSpeed2400      2
#define K_ConnectSpeed9600      3
#define K_ConnectSpeedAuto      4
#define K_ConnectSpeedAutoc     5
#define K_ConnectSpeedAutocu    6
#define K_ConnectSpeedMax14400  7
#define K_ConnectSpeedMax33600  8
#define K_ConnectSpeedMax57600  9
#define K_ConnectSpeedMax9600   10
#define K_ConnectUnionpaySupp   0x80

#define K_AsyncMode             0x00
#define K_SyncMode              0x04
#define K_CcittMode             0x00
#define K_BellMode              0x02
#define K_BlindDialing          0x08
#define K_SoftReset             0x10
#define K_HardReset             0x00
#define K_ShortDialtone         0x20
#define K_NormalDialtone        0x00
#define K_NoLoopCurrentDetect   0x40
#define K_LoopCurrentDetect     0x00
#define K_DetectBusyTone        0x00
#define K_DisableBusyTone       0x01
#define K_HypercomSpecial       0x80
//
#define K_LineInUseDetect (1<<0)
#define K_ExtensionPickupDetect (1<<1)
#define K_RemoteHangupDetect (1<<2)
#define K_MDM_STE_DEFAULT_VALUE (K_LineInUseDetect|K_ExtensionPickupDetect|K_RemoteHangupDetect)
//
#define K_DiscNormal            0x00
#define K_DiscNReset            0x01
#define K_DiscRedial            0x02
#define K_DiscTestMode          0x03
#define MDM_INIT_SIZE 8
//BOOLEAN os_mdm_open(void * a_init_str);
#define os_mdm_open(x)          sys_func1(OS_FN_MDM_OPEN,(DWORD)(x))

//void os_mdm_close(void);
#define os_mdm_close()          sys_func0(OS_FN_MDM_CLOSE)

//BOOLEAN os_mdm_dial(void * a_dial_str);
#define os_mdm_dial(x)          sys_func1(OS_FN_MDM_DIAL,(DWORD)(x))

//void os_mdm_disc(DWORD a_test_mode);
#define os_mdm_disc(x)           sys_func1(OS_FN_MDM_DISC,(x))

//DWORD os_mdm_cid(void * a_ptr);
#define os_mdm_cid(x)           sys_func1(OS_FN_MDM_CID,(DWORD)(x))

//DWORD os_mdm_status(DWORD a_select);
#define os_mdm_status(x)        sys_func1(OS_FN_MDM_STATUS,(x))

#define K_MdmStatus         0
#define K_MdmReady              0
// Data mode state
#define K_MdmConnected          1
#define K_MdmRingIn             2
#define K_MdmNoCarrier          3
#define K_MdmNoDialTone         4
#define K_MdmLineInUse          5
#define K_MdmNoLine             6
#define K_MdmBusy               7
#define K_MdmIntrusion          8
#define K_MdmDialing            9
#define K_MdmToVoice            10
#define K_MdmCid                11
#define K_MdmWrongNo            12

#define K_MdmSpeed          1
#define K_Speed300              0
#define K_Speed1200             1
#define K_Speed2400             2
#define K_Speed4800             3
#define K_Speed7200             4
#define K_Speed9600             5
#define K_Speed12000            6
#define K_Speed14400            7
#define K_Speed16800            8
#define K_Speed19200            9
#define K_Speed21600            10
#define K_Speed24000            11
#define K_Speed26400            12
#define K_Speed28000            13
#define K_Speed28800            14
#define K_Speed29333            15
#define K_Speed30666            16
#define K_Speed31200            17
#define K_Speed32000            18
#define K_Speed33333            19
#define K_Speed33600            20
#define K_Speed34000            21
#define K_Speed34666            22
#define K_Speed36000            23
#define K_Speed37333            24
#define K_Speed38000            25
#define K_Speed38666            26
#define K_Speed40000            27
#define K_Speed41333            28
#define K_Speed42000            29
#define K_Speed42666            30
#define K_Speed44000            31
#define K_Speed45333            32
#define K_Speed46000            33
#define K_Speed46666            34
#define K_Speed48000            35
#define K_Speed49333            36
#define K_Speed50000            37
#define K_Speed50666            38
#define K_Speed52000            39
#define K_Speed53333            40
#define K_Speed54000            41
#define K_Speed54666            42
#define K_Speed56000            43

#define K_MdmOnline         2   // 1 = online, 0 = offline
#define K_MdmHsInUse        3   // 0 = free, 1 = in use, 2 = no line
// ** n.b poll line free or phone off hook must be done in period > 1 sec **

#define K_MdmOnHook         4   // for cert test only, in data mode
#define K_MdmOffHook        5   // for cert test only, in data mode

#define K_MdmModel          6
  #define K_ScxV32bis             0x00
  #define K_ScxV34                0x01
  #define K_ScxV92                0x02
  #define K_Si2415                0x08
  #define K_Si2434                0x09
  #define K_Si2457                0x0A


#define K_MdmCidEnable      7
#define K_MdmCidEnableSilab 0x80
#define K_MdmCidDisable     8
#define K_MdmCidDetected    9

#define K_MdmDisconnected   10  // 0 = disconnect finish, other = disconnecting
#define K_MdmDialFinish     11  // 0 = still dialing, 2 = finish, 1 = dial timeout, for voice mode only

#define K_MdmOnlineCmdMode  12  // switch from data mode to command by sending +++ and wait for OK, not support yet
#define K_MdmOnlineDataMode 13  // switch from command mode back to data mode, not support yet
#define K_MdmSignalLevel    14  // get online signal level, not support yet
#define K_MdmSignalQual     15  // get online signal quality, not support yet
#define K_MdmTpVolt         16  // return in unit of hundredth volt, in command mode. In Online, return 0
                                // if dialing, return 10V
                                // < 3V => no line
                                // > 3V && <15V => line in use
                                // >= 15V => line free

#define K_MdmCertTest       19  // not support yet
#define K_MdmDeepSleep      0x100
#define K_MdmReadSReg       0x80000000  //0x80NN0000,  where NN is the SReg no
#define K_MdmSetSReg        0x40000000  //0x40NN00VV, where NN is the SReg no and VV is the value

//DWORD os_mdm_last_conn_stat(BYTE * a_buf); // buffer must be at least 1K
#define os_mdm_last_conn_stat(x)    sys_func1(OS_FN_MDM_LAST_CONN_STAT,(DWORD)(x))

                /****** mcom rountine ******/

#define K_ModeVisa1      0
#define K_ModeSdlc       1
#define K_ModeAsync      2

//BOOLEAN os_mcom_open(void * init_str);
#define os_mcom_open(x)         sys_func1(OS_FN_MCOM_OPEN,(DWORD)(x))

//BOOLEAN os_mcom_close(void);
#define os_mcom_close()         sys_func0(OS_FN_MCOM_CLOSE)

//BOOLEAN os_mcom_permit(void);
#define os_mcom_permit()         sys_func0(OS_FN_MCOM_PERMIT)

//BOOLEAN os_mcom_rxrdy();
#define os_mcom_rxrdy()         sys_func0(OS_FN_MCOM_RXRDY)

//BOOLEAN os_mcom_txrdy();
#define os_mcom_txrdy()         sys_func0(OS_FN_MCOM_TXRDY)

//T_BUF * os_mcom_sread(void); // system only
#define os_mcom_sread()         ((T_BUF *)sys_func0(OS_FN_MCOM_SREAD))

//BOOLEAN os_mcom_ssend(void * ptr); // system only
#define os_mcom_ssend(x)        sys_func1(OS_FN_MCOM_SSEND,(DWORD)(x))

// VISA 1
#define K_RxTimeout             1
#define K_RspTimeout            2
#define K_ConnectFail           3
#define K_LinkDrop              4
// SDLC
#define K_DiscRcvd              1               /* Disconnect received */
#define K_NoPolling             2               /* No SDLC polling */
#define K_RetryErr              3               /* Retry exceeded */
// ASYNC
#define K_Overflow              1
// SMS
#define K_Overflow              1               /* message overwritten */
#define K_Timeout               2               /* transmit timeout */
//DWORD os_mcom_status(DWORD select);
#define os_mcom_status(x)       sys_func1(OS_FN_MCOM_STATUS,(x))

//DWORD os_mcom_read(BYTE * a_buf); // app only
#define os_mcom_read(x)         sys_func1(OS_FN_MCOM_READ,(DWORD)(x))

//BOOLEAN os_mcom_send(BYTE * a_buf,DWORD a_len); // app only
#define os_mcom_send(x,y)       sys_func2(OS_FN_MCOM_SEND,(DWORD)(x),(DWORD)(y))

                /****** battery rountine ******/

// Battery level in 100mV
#define K_BatteryCharging       0x80000000
#define K_PowerPluggedIn        0x40000000
// DWORD os_batt_level(void);
#define os_batt_level()         sys_func0(OS_FN_BATT_LEVEL)

// int os_batt_temperature(void);
#define os_batt_temperature()   sys_func0(OS_FN_BATT_TEMP)

//void os_batt_control(DWORD a_control); // mfg only, default battery enable
#define K_BatteryEnable       1
#define K_BatteryDisable      0
#define os_batt_control(x)      sys_func1(OS_FN_BATT_CONTROL,(DWORD)(x))

//BYTE os_batt_charge_mode(BYTE a_mode);
#define os_batt_charge_mode(x)  sys_func1(OS_FN_BATT_CHARGE_MODE,(DWORD)(x))

// void os_batt_charge_now(void)
#define os_batt_charge_now()   sys_func0(OS_FN_BATT_CHARGE_NOW)

                /****** ADC rountine ******/
#define K_AdcBattVolt         0
#define K_AdcBattTemp         1
#define K_AdcLptTemp          2


// void os_adc_ch_enable(DWORD a_which,DWORD a_sample_time); // system only, a_sample_time in 10ms unit
#define os_adc_ch_enable(x,y)  sys_func2(OS_FN_ADC_CH_ENABLE,(DWORD)(x),(DWORD)(y))

// void os_adc_ch_disable(DWORD a_which); // system only
#define os_adc_ch_disable(x)   sys_func1(OS_FN_ADC_CH_DISABLE,(DWORD)(x))

// DWORD os_adc_ch_read(DWORD a_which); // system only, if channel not enable, return 0
#define os_adc_ch_read(x)      sys_func1(OS_FN_ADC_CH_READ,(DWORD)(x))

                /****** APP DLL rountine ******/

enum appdl_option_list
{
  K_APPDL_OPTION_NONE         = 0,
  K_APPDL_OPTION_VERIFY_ONLY  = 1,
  K_APPDL_OPTION_DELETE       = 2,
  K_APPDL_OPTION_ACTIVATE     = 3,
  K_APPDL_OPTION_DEACTIVATE   = 4,
  K_APPDL_OPTION_DATA_DELETE  = 5,
  K_APPDL_OPTION_DATA_WRITE   = 6
};

enum appdl_result_list
{
  K_APPDL_RESULT_OKAY             = 0,
  K_APPDL_RESULT_SAME             = 1,
  K_APPDL_RESULT_LOW_VERSION_ERROR = (DWORD)-13,
  K_APPDL_RESULT_OPEN_FILE_ERROR  = (DWORD)-12,
  K_APPDL_RESULT_OUT_OF_MEM       = (DWORD)-11,
  K_APPDL_RESULT_ACTIVATE_ERROR   = (DWORD)-10,
  K_APPDL_RESULT_DEACTIVATE_ERROR = (DWORD)-9,
  K_APPDL_RESULT_DELETE_ERROR     = (DWORD)-8,
  K_APPDL_RESULT_APPID_NOT_ALLOW  = (DWORD)-7,
  K_APPDL_RESULT_APP_NOT_FOUND    = (DWORD)-6,
  K_APPDL_RESULT_INVALID_HDR      = (DWORD)-5,
  K_APPDL_RESULT_CS_ERROR         = (DWORD)-4,
  K_APPDL_RESULT_AUTHERR          = (DWORD)-3,
  K_APPDL_RESULT_DATAERR          = (DWORD)-2,
  K_APPDL_RESULT_UNKNOWN          = (DWORD)-1
};

// DWORD os_appdl_write_app(const char *ap_filename)
#define os_appdl_write_app(x)       (DWORD)sys_func2(OS_FN_APPDL_WRITE_APP, (DWORD)x, (DWORD)K_APPDL_OPTION_NONE)

// DWORD os_appdl_verify_app(const char *ap_filename)
#define os_appdl_verify_app(x)      (DWORD)sys_func2(OS_FN_APPDL_WRITE_APP, (DWORD)x, (DWORD)K_APPDL_OPTION_VERIFY_ONLY)

// DWORD os_appdl_delete_app(const char *ap_filename)
#define os_appdl_delete_app(x)      (DWORD)sys_func2(OS_FN_APPDL_WRITE_APP, (DWORD)x, (DWORD)K_APPDL_OPTION_DELETE)

// DWORD os_appdl_activate_app(const char *ap_filename)
#define os_appdl_activate_app(x)    (DWORD)sys_func2(OS_FN_APPDL_WRITE_APP, (DWORD)x, (DWORD)K_APPDL_OPTION_ACTIVATE)

// DWORD os_appdl_deactivate_app(const char *ap_filename)
#define os_appdl_deactivate_app(x)  (DWORD)sys_func2(OS_FN_APPDL_WRITE_APP, (DWORD)x, (DWORD)K_APPDL_OPTION_DEACTIVATE)

// DWORD os_appdl_delete_data(const char *ap_filename, const char * ap_owner) // ap_owner = <2-digit hex appid><app name> max 12
#define os_appdl_delete_data(x,y)   (DWORD)sys_func3(OS_FN_APPDL_WRITE_APP, (DWORD)x, (DWORD)K_APPDL_OPTION_DATA_DELETE,(DWORD)y)

// DWORD os_appdl_write_data(const char *ap_filename)
#define os_appdl_write_data(x)      (DWORD)sys_func2(OS_FN_APPDL_WRITE_APP, (DWORD)x, (DWORD)K_APPDL_OPTION_DATA_WRITE)

                /****** misc routine ******/

// BOOLEAN os_rand(void * a_dest);
#define os_rand(x)              sys_func1(OS_FN_RAND,(DWORD)(x))

// void os_rand_emv_update(void);
#define os_rand_emv_update()    sys_func0(OS_FN_RAND_UPDATE)

#define K_AppIdMask             0x000000ff
#define K_ProcIdMask            0x0000ff00
#define K_KeyAuthMask           0x00070000
#define K_SecAuthed             0x00010000      // indicates authed that security functions can be used
#define K_DllAuthed             0x00020000      // indicates authed that app dll funcitons can be used
#define K_AuthAuthed            0x00040000      // indicate authed that auth functions can be used
// DWORD os_get_caller_id(void);
#define os_get_caller_id()      sys_func0(OS_FN_GET_CALLID)

// void os_sleep(void);
#define os_sleep()              sys_func0(OS_FN_SLEEP)

// exit code
#define K_PowerSleepEntryFailGPS      -22
#define K_PowerSleepEntryFailWiFi     -21
#define K_PowerSleepEntryFailGprsErr  -20
#define K_PowerSleepEntryFailLine     -19
#define K_PowerSleepEntryFailPpp      -18
#define K_PowerSleepEntryFailGprsUp   -17
#define K_PowerSleepEntryFailGprs     -16
#define K_PowerSleepEntryFailCodec    -15
#define K_PowerSleepEntryFailReserved -14
//#define K_PowerSleepEntryFailRC531    K_PowerSleepEntryFailCLicc
#define K_PowerSleepEntryFailClicc    -13
#define K_PowerSleepEntryFailMdm      -12
#define K_PowerSleepEntryFailLpt      -11
#define K_PowerSleepEntryFailSle      -10
#define K_PowerSleepEntryFailMmc      -9
#define K_PowerSleepEntryFailMsr      -8
#define K_PowerSleepEntryFailSam      -7
#define K_PowerSleepEntryFailCom2     -6
#define K_PowerSleepEntryFailAux1     K_PowerSleepEntryFailCom2
#define K_PowerSleepEntryFailCom1     -5
#define K_PowerSleepEntryFailAuxdbg   K_PowerSleepEntryFailCom1
#define K_PowerSleepEntryFailLan      -4
#define K_PowerSleepEntryFailUsb      -3
#define K_PowerSleepExitFail          -2
#define K_PowerSleepEntryFail         -1
#define K_PowerSleepEntryFailThread   0
#define K_PowerSleepPowerKeyPress     1
#define K_PowerSleepPowerUnplugged    2
#define K_PowerSleepPowerPluggedIn    3
#define K_PowerSleepKeyPadPress       4   // All key wake up
 //int os_power_sleep(void);
#define os_power_sleep()          (int)sys_func1(OS_FN_POWER_SLEEP,1) // Poll On Key Only
#define os_power_sleep_nk_npwake()  (int)sys_func1(OS_FN_POWER_SLEEP,2) // Poll Power Only (Ext. Power Removal)
#define os_power_sleep_pwake()      (int)sys_func1(OS_FN_POWER_SLEEP,3) // Poll On Key + Ext. Power plug-in (ECR17-0008)
#define os_power_sleep_ak_pwake() (int)sys_func1(OS_FN_POWER_SLEEP,4) // Poll All Key + Ext. Power plug-in (ECR17-0029)
#define os_power_sleep_awake()      (int)sys_func1(OS_FN_POWER_SLEEP,0) // Poll All

// void os_reset(void);
#define os_reset()              sys_func0(OS_FN_RESET)

// DWORD os_free_run(void);
#define os_free_run()           sys_func0(OS_FN_FREE_RUN)

//void os_rdll_app(void);
#define os_rdll_app()           sys_func0(OS_FN_RDLL_APP)

//void os_dummy(void);
#define os_dummy()              sys_func0(OS_FN_DUMMY)

// int os_off(void);
#define os_off()                sys_func0(OS_FN_OFF)

// BOOLEAN os_dmk_kvc_read(BYTE *a_kvc);  // return 2-byte KVC
#define os_dmk_kvc_read(x)      sys_func1(OS_FN_DMK_KVC_READ,(DWORD)(x))

// int os_dmk_inject(BYTE *a_dmk);    // need K_AuthAppDll, return 1 = successful
#define os_dmk_inject(x)        sys_func1(OS_FN_DMK_INJECT,(DWORD)(x))

#define K_RmbOffset             (0x80 - 0x5C)
// void os_rmb_control(DWORD a_on);
#define os_rmb_control(x)       sys_func1(OS_FN_RMB_CONTROL,(DWORD)x)

#define K_NoExternalPwr           0
#define K_UsbPwr                  1
#define K_9VPwr                   2
// int os_power(void);
#define os_power(void)          sys_func0(OS_FN_POWER)

              /****** Hardware Config ******/
// standard config dword
#define K_HdModem               0x00000003
  #define K_HdNoModem             0x00000000
  #define K_HdMdm14400            0x00000001
  #define K_HdMdm33600            0x00000002
  #define K_HdMdm56000            0x00000003
#define K_HdGprs                0x0000001C
#define K_HdGprsShift           2
  #define K_HdNoGprs             0x00000000
  #define K_HdFibocomG510        0x00000004
  #define K_HdFibocomH330        0x00000008
  #define K_HdQuectelUC15        0x0000000C
  #define K_HdSierraHL8548       0x00000010
  #define K_HdSierraHL6528       0x00000014
#define K_HdLan                 0x00000020
  #define K_HdNoLan               0x00000000
  #define K_HdLanInstalled        0x00000020
#define K_HdCICC                0x000000C0
  #define K_HdNoCICC              0x00000000
  #define K_HdAS3911Installed     0x00000040
  #define K_HdOM9663Installed     0x00000080
#define K_HdFlash               0x00000100
  #define K_Hd128MFlash           0x00000000
  #define K_Hd512MFlash           0x00000100
#define K_HdDdrRam              0x00000200
  #define K_Hd64MDdr             0x00000000
  #define K_Hd128MDdr            0x00000200
#define K_HdSam                 0x00003C00
#define K_HdSamShift            10
  #define K_HdNoSam               0x00000000
  #define K_Hd1SamInstalled       0x00000400
  #define K_Hd2SamInstalled       0x00000800
  #define K_Hd3SamInstalled       0x00001000
  #define K_Hd4SamInstalled       0x00002000
#define K_HdIcc                 0x00004000
  #define K_HdNoIcc               0x00000000
  #define K_HdIccInstalled        0x00004000
#define K_HdSim                 0x00018000
#define K_HdSimShift            15
  #define K_HdNoSim               0x00000000
  #define K_Hd1SimInstalled       0x00008000
  #define K_Hd2SimInstalled       0x00010000
#define K_HduSD                 0x00020000
  #define K_HdNouSD               0x00000000
  #define K_HduSDInstalled        0x00020000
#define K_HdBattery             0x00040000
  #define K_HdBatteryInstalled    0x00000000
  #define K_HdNoBattery           0x00040000
#define K_HdCodec               0x00080000
  #define K_HdNoCodec             0x00000000
  #define K_HdCodecInstalled      0x00080000
#define K_HdAux                 0x00100000
  #define K_Hd1AuxInstalled       0x00000000
  #define K_Hd2AuxInstalled       0x01000000
#define K_HdTPad                0x00200000
  #define K_HdNoTPad              0x00000000
  #define K_HdTPadinstalled       0x00200000
#define K_HdBarcode             0x00C00000
  #define K_HdNoBarcode           0x00000000
  #define K_Hd1dBarcode           0x00400000
  #define K_Hd2dBarcode           0x00800000
#define K_HdPcb                 0xff000000
#define K_HdPcbShift            24
  #define K_HdMainPcb             0xf0000000
  #define K_HdCpuPcb              0x0f000000

// DWORD os_hd_config(void);
#define os_hd_config()          sys_func0(OS_FN_HD_CONFIG)

#define K_HdConfigRead          0
#define K_HdConfigExtendRead    1
#define K_HdXConfigShift        24
#define K_HdXConfigBit          0x80000000

/* The following XHd can only be got by K_T1000XHwConfig */
// extend hd config byte
#define K_XHdCICCLed            0x01
  #define K_XHdCICCCColor         0x00
  #define K_XHdCICCGreen          0x01
#define K_XHdModemBrand         0x02
  #define K_XHdModemSiLab         0x02
#define K_XHdICCMode            0x04
  #define K_XHdICCNormal          0x00
  #define K_XHdICCInverse         0x04
#define K_XHdWifi               0x08
  #define K_XHdNoWifi             0x00
  #define K_XHdWifiATWINC1500          0x08
#define K_XHdGPS                 0x10
  #define K_XHdNoGPS               0x00
  #define K_XHdGPSInstalled        0x10
#define K_XHdImageSensor         0x20
  #define K_XHdNoImageSensor       0x00
  #define K_XHdImageSensorInstalled 0x20

// DWORD os_hd_config_extend(void);
#define os_hd_config_extend()   sys_func1(OS_FN_HD_CONFIG,K_HdConfigExtendRead)

// void os_request_dll(void);     // authenticated command
#define os_request_dll()        sys_func0(OS_FN_DLL_REQ)

                /****** mfg only routine ******/

// DWORD os_breg_mfg_read(void);    // mfg only
#define os_breg_mfg_read()      sys_func0(OS_FN_BREG_READ)

// void os_breg_mfg_write(a_value); // mfg only, a-byte
#define os_breg_mfg_write(x)    sys_func1(OS_FN_BREG_WRITE,(DWORD)(x))

#define K_Switch0             (1<<0)
#define K_Switch1             (1<<1)
#define K_Switch2             (1<<2)
#define K_Switch3             (1<<3)
#define K_Switch4             (1<<4)
#define K_Switch5             (1<<5)
#define K_Switch6             (1<<6)
#define K_Switch7             (1<<7)

// int os_tamper_on(void);  // mfg only
#define os_tamper_on()                  (int)sys_func0(OS_FN_TAMPER_ON)

// DWORD os_tamper_get_status(void);  // mfg only
#define os_tamper_get_status()          sys_func0(OS_FN_TAMPER_GET_STATUS)

// DWORD os_tamper_reset_status(void);  // mfg only
#define os_tamper_reset_status()        sys_func1(OS_FN_TAMPER_RESET_STATUS,1)

/*------------------------- GPRS Functions ---------------------------------*/

// signal level 0-5
// DWORD os_gprs_signal(void);
#define os_gprs_signal()        sys_func0(OS_FN_GPRS_SIGNAL)

enum gprs_status_list
{
  K_GPRS_STATUS_CALLSTATE =             0x3f,
  K_GPRS_STATUS_PSSUP =               0x0100,
  K_GPRS_STATUS_READY =               0x0200,
  K_GPRS_STATUS_NETWORK_READY =       0x0400,
  K_GPRS_STATUS_PHONEBOOK_READY =     0x0800,
  K_GPRS_STATUS_SMS_READY =           0x1000,
  K_GPRS_STATUS_CBM_READY =           0x2000,
  K_GPRS_STATUS_RING =                0x4000,
  K_GPRS_STATUS_CID =                 0x8000,
  K_GPRS_STATUS_TIMEOUT =           0x010000,
  K_GPRS_STATUS_CMUXUP =            0x020000,
  K_GPRS_STATUS_CD =                0x040000,
  K_GPRS_STATUS_LINE =              0x080000,
  K_GPRS_STATUS_SMS_SEND_OK =       0x100000,
  K_GPRS_STATUS_SMS_SEND_ERROR =    0x200000,
  K_GPRS_STATUS_SMS_SEND_TIMEOUT =  0x400000,
  K_GPRS_STATUS_SMSRXRDY =          0x800000,
  K_GPRS_STATUS_TXRDY1 =          0x01000000,
  K_GPRS_STATUS_TXRDY2 =          0x02000000,
  K_GPRS_STATUS_RXRDY1 =          0x04000000,
  K_GPRS_STATUS_RXRDY2 =          0x08000000,
  K_GPRS_STATUS_POWER =           0x30000000,
  K_GPRS_STATUS_ROUTER =          0x40000000,
  K_GPRS_STATUS_ALL = (DWORD)-1
};

// For K_GPRS_STATUS_POWER
#define K_GPRS_STATUS_POWER_OFF   0x00000000
#define K_GPRS_STATUS_POWERING    0x10000000
#define K_GPRS_STATUS_POWER_ON    0x20000000
#define K_GPRS_STATUS_POWER_ERROR 0x30000000

// DWORD os_gprs_status(DWORD a_mask);
#define os_gprs_status(x)        sys_func1(OS_FN_GPRS_STATUS,(DWORD)(x))

// DWORD os_gprs_at_cmd(BYTE *a_cmd, DWORD a_timeout);
#define os_gprs_at_cmd(x,y)     sys_func2(OS_FN_GPRS_AT_CMD,(DWORD)(x),(DWORD)(y))

// DWORD os_gprs_at_resp(BYTE *a_resp,DWORD a_maxlen);
#define os_gprs_at_resp(x,y)    sys_func2(OS_FN_GPRS_AT_RESP,(DWORD)(x),(DWORD)(y))

// DWORD os_gprs_send(T_BUF *a_ptr);  // system only
#define os_gprs_send(x)         sys_func1(OS_FN_GPRS_SEND,(DWORD)(x))

// T_BUF * os_gprs_receive(void);     // system only
#define os_gprs_receive()       (T_BUF*)sys_func0(OS_FN_GPRS_READ)

enum gprs_mode_list
{
  K_GPRS_MODE_INIT = 0,
  K_GPRS_MODE_NORMAL,
  K_GPRS_MODE_ROUTER,      // system only
  K_GPRS_MODE_UPDATE       // system only
};

// DWORD os_gprs_reset(DWORD a_mode);
#define os_gprs_reset(x)        sys_func1(OS_FN_GPRS_RESET,(DWORD)(x))
#define os_gprs_is_HL6RD()              sys_func0(OS_FN_GPRS_IS_HL6RD)  // HL6RD|HL6 compatible
#define os_gprs_is_HL6RD_old_frm()          sys_func0(OS_FN_GPRS_IS_HL6RD_OLD_FRM)  // HL6RD|HL6 compatible

enum gprs_control_list
{
  K_GPRS_CONTROL_DCHAN_OPEN = 0,  // system only
  K_GPRS_CONTROL_DCHAN_CLOSE,     // system only
  K_GPRS_ON,
  K_GPRS_OFF,
  K_GPRS_CONTROL_GET_PRODUCT_NAME,
  K_GPRS_CONTROL_PPP_SET_USERNAME,
  K_GPRS_CONTROL_PPP_SET_PASSWORD,
  K_GPRS_CONTROL_PPP_SET_APN,
  K_GPRS_CONTROL_PPP_WRITE_PROFILE,
  K_GPRS_CONTROL_PPP_GET_PHASE,
  K_GPRS_CONTROL_PPP_CONNECT,
  K_GPRS_CONTROL_PPP_DISCONNECT,
  K_GPRS_CONTROL_DOWNLOAD_FIRMWARE,
  K_GPRS_CONTROL_PPP_GET_UPTIME,
  K_GPRS_CONTROL_GET_LINE_STATE,
  K_GPRS_CONTROL_SET_LINE_STATE,
  K_GPRS_CONTROL_CLEAR_LINE_STATE,
  K_GPRS_CONTROL_SET_FREQBAND,
  K_GPRS_CONTROL_GET_CURRENT_BAND,
/* For non-sierra modules */
  K_GPRS_CONTROL_GET_DAT_LINE_STATE,
  K_GPRS_CONTROL_SET_DAT_LINE_STATE,
  K_GPRS_CONTROL_CLEAR_DAT_LINE_STATE,

  K_GPRS_CONTROL_IS_ROUTER_MODE_READY,
  K_GPRS_CONTROL_GET_CURRENT_FREQ,
  K_GPRS_CONTROL_CHANGE_SIM, //Sim selection. Application should save config before calling.
};

enum gprs_band_list
{
  K_GPRS_BAND_G850  = (1<<0),
  K_GPRS_BAND_G900  = (1<<1),
  K_GPRS_BAND_G1800 = (1<<2),
  K_GPRS_BAND_G1900 = (1<<3),
  K_GPRS_BAND_W850  = (1<<4),
  K_GPRS_BAND_W900  = (1<<5),
  K_GPRS_BAND_W1900 = (1<<6),
  K_GPRS_BAND_W2100 = (1<<7),
  K_GPRS_BAND_OTHER = (1<<8),
  K_GPRS_BAND_UPDATED = (1<<9),
/* The following is defined for Sierra HL8548x */
  K_GPRS_BAND_E_GSM = (1<<10),
};


// DWORD os_gprs_control(DWORD a_control);
#define os_gprs_control(x)        sys_func1(OS_FN_GPRS_CONTROL, x)
// DWORD os_gprs_control1(DWORD a_control, DWORD a_param1);
#define os_gprs_control1(x, y)    sys_func2(OS_FN_GPRS_CONTROL, x, (DWORD)(y))
// DWORD os_gprs_control2(DWORD a_control, DWORD a_param1, DWORD a_param2);
#define os_gprs_control2(x, y, z) sys_func3(OS_FN_GPRS_CONTROL, x, (DWORD)(y), (DWORD)(z))

// DWORD os_gprs_get_cid(void *a_dest, DWORD a_len)
#define os_gprs_get_cid(x, y)   sys_func2(OS_FN_GPRS_GET_CID, (DWORD)(x), y)

// DWORD os_gprs_read_ussd(void *a_ptr, DWORD a_len)
#define os_gprs_read_ussd(x, y)   sys_func2(OS_FN_GPRS_READ_USSD, (DWORD)(x), y)

                /****** gprs sms routine ******/

enum gprs_sms_addrtype_list
{
  K_GPRS_SMS_ADDRTYPE_UNKNOWN       = 0x81, // 129
  K_GPRS_SMS_ADDRTYPE_INTERNATIONAL = 0x91, // 145
};

enum gprs_sms_param_list
{
  K_GPRS_SMS_PARAM_STATUS_REC_UNREAD  = 0,
  K_GPRS_SMS_PARAM_STATUS_REC_READ    = 1,
  K_GPRS_SMS_PARAM_STATUS_STO_UNSENT  = 2,
  K_GPRS_SMS_PARAM_STATUS_STO_SENT    = 3,
  K_GPRS_SMS_PARAM_STATUS_MASK        = 0x03,
  K_GPRS_SMS_PARAM_DEST_STORAGE       = 0x08,
  K_GPRS_SMS_PARAM_MODE_PDU           = 0x00,
  K_GPRS_SMS_PARAM_MODE_TEXT          = 0x10
};

enum gprs_sms_tp_list
{
  K_GPRS_SMS_TP_RP   = 0x80,  // (for both) reply path
  K_GPRS_SMS_TP_UDHI = 0x40,  // (for both) user data header indicator
  K_GPRS_SMS_TP_SRI  = 0x20,  // (for deliver) status report indicator
  K_GPRS_SMS_TP_SRR  = 0x20,  // (for submit) status report request
  K_GPRS_SMS_TP_SRQ  = 0x20,  // (for status report) status report qualifier
  K_GPRS_SMS_TP_VPF  = 0x18,  // (for submit) validity period format
  K_GPRS_SMS_TP_MMS  = 0x04,  // (for deliver and status report) more messages to send
  K_GPRS_SMS_TP_RD   = 0x04,  // (for submit) reject duplicates
  K_GPRS_SMS_TP_MTI  = 0x03   // (for all) messge type indicator, bit 0-1
};

enum gprs_sms_tp_mti_list
{
  K_GPRS_SMS_TP_MTI_SMS_DELIVER        = 0,   // recv
  K_GPRS_SMS_TP_MTI_SMS_DELIVER_REPORT = 0,   // xmit
  K_GPRS_SMS_TP_MTI_SMS_SUBMIT         = 1,   // xmit
  K_GPRS_SMS_TP_MTI_SMS_SUBMIT_REPORT  = 1,   // recv
  K_GPRS_SMS_TP_MTI_SMS_STATUS_REPORT  = 2,   // recv
  K_GPRS_SMS_TP_MTI_SMS_COMMAND        = 2    // xmit
};

enum gprs_sms_pid_list
{
  K_GPRS_SMS_PID_SME2SME   = 0x00, // No interworking, sme to sme
  K_GPRS_SMS_PID_IMPLICIT  = 0x20, // device type is specific to this SC, or can be concluded on the basis of the address
  K_GPRS_SMS_PID_TELEX     = 0x21, // telex
  K_GPRS_SMS_PID_G3FAX     = 0x22, // group 3 telefax
  K_GPRS_SMS_PID_G4FAX     = 0x23, // group 4 telefax
  K_GPRS_SMS_PID_VOICE     = 0x24, // voice phone
  K_GPRS_SMS_PID_ERMES     = 0x25, // european radio messaging system
  K_GPRS_SMS_PID_NPS       = 0x26, // national paging system
  K_GPRS_SMS_PID_VIDEOTEX  = 0x27, // videotex
  K_GPRS_SMS_PID_TTX       = 0x28, // Teletex, carrier unspecified
  K_GPRS_SMS_PID_TTX_PSPDN = 0x29, // teletex in pspdn
  K_GPRS_SMS_PID_TTX_CSPDN = 0x2a, // teletex in cspdn
  K_GPRS_SMS_PID_TTX_PSTN  = 0x2b, // teletex in analog pstn
  K_GPRS_SMS_PID_TTX_ISDN  = 0x2c, // teletex in isdn
  K_GPRS_SMS_PID_UCI       = 0x2d, // UCI (Universal Computer Interface, ETSI DE/PS 3 01-3)
  K_GPRS_SMS_PID_MHF       = 0x30, // a message handling facility (known to the SC)
  K_GPRS_SMS_PID_X400      = 0x31, // any public X.400-based message handling system
  K_GPRS_SMS_PID_EMAIL     = 0x32, // Internet electronic mail
  K_GPRS_SMS_PID_GSM       = 0x3f, // A GSM mobile station. The SC converts the SM from the received TP-Data-Coding-Scheme to any data coding scheme supported by that MS (e.g. the default).
  K_GPRS_SMS_PID_TYPE0     = 0x40, // Short message type 0
  K_GPRS_SMS_PID_TYPE1     = 0x41, // Replace Short message type 1
  K_GPRS_SMS_PID_TYPE2     = 0x42, // Replace Short message type 2
  K_GPRS_SMS_PID_TYPE3     = 0x43, // Replace Short message type 3
  K_GPRS_SMS_PID_TYPE4     = 0x44, // Replace Short message type 4
  K_GPRS_SMS_PID_TYPE5     = 0x45, // Replace Short message type 5
  K_GPRS_SMS_PID_TYPE6     = 0x46, // Replace Short message type 6
  K_GPRS_SMS_PID_TYPE7     = 0x47, // Replace Short message type 7
  K_GPRS_SMS_PID_RCM       = 0x5f, // return call message
  K_GPRS_SMS_PID_MEDL      = 0x7d, // me data download
  K_GPRS_SMS_PID_MEDP      = 0x7e, // me de-personalization short message
  K_GPRS_SMS_PID_SIMDL     = 0x7f, // sim data download
};

enum gprs_sms_tp_dcs_list
{
  //sms data coding scheme (tp_dcs)
  // for bit7-6 = 00 (GENERAL DATA CODING INDICATION)
  K_GPRS_SMS_TP_DCS_TEXT_COMP  = 0x20, // text is compressed
  K_GPRS_SMS_TP_DCS_HAVE_CLASS = 0x10, // have message class
  K_GPRS_SMS_TP_DCS_CLASS      = 0x03, // message class 0-3
  K_GPRS_SMS_TP_DCS_ALPHA      = 0x0c, // alphabet (0=default 7-bit, 1=8-bit, 2=ucs2)
  // for bit7-4 = 1100 (Message Waiting Indication Group: Discard Message)
  // bit 7-4 = 1101 (Message Waiting Indication Group: Store Message)
  // bit 7-4 = 1110 (Message Waiting Indication Group: Store Message)
  K_GPRS_SMS_TP_DCS_IND_ACTIVE = 0x08, // set indication active
  K_GPRS_SMS_TP_DCS_IND_TYPE   = 0x03, // indication type, value defined below
  K_GPRS_SMS_TP_DCS_IND_VMAIL  = 0x00, // value 0 = voice mail message waiting
  K_GPRS_SMS_TP_DCS_IND_FAX    = 0x01, // value 1 = FAX message waiting
  K_GRPS_SMS_TP_DCS_IND_EMAIL  = 0x02, // value 2 = Electronic mail message waiting
  K_GPRS_SMS_TP_DCS_IND_OTHER  = 0x03, // value 3 = Other message waiting
  //bit 7-4 = 1111 (Data coding / message class)
  // bit 1-0 are message class, same as General data coding indciation
};

// validity period (TP_VP in relative format) defines
// 0 to 143, 5mins-12hours (5min interval, x=5-720)
#define GPRS_SMS_TP_VP_5MIN(x)          (((x) / 5) - 1)
// 144 to 167, 12hours-24hours (half hour interval, x=25-48)
#define GPRS_SMS_TP_VP_HALFHOUR(x)      ((x) - 24 + 143)
// 168 to 196, 2days-30days (1 day interval, x=2-30)
#define GPRS_SMS_TP_VP_DAY(x)           ((x) + 166)
// 197 to 255, 5weeks-63weeks (1 week interval, x=5-63)
#define GPRS_SMS_TP_VP_WEEK(x)          ((x)+192)

enum gprs_sms_tp_vpf_list
{
  K_GPRS_SMS_TP_VPF_RELATIVE = 0x10,
  K_GPRS_SMS_TP_VPF_ENHANCED = 0x08,
  K_GPRS_SMS_TP_VPF_ABSOLUTE = 0x18
};

enum gprs_sms_pi_list
{
  K_GPRS_SMS_PI_PID = 0x01,
  K_GPRS_SMS_PI_DCS = 0x02,
  K_GPRS_SMS_PI_UDL = 0x04,
  K_GPRS_SMS_PID_EA = 0x80
};

enum gprs_sms_rp_list
{
  K_GPRS_SMS_RP_NONE = 0,
  K_GPRS_SMS_RP_ACK,
  K_GPRS_SMS_RP_ERROR
};

// SMS pdu struct
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif

typedef struct
{
  BYTE b_type; // 145 = international, 129 = default
  BYTE b_len;  // len = no of phone no. digits
  BYTE s_addr[20]; // ascii code
}
#ifndef __IAR_SYSTEMS_ICC__
__attribute__((packed))
#endif
T_GPRS_SMS_ADDR;

typedef struct
{
  T_GPRS_SMS_ADDR s_smsc;       // sms service center address
  BYTE            b_pdu_len;    // not use, but keep so that same as  T800 (pdu length (start from tp_flags))
  BYTE            b_tp_flags;   // sms pdu first octat
  T_GPRS_SMS_ADDR s_tp_a;       // deliver: originating address, submit: destination address
  BYTE            b_tp_mr;      // message reference
  BYTE            b_tp_st;      // status
  BYTE            b_tp_fcs;     // tp-failure-cause (for RP-ERROR only)
  BYTE            b_tp_pi;      // tp-parameter indicator
  BYTE            b_tp_pid;     // protocol identifier
  BYTE            b_tp_ct;      // command type
  BYTE            b_tp_dcs;     // data coding scheme
  BYTE            s_tp_scts[7]; // deliver: service center timestamp, submit: validaity period
  BYTE            s_tp_dt[7];   // discharge time
  BYTE            b_tp_udl;     // user data len, max. 160 for 7-bit data, otherwise max. 140
  BYTE            s_tp_ud[160]; // user data (ascii code)
}
#ifndef __IAR_SYSTEMS_ICC__
__attribute__((packed))
#endif
T_GPRS_SMS_PDU;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif

// DWORD os_gprs_sms_decode(T_GPRS_SMS_PDU *a_pdu, void *a_src, DWORD a_len)
#define os_gprs_sms_decode(x,y,z) sys_func3(OS_FN_GPRS_SMS_DECODE,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_gprs_sms_ack(DWORD a_rp, T_GPRS_SMS_PDU *a_pdu)
#define os_gprs_sms_ack(x,y)    sys_func2(OS_FN_GPRS_SMS_ACK,(DWORD)(x),(DWORD)(y))

// DWORD os_gprs_sms_send(T_GPRS_SMS_PDU *a_pdu, DWORD a_param)
#define os_gprs_sms_send(x,y)   sys_func2(OS_FN_GPRS_SMS_SEND,(DWORD)(x),(DWORD)(y))

// DWORD os_gprs_sms_control(DWORD a_enabled)
#define os_gprs_sms_control(x)  sys_func1(OS_FN_GPRS_SMS_CONTROL,(DWORD)(x))

// DWORD os_gprs_sms_read(void *a_dest, DWORD a_len)
#define os_gprs_sms_read(x,y)  sys_func2(OS_FN_GPRS_SMS_READ,(DWORD)(x),(DWORD)(y))

#define os_gprs_set_pin_status(x,y) sys_func2(OS_FN_GPRS_SET_PIN_STATUS,(DWORD)x, (DWORD)y)

                /****** codec routine *****/

// status return from os_codec_play or os_codec_play_file
#define K_CodecOpened               2
#define K_CodecPlaying              1
#define K_WrongState                0
#define K_ModemError                -1
#define K_NotEnoughMemory           -2
#define K_FileOpenError             -3
#define K_FileSeekError             -4
#define K_FileReadError             -5
#define K_NotWaveFile               -6
#define K_FormatNotSupport          -7
#define K_NotFinishError            -8
#define K_CodecInitError            -9
#define K_InternalError             -10
#define K_OverMaxSize               -11       // max size of wave data is 16M

// select in os_codec_status
#define K_CodecStatus           0
#define K_CodecVolume           0x80000000
// select = K_CodecVolume + x for set codec volume, where x from 0 to 15, 0 is lowest volume

// BOOLEAN os_codec_open(void);
#define os_codec_open()         sys_func0(OS_FN_CODEC_OPEN)

// BOOLEAN os_codec_close(void);
#define os_codec_close()        sys_func0(OS_FN_CODEC_CLOSE)

// if file is not in local directory, full path is needed
// DWORD os_codec_play(BYTE * a_wave_file,DWORD a_offset,DWORD a_len);
#define os_codec_play(x,y,z)    sys_func3(OS_FN_CODEC_PLAY,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_codec_play_ram(BYTE * a_ptr,DWORD a_len);
#define os_codec_play_ram(x,y)  sys_func2(OS_FN_CODEC_PLAY_RAM,(DWORD)(x),(DWORD)(y))

// BOOLEAN os_codec_stop(void);
#define os_codec_stop()         sys_func0(OS_FN_CODEC_STOP)

// DWORD os_codec_status(DWORD a_select);
#define os_codec_status(x)      sys_func1(OS_FN_CODEC_STATUS,(DWORD)(x))

                /****** rc531 driver rountine ******/

#define K_ERR_RC531_OK          K_ERR_CLICC_OK
#define K_ERR_RC531_NO_CARD     K_ERR_CLICC_NO_CARD
#define K_ERR_RC531_COLLISION   K_ERR_CLICC_COLLISION
#define K_ERR_RC531_TRANS       K_ERR_CLICC_TRANS
#define K_ERR_RC531_PROTOCOL    K_ERR_CLICC_PROTOCOL
#define K_ERR_RC531_TIMEOUT     K_ERR_CLICC_TIMEOUT
#define K_ERR_RC531_INTERNAL    K_ERR_CLICC_INTERNAL
#define K_ERR_RC531_NOT_READY   K_ERR_CLICC_NOT_READY
#define K_ERR_RC531_INPUT       K_ERR_CLICC_INPUT
#define K_ERR_RC531_WTX_TIMEOUT K_ERR_CLICC_WTX_TIMEOUT
#define K_ERR_RC531_ABORT       K_ERR_CLICC_ABORT

#define K_RC531_CTRL_NORMAL     K_CLICC_CTRL_NORMAL
#define K_RC531_CTRL_ABORT      K_CLICC_CTRL_ABORT
#define K_RC531_CTRL_POKE       K_CLICC_CTRL_POKE

#define K_RC531_INS_CARRIER     K_CLICC_INS_CARRIER
#define K_RC531_INS_POLLING     K_CLICC_INS_POLLING
#define K_RC531_INS_RESET       K_CLICC_INS_RESET
#define K_RC531_INS_WUPA        K_CLICC_INS_WUPA
#define K_RC531_INS_HLTA        K_CLICC_INS_HLTA
#define K_RC531_INS_ARATS       K_CLICC_INS_ARATS
#define K_RC531_INS_WUPB        K_CLICC_INS_WUPB
#define K_RC531_INS_HLTB        K_CLICC_INS_HLTB
#define K_RC531_INS_BATTR       K_CLICC_INS_BATTR
#define K_RC531_INS_PRE_VALID   K_CLICC_INS_PRE_VALID
#define K_RC531_INS_LOOPBACK    K_CLICC_INS_LOOPBACK
#define K_RC531_INS_REG_SET     K_CLICC_INS_REG_SET
#define K_RC531_INS_REG_GET     K_CLICC_INS_REG_GET
#define K_RC531_INS_CMD_SEND    K_CLICC_INS_CMD_SEND
#define K_RC531_INS_END         K_CLICC_INS_END

// int os_rc531_open(void);
#define os_rc531_open           os_clicc_open

// int os_rc531_poll(BYTE* a_init_str)
#define os_rc531_poll           os_clicc_poll

// int os_rc531_remove(DWORD timeout)
#define os_rc531_remove         os_clicc_remove

// void os_rc531_close(void)
#define os_rc531_close          os_clicc_close

// int os_rc531_SendCommand(BYTE* a_in, BYTE* a_out, WORD a_len)
#define os_rc531_SendCommand    os_clicc_SendCommand

// DWORD os_rc531_ctrl(DWORD a_ctrl)
#define os_rc531_ctrl           os_clicc_ctrl

// BYTE os_rc531_emv_lv1(BYTE a_ins, DWORD param1, DWORD param2)
#define os_rc531_emv_lv1        os_clicc_emv_lv1

                /****** clicc driver rountine ******/

#define K_ERR_CLICC_OK          (0)
#define K_ERR_CLICC_NO_CARD     (-1)
#define K_ERR_CLICC_COLLISION   (-2)
#define K_ERR_CLICC_TRANS       (-3)
#define K_ERR_CLICC_PROTOCOL    (-4)
#define K_ERR_CLICC_TIMEOUT     (-5)
#define K_ERR_CLICC_INTERNAL    (-6)
#define K_ERR_CLICC_NOT_READY   (-7)
#define K_ERR_CLICC_INPUT       (-8)
#define K_ERR_CLICC_WTX_TIMEOUT (-9)
#define K_ERR_CLICC_ABORT       (-10)

// int os_clicc_open(void);
#define os_clicc_open()               (int)sys_func0(OS_FN_CLICC_OPEN)

// int os_clicc_poll(BYTE* a_init_str)
#define os_clicc_poll(x)              (int)sys_func1(OS_FN_CLICC_POLL, (DWORD)x)

// int os_clicc_remove(DWORD timeout)
#define os_clicc_remove(x)            (int)sys_func1(OS_FN_CLICC_REMOVE, (DWORD)x)

// void os_clicc_close(void)
#define os_clicc_close()              sys_func0(OS_FN_CLICC_CLOSE)

// int os_clicc_SendCommand(BYTE* a_in, BYTE* a_out, WORD a_len)
#define os_clicc_SendCommand(x,y,z)   (int)sys_func3(OS_FN_CLICC_SENDCOMMAND, (DWORD)x, (DWORD)y, (DWORD)z)

enum {
  K_CLICC_CTRL_NORMAL = 0,
  K_CLICC_CTRL_ABORT  = 1,
  K_CLICC_CTRL_POKE   = 0xFF,
  K_CLICC_CTRL_GET_TIME = 0x100
};

// DWORD os_clicc_ctrl(DWORD a_ctrl)
#define os_clicc_ctrl(x)              sys_func1(OS_FN_CLICC_CTRL, (DWORD)(x))

enum {
  K_CLICC_INS_CARRIER = 0,
  K_CLICC_INS_POLLING,
  K_CLICC_INS_RESET,
  K_CLICC_INS_WUPA,
  K_CLICC_INS_HLTA,
  K_CLICC_INS_ARATS,
  K_CLICC_INS_WUPB,
  K_CLICC_INS_HLTB,
  K_CLICC_INS_BATTR,
  K_CLICC_INS_PRE_VALID,
  K_CLICC_INS_LOOPBACK,
  K_CLICC_INS_REG_SET,
  K_CLICC_INS_REG_GET,
  K_CLICC_INS_CMD_SEND,
  K_CLICC_INS_END
};

// BYTE os_clicc_emv_lv1(BYTE a_ins, DWORD param1, DWORD param2)
#define os_clicc_emv_lv1(x,y,z)       sys_func3(OS_FN_CLICC_EMV_LV1, (DWORD)x, y, z)

// int clicc_debug(DWORD a_ins, DWORD a_param1, DWORD a_param2);
#define os_clicc_debug(x,y,z)         (int)sys_func3(OS_FN_CLICC_DEBUG, (DWORD)x, y, z)

#if 0
                /****** rc531 LED rountine ******/

#define K_Rc531LedGreen         K_CliccLedGreen
#define K_Rc531LedBlue          K_CliccLedBlue
#define K_Rc531LedYellow        K_CliccLedYellow
#define K_Rc531LedRed           K_CliccLedRed
#define K_Rc531LedMax           K_CliccLedMax

// void os_rc531_led_open(void);
#define os_rc531_led_open           os_clicc_led_open

// void os_rc531_led_close(void);
#define os_rc531_led_close          os_clicc_led_close

// DWORD os_rc531_led_status(DWORD a_which);
#define os_rc531_led_status         os_clicc_led_status

// void os_rc531_led_set(DWORD a_which);
#define os_rc531_led_set            os_clicc_led_set

// void os_rc531_led_clear(DWORD a_which);
#define os_rc531_led_clear          os_clicc_led_clear

// void os_rc531_led_on(DWORD a_which, DWORD a_on_off, DWORD a_cycle);
#define os_rc531_led_on             os_clicc_led_on
#endif
                /****** clicc LED rountine ******/

#define K_CliccLedGreen         0
#define K_CliccLedBlue          1
#define K_CliccLedYellow        2
#define K_CliccLedRed           3
#define K_CliccLedMax           (K_CliccLedRed+1)

// void os_clicc_led_open(void);
#define os_clicc_led_open()         sys_func0(OS_FN_CLICC_LED_OPEN)

// void os_clicc_led_close(void);
#define os_clicc_led_close()        sys_func0(OS_FN_CLICC_LED_CLOSE)

// DWORD os_clicc_led_status(DWORD a_which);
#define os_clicc_led_status(x)      sys_func1(OS_FN_CLICC_LED_STATUS, (DWORD)x)

// void os_clicc_led_set(DWORD a_which);
#define os_clicc_led_set(x)         sys_func1(OS_FN_CLICC_LED_SET, (DWORD)x)

// void os_clicc_led_clear(DWORD a_which);
#define os_clicc_led_clear(x)       sys_func1(OS_FN_CLICC_LED_CLEAR, (DWORD)x)

// void os_clicc_led_on(DWORD a_which, DWORD a_on_off, DWORD a_cycle);
#define os_clicc_led_on(x,y,z)      sys_func3(OS_FN_CLICC_LED_ON, (DWORD)x, (DWORD)y, (DWORD)z)

                /****** USB Host routine  ******/

// ----------- USB system Status -------------
// bit 0 and bit 1: root hub status. See USB 2.0 spec Table 11-19
#define USB_ROOTHUB_LOCAL_POWER   0x0001
#define USB_ROOTHUB_OVERCURRENT   0x0002
// bit 2 to 10 host controller status. See OHCI specification 1.0a 6.2
#define USB_HC_PERIODIC_LIST_ENABLE 0x0004
#define USB_HC_ISOCHRONOUS_ENABLE 0x0008
#define USB_HC_CONTROL_LIST_ENABLE  0x0010
#define USB_HC_BULK_LIST_ENABLE   0x0020
#define USB_HC_FUNCTION_STATE   0x00c0
#define USB_HC_INTERRUPT_ROUTING  0x0100
#define USB_HC_REMOTE_WAKEUP_CONNECTED  0x0200
#define USB_HC_REMOTE_WAKEUP_ENABLE 0x0400

// bit 15 indicates that the usb host clock is turned on.
#define USB_HC_CLOCK_ON     0x8000
// bit 16-31 is the usb frame number
#define USB_HC_FRAME_NUMBER   0xffff0000

// value of USB_HC_FUNCTION_STATE
#define USB_HCFS_RESET        0x0000
#define USB_HCFS_RESUME       0x0040
#define USB_HCFS_OPERATIONAL  0x0080
#define USB_HCFS_SUSPEND      0x00c0

// int os_usbhost_submit_buffer(T_BUF *buf);  // usb host interface for device driver
#define os_usbhost_submit_buffer(x)   (int)sys_func1(OS_FN_USBHOST_SUBMIT_BUFFER, (DWORD)x)

//int os_usbhost_unlink_buffer(T_BUF *buf); // usb host interface for device driver
#define os_usbhost_unlink_buffer(x)   (int)sys_func1(OS_FN_USBHOST_UNLINK_BUFFER, (DWORD)x)

//int os_usbhost_reset_device(T_USBH_Device *udev); // usb host interface for device driver
#define os_usbhost_reset_device(x)  (int)sys_func1(OS_FN_USBHOST_RESET_DEVICE, (DWORD)x)

// DWORD os_usbhost_status();
#define os_usbhost_status()   sys_func0(OS_FN_USBHOST_STATUS)

// DWORD os_usbhost_devicemap();
#define os_usbhost_devicemap()    sys_func0(OS_FN_USBHOST_DEVICEMAP)

//int os_usbhost_rhport_status(int portnum);  // return roothub port status, See USB 2.0 spec Table 11-21
#define os_usbhost_rhport_status(x)        sys_func1(OS_FN_USBHOST_RHPORT_STATUS, (DWORD)(x))

//USB port number
#define USB_ROOTHUB_EXTERNAL_PORT   1     // for color
#define USB_ROOTHUB_INTERNAL_PORT   2     // for bw and colr cd

//int os_usbhost_devinfo(int devID, T_USBDevInfo *udevInfo);
#define os_usbhost_devinfo(x,y)     sys_func2(OS_FN_USBHOST_DEVINFO, (DWORD)(x), (DWORD)(y))

typedef struct S_USBDevInfo
{
  unsigned short bcdUSB;              //!< USB specification release number
  unsigned short bcdDevice;           //!< Device release number
  unsigned short idVendor;            //!< Vendor ID
  unsigned short idProduct;           //!< Product ID
  unsigned char  bDeviceClass;        //!< Device Class code
  unsigned char  bDeviceSubClass;     //!< Device Subclass code
  unsigned char  bDeviceProtocol;     //!< Device Protocol code
  unsigned char  bMaxPacketSize0;     //!< Control endpoint 0 max. packet size

  unsigned char bInterfaceNumber;     //!< Number of this interface
  unsigned char bAlternateSetting;    //!< Value used to select this alternate setting
  unsigned char bNumEndpoints;        //!< Number of endpoints used by this
                    //!< interface (excluding endpoint zero)
  unsigned char bInterfaceClass;      //!< Interface Class code
  unsigned char bInterfaceSubClass;   //!< Interface Sub-class
  unsigned char bInterfaceProtocol;   //!< Interface Protocol code

  unsigned char address;        ///< usb address
  unsigned char portnum;        ///< the port number attached (start from 1)
  unsigned char level;        ///< attach level, 0: roothub, 1:roothub->device, 2:roothub->hub->device, ...
  unsigned char speed;        ///< 1:low, 2:full, 3:high, 4:super
  unsigned char state;        ///< usb device state
} T_USBDevInfo;

// ---------------- Device Interface for Device Driver ------------------

//-----------------------------------------------------------------------------
// Device related struct
//-----------------------------------------------------------------------------
struct S_Driver;
struct S_Device;

struct S_CharOperations
{
    int (*open)(struct S_Device *dev);
    void (*close)(struct S_Device *dev);
    int (*read)(struct S_Device *dev, void *data, int length);
    int (*write)(struct S_Device *dev, const void *data, int length);
    int (*flush)(struct S_Device *dev);
};

// media information - for block device
struct media_info
{
    struct S_Device *dev;       //!< block device pointer
    char vendor[16];
    char dev_name[32];
    unsigned int format;        //!< bit0-7 file system format (value), bit8: fs case sensitive
    unsigned int version;
    unsigned int phy_sect_size; //!< physical sector size (set by fs)
    unsigned int total_sectors; //!< total no. of physical sectors (set by fs)
    unsigned int flags;
    unsigned int uid;           //! user id
    void *fs_data;              //!< pointer to file system's data
};

// case-sensitive bit in format field
#define MFS_CASE_SENSITIVE      0x100

// media info flags, bit 0-3 is the lun (logical unit number)
// bit0-15 updated by driver, bit16-31 updated by file system
#define MIF_LUN_MASK            0x0f    //!< for 0-15, max. 15 luns
#define MIF_AVAILABLE           0x10    //!< indicates media is available (for removable media device)
#define MIF_VALID               0x20    //!< indicates the media is valid mounted
#define MIF_MEDIA_REMOVABLE     0x40    //!< indicates if the media is removable, such as external CDROM
#define MIF_MTD_DEVICE          0x80    //!< indicates that it is a mtd device
#define MIF_MOUNTED             0x10000

#define media_get_lun(m)    ((m)->flags & MIF_LUN_MASK)

// block operation data - used for parameter of block read/write with oob
struct blk_op_data
{
    int datalen;                //!< data length
    int ret_datalen;            //!< return data length read/written
    int ooblen;                 //!< oob length to read/write
    int ret_ooblen;             //!< return oob length read/written
    unsigned char *data;        //!< data buffer, if null, no data are read/write
    unsigned char *oob;         //!< oob buffer, if null, no oob are read/write
};

struct blk_request
{
    struct S_Device *dev;
    unsigned char *buffer;      //!< buffer for read or write
    unsigned int buf_len;       //!< buffer length
    unsigned int sector;        //!< start sector address
    unsigned int nSector;       //!< no. of sector
    unsigned int timeout;       //!< request timeout, set 0 for driver default
    unsigned char flags;        //!< bit0-3: lun, bit4: direction
    unsigned char cmd_op;       //!< scsi command operation code
    unsigned short cmd_param;    //!< command parameter
    // below params are filled by device driver
    unsigned char cmd[16];      //!< scsi command
    unsigned char sense_buffer[18];
    unsigned char cmd_len;      //!< length of cmd
    unsigned char rfu;
    unsigned int transfered;    //!< no. of byte transfered
    int result;
};

// blk_request flags
#define BLK_LUN_MASK            0x0f
// blk_request flags direction bit
#define BLK_SYSTEM_TO_DEVICE    0
#define BLK_DEVICE_TO_SYSTEM    0x10

// operation code of scsi block commands and reduced block commands
// reference: SBC-3, SPC-4, RBC, MMC-6
#define SBC_INQUIRY             0x12
#define SBC_MODE_SELECT6        0x15
#define SBC_MODE_SENSE6         0x1a
#define SBC_MODE_SELECT10       0x55
#define SBC_MODE_SENSE10        0x5a
#define SBC_READ_CAPACITY       0x25
#define SBC_READ10              0x28
#define SBC_REQUEST_SENSE       0x03
#define SBC_START_STOP_UNIT     0x1b
#define SBC_SYNCHRONIZE CACHE10 0x35
#define SBC_TEST_UNIT_READY     0x00
#define SBC_VERIFY10            0x2f
#define SBC_WRITE10             0x2a
#define SBC_WRITE_BUFFER        0x3b
#define SBC_READ_FORMAT_CAP     0x23

// result code for blk_request
#define BREQ_GOOD               0
#define BREQ_CHECK_CONDITION    1
#define BREQ_ERROR              2

//! Standard INQUIRY data format. \see SPC-4 6.4.2
struct scsi_inquiry_data
{
    unsigned char peripheral;   //!< bit5-7 qualifier, bit0-4 device type
    unsigned char rmb;          //!< bit7 RMB
    unsigned char version;
    unsigned char data_format;  //!< bit5 NormACA, bit4 HiSUP, bit0-3 response data format
    unsigned char alength;      //!< additional length
    unsigned char flags1;   //!< SCCS ACC TPGS 3PC Reserved PROTECT
    unsigned char flags2;   //!< Obsolete ENCSERV VS MULTIP Obsolete Obsolete Obsolete ADDR16
    unsigned char flags3;   //!< Obsolete Obsolete WBUS16a SYNCa Obsolete Obsolete CMDQUE VS
    char T10_VID[8];        //!< T10 VENDOR IDENTIFICATION
    char PID[16];           //!< PRODUCT IDENTIFICATION
    char revision[4];       //!< PRODUCT REVISION LEVEL
    unsigned char vendor_specific[20];
    unsigned char clocking; //!< Reserved CLOCKINGa QASa IUSa
    unsigned char reserved1;
    unsigned short descriptor[8];   //!< vendor descriptor
    unsigned char reserved2[22];
};

//! general capacity descriptor. \see MMC-6 6.23.3.2
struct scsi_capacity_desc
{
    unsigned char numBlocks[4]; //!< MSB first
    unsigned char type;         //!< descriptor type
    unsigned char blkLength[3]; //!<  block length MSB first
};

//! format capacities data. \see MMC-6 6.23.3
struct scsi_format_capacities_data
{
    unsigned char reserved[3];
    unsigned char list_length;  //!< =no. of descriptors * size of descriptor
    struct scsi_capacity_desc descriptors[1];
};

//! Fixed format sense data. \see SPC-4 4.5.3
struct scsi_sense_data
{
    unsigned char response_code;
    unsigned char obsolete;
    unsigned char flags;    //!< FILEMARK EOM ILI Reserved SENSE KEY(bit0-3)
    unsigned char info[4];
    unsigned char alength;  //!< additional sense length
    unsigned char cmd_info[4];
    unsigned char asense_code;      //!< additional sense code
    unsigned char asc_qualifier;    //!< additional sense code qualifier
    unsigned char field_code;       //!< FIELD REPLACEABLE UNIT CODE
    unsigned char key_specific[3];  //!< SENSE KEY SPECIFIC
};

struct S_BlkOperations
{
    int (*open)(struct S_Device *dev, int lun);
    void (*close)(struct S_Device *dev);
    int (*request)(struct blk_request *req);
    int (*media_changed)(struct media_info *); // check if media changed
    int (*revalidate)(struct media_info *); // revalidate media, if media changed

    // for mtd driver only
    int (*read_with_oob)(struct S_Device *dev, unsigned int sect, struct blk_op_data *);
    int (*write_with_oob)(struct S_Device *dev, unsigned int sect, struct blk_op_data *);
    int (*erase)(struct S_Device *dev, int blk_no);
    int (*is_badblock)(struct S_Device *dev, int blk_no);
    int (*mark_bad)(struct S_Device *dev, int blk_no);
    int (*write_protect)(struct S_Device *dev, int wp_on);
};

#define K_EpALen          6           /* hardware address length*/
/*  (physical) net address  */
typedef struct  {                     /* a hardware address */
  BYTE  s_ha_addr[K_EpALen];            /* the address */
} T_HWA;

/* Definitions of network interface structure (one per interface) */
typedef struct {              /* info about one net interface */
  DWORD  d_ni_ip;            /* IP address for this interface */
  DWORD d_ni_net;           /* network IP address */
  DWORD d_ni_subnet;        /* subnetwork IP address */
  DWORD d_ni_mask;          /* IP subnet mask for interface */
  DWORD d_ni_brc;           /* IP broadcast address */
  DWORD d_ni_nbrc;          /* IP net broadcast address */
  DWORD  d_ni_gateway;       /* IP of gateway */
  DWORD  s_ni_dns[2]; /* IP of domain server */
  T_HWA   s_ni_hwa;           /* hardware address of interface */
  T_HWA   s_ni_hwb;           /* hardware broadcast address */
  BOOLEAN b_ni_ivalid;        /* is ni_ip valid? */
  BOOLEAN b_ni_svalid;        /* is ni_subnet valid? */
  BYTE    b_ni_ipinq;         /* IP input queue */
  BYTE    b_ni_outq;          /* (device) output queue */
  struct S_Device *dev;   // link to device (this value set by driver)
  unsigned int trans_start;   // freerun time for starting transfer
  WORD    mtu;          // maximum transfer unit
  BYTE  hard_header_len;  // hardware header length
  BYTE    state;          // empty, installed or enabled
  BYTE    dhcp;           // 1:dhcp enabled, 0: dhcp disabled
  BYTE  type;     // hardware device type (set by driver)
  BYTE  rfu[2];
} T_NETIF;

/// nif state
enum {NIF_STATE_EMPTY, NIF_STATE_ALLOCATED, NIF_STATE_INSTALLED, NIF_STATE_ENABLED};
/// nif type
enum {
  NIF_TYPE_LOCAL,
  NIF_TYPE_ETHERNET,
  NIF_TYPE_PPP,
  NIF_TYPE_MOBILE,
  NIF_TYPE_WIFI,
  NIF_TYPE_BLUETOOTH,
  NIF_TYPE_WIMAX
};

#define netif_running(nif)  ((nif)->state == NIF_STATE_ENABLED)

struct S_NetOperations
{
    int (*open)(T_NETIF *nif);
    void (*close)(T_NETIF *nif);
    int (*tx_timeout)(T_NETIF *nif);
    int (*set_mac_address)(T_NETIF *nif, void *addr);
    int (*change_mtu)(T_NETIF *nif, int new_mtu);
    int (*get_link)(T_NETIF *nif);
};

struct S_Driver
{
    const char *name;
    unsigned int version;
    unsigned int attr;
    struct S_Bus *bus;
    struct S_Driver *next;
    int (*probe)(struct S_Device *dev);
    int (*remove)(struct S_Device *dev);
    int (*shutdown)(struct S_Device *dev);
    int (*suspend)(struct S_Device *dev);
    int (*resume)(struct S_Device *dev);
    int (*show_attr)(struct S_Device *dev, char *buf, int size);
    int (*release)(struct S_Device *dev);
    int (*ioctl)(struct S_Device *dev, unsigned int cmd, void *param);
    void *operations;
};

/**
 * driver attribute, bit0-5 are common attributes,
 * bit6,7 are device type
 * bit8-15 are device code (same as dev_id bit8-15 in device struct)
 * bit16-23 reserved
 * bit24-31 are driver specific attributes
 */
#define DA_DEVICE_TYPE_MASK     0xc0
#define DA_CHAR_DRIVER          0x00
#define DA_BLOCK_DRIVER         0x40
#define DA_NET_DRIVER           0x80
#define DA_DEVICE_CODE_MASK     0xff00

#define __get_driver_type(drv)  ((drv)->attr & DA_DEVICE_TYPE_MASK)
#define __get_device_type(dev)  ((dev)->dev_id & DA_DEVICE_TYPE_MASK)

struct S_DeviceType
{
    const char *name;
    int (*suspend)(struct S_Device *dev);
    int (*resume)(struct S_Device *dev);
    void (*release)(struct S_Device *dev);
};

struct S_Device
{
    const char *name;
    // device id, bit0-5: dev num, bit6,7: dev type, bit8-15: dev code, bit16-31: reserved
    int dev_id;
    // refence count, increase when referenced, decrease when de-referenced
    int ref_count;
    // counter for application usage. increase when app open, decrease when close
    int use_count;
    // device type
    struct S_DeviceType *type;
    // parent device, or null
    struct S_Device *parent;
    // bus pointer
    struct S_Bus *bus;
    // the driver to drive this device
    struct S_Driver *driver;
    // device's data
    void *dev_data;
    // block device = no. of device info entry, net device = nif number
    int nInfo;
    // device's info array (store media_info for block device, store nif for net driver)
    void *dev_info;
    // next device
    void *next;
    // release function, called by system when device release
    void (*release)(struct S_Device *dev);
};

//-----------------------------------------------------------------------------
// Device driver functions
//-----------------------------------------------------------------------------
// int os_driver_register(struct S_Driver *drv, int bus_id);
#define os_driver_register(x,y)   (int)sys_func2(OS_FN_DRIVER_REGISTER, (DWORD)x, (DWORD)y)

// int os_driver_unregister(struct S_Driver *drv);
#define os_driver_unregister(x)   (int)sys_func1(OS_FN_DRIVER_UNREGISTER, (DWORD)x)

// struct S_Device *os_device_alloc(unsigned long size);
#define os_device_alloc(x)    (struct S_Device *)sys_func1(OS_FN_DEVICE_ALLOC, (DWORD)x)

// int os_device_register(struct S_Device *dev, int bus_id);
#define os_device_register(x,y)   (int)sys_func2(OS_FN_DEVICE_REGISTER, (DWORD)x, (DWORD)y)

// int os_device_unregister(struct S_Device *dev);
#define os_device_unregister(x)   (int)sys_func1(OS_FN_DEVICE_UNREGISTER, (DWORD)x)

// int os_interrupt_register(int intr_id, int (*intr)());
#define os_interrupt_register(x,y)  (int)sys_func2(OS_FN_INTERRUPT_REGISTER, (DWORD)x, (DWORD)y)

// int os_interrupt_unregister(int intr_id, int (*intr)());
#define os_interrupt_unregister(x,y)  (int)sys_func2(OS_FN_INTERRUPT_UNREGISTER, (DWORD)x, (DWORD)y)

// int os_device_alloc_media(struct S_Device *dev, int nMedia);
#define os_device_alloc_media(x,y)  (int)sys_func2(OS_FN_DEVICE_ALLOC_MEDIA, (DWORD)x, (DWORD)y)

// ----------- Device Interface for Application -------------

//------------------------------------------------------------------------------
// ioctl commands
//------------------------------------------------------------------------------
/**
 * ioctl command format
 * bit31-24: device code, bit23-16: command code, bit15-14: direction, bit13-0: param size
 * if device code == 0, it is a common device command, and will be handled in devdrv.c
 */
#define IOCMD_DIR_READ          0x8000
#define IOCMD_DIR_WRITE         0x4000

/*!
 * get device info
 * param = buffer
 * return error code
 * output: param = T_DeviceInfo
 */
#define IOCMD_GET_DEVINFO   ((1<<16) | IOCMD_DIR_READ | sizeof(T_DeviceInfo))

/*!
 * suspend device
 * param = 0
 * return error code
 */
#define IOCMD_DEVICE_SUSPEND   ((2<<16) | 0 | 0)

/*!
 * resume device
 * param = 0
 * return error code
 */
#define IOCMD_DEVICE_RESUME   ((3<<16) | 0 | 0)

/*!
 * get block device's total number of logical unit
 * param = 0
 * return number of logical unit or error code.
 */
#define IOCMD_BLK_TOTAL_LUN ((4<<16) | IOCMD_DIR_READ | 0)

/*!
 * get media's capacity and sector size
 * param = unsigned int [3], where param[0] = logical unit number (lun)
 * return error code.
 * output: param[1] = total no. of block in device, param[2] = block size in byte
 */
#define IOCMD_MEDIA_CAPACITY    ((5<<16) | IOCMD_DIR_READ | 12)

/*!
 * read a block from device
 * param = block address (4 bytes, little endian) + 3 bytes reserved + 1 byte lun + buffer[block size]
 * return number of bytes read or error code
 */
#define IOCMD_MEDIA_BLK_READ    ((6<<16) | IOCMD_DIR_READ | 512 + 8)

/*!
 * write a block to device
 * param = block address (4 bytes, little endian) + 3 bytes reserved + 1 byte lun + buffer[block size]
 * return number of bytes written or error code
 */
#define IOCMD_MEDIA_BLK_WRITE  ((7<<16) | IOCMD_DIR_WRITE | 512 + 8)

/*!
 * get net device's nif (network interface) number
 * param = 0
 * return nif number.
 */
#define IOCMD_GET_NIF_NUM ((8<<16) | IOCMD_DIR_READ | 0)

/*!
 * get comm port status
 * param = 0
 * return status or error code
 */
#define IOCMD_COMM_STATUS   (((DEV_COMM_PORT & 0xff00)<<16) | (1<<16) | IOCMD_DIR_READ)

// usbh serial state
#define K_SERIAL_RXRDY          1
#define K_SERIAL_TXRDY          2
#define K_SERIAL_DISABLED       0x80

/*!
 * get comm line state
 * param = 0
 * return line state or error code
 */
#define IOCMD_COMM_LINE_GETSTATE  (((DEV_COMM_PORT & 0xff00)<<16) | (2<<16) | IOCMD_DIR_READ)

/*!
 * set comm line state
 * param = (int)(dtr | rts)
 * return error code
 */
#define IOCMD_COMM_LINE_SETSTATE  (((DEV_COMM_PORT & 0xff00)<<16) | (3<<16) | IOCMD_DIR_WRITE)

/*!
 * clear comm line state (dtr or rts)
 * param = (int)(dtr | rts)
 * return error code
 */
#define IOCMD_COMM_LINE_CLRSTATE  (((DEV_COMM_PORT & 0xff00)<<16) | (4<<16) | IOCMD_DIR_WRITE)

// line state bit map
#define K_LINE_STATE_DCD    1
#define K_LINE_STATE_DSR    2
#define K_LINE_STATE_RI     8
#define K_LINE_STATE_CTS    0x100
#define K_LINE_STATE_DTR    0x200
#define K_LINE_STATE_RTS    0x400

/*!
 * Set modem power state
 * param = (int) 1:on, 0:off
 * return error code
 */
#define IOCMD_COMM_SETPOWER   (((DEV_COMM_PORT & 0xff00)<<16) | (5<<16) | IOCMD_DIR_WRITE)

/*!
 * Reset modem
 * param = 0
 * return error code
 */
#define IOCMD_COMM_RESET    (((DEV_COMM_PORT & 0xff00)<<16) | (6<<16) | IOCMD_DIR_WRITE)

/*!
 * Set NMEA (GPS) mode
 * param = (int) 1: enable, 0: disable
 * return error code
 */
#define IOCMD_COMM_SET_NMEA   (((DEV_COMM_PORT & 0xff00)<<16) | (7<<16) | IOCMD_DIR_WRITE)

// device id (bit8-15:dev code, bit6-7:dev type)
// char device (dev code from 0x01-0x7f inclusive)
#define DEV_GENERIC_USB 0x0100
#define DEV_COMM_PORT   0x0200
#define DEV_BUZZER      0x0300
#define DEV_SPEAKER     0x0400
#define DEV_KEYBOARD    0x0500
#define DEV_THERMAL_PRN 0x0600
#define DEV_IMPACT_PRN  0x0700
#define DEV_USBHUB      0x0800
#define DEV_MODEM       0x0900
#define DEV_ICCARD      0x0a00
#define DEV_SAM         0x0b00
#define DEV_CONTACTLESS 0x0c00
#define DEV_MSR         0x0d00
#define DEV_BATTERY     0x0e00
// block device (dev code from 0x80-0xbf inclusive)
#define DEV_USB_STORAGE 0x8040
#define DEV_SD_CARD     0x8140
// network device (dev code from 0xc0-0xff inclusive)
#define DEV_ETHERNET    0xc080
#define DEV_PPP         0xc180

/*!
 * set comm port config
 * param = buffer for comm port config
 * return status or error code
 */
#define IOCMD_COMM_SET_CONFIG (((DEV_COMM_PORT & 0xff00)<<16) | (3<<16) | IOCMD_DIR_WRITE | 512)

/////////////////////////////// net io command /////////////////////

/*!
 * get device firmware version
 * param = 32-byte buffer
 * return error code
 */
#define IOCMD_NET_GET_FW_VER  (((DEV_ETHERNET & 0xff00)<<16) | (1<<16) | IOCMD_DIR_READ | 32)

/*!
 * scan AP
 * param = 0 : get scanning status, 1 : start scan
 * return scan status or error code
 */
#define IOCMD_NET_SCAN_AP (((DEV_ETHERNET & 0xff00)<<16) | (2<<16))
  // scan status
  enum {
    WIFI_SCAN_IDLE,
    WIFI_SCANNING,
    WIFI_SCAN_COMPLETE
  };

/*!
 * get result of AP scanning
 * param = T_WiFi_AP_Info[MAX_SCANNED_WIFI_AP]
 * return number of scanned AP or error code
 */
#define IOCMD_NET_SCAN_RESULT (((DEV_ETHERNET & 0xff00)<<16) | (3<<16) | IOCMD_DIR_READ | MAX_SCANNED_WIFI_AP)

#define MAX_SCANNED_WIFI_AP 32
#define MAX_WIFI_MTU  1446

/*!
 * connect AP profile
 * param = T_WiFi_AP_Profile
 * return error code
 */
#define IOCMD_NET_CONNECT_AP  (((DEV_ETHERNET & 0xff00)<<16) | (4<<16))

/*!
 * disconnect from AP
 * param = 0
 * return error code
 */
#define IOCMD_NET_DISCONNECT_AP (((DEV_ETHERNET & 0xff00)<<16) | (5<<16))

/*!
 * get connection status
 * param = T_WiFi_AP_Profile pointer (for output)
 * return AP connection status or error code
 */
#define IOCMD_NET_AP_CONNECTION_STATUS  (((DEV_ETHERNET & 0xff00)<<16) | (6<<16) | IOCMD_DIR_READ)

  // WiFi AP connection status
  enum {
    NET_AP_DISCONNECTED,
    NET_AP_CONNECTED,
    NET_AP_WRONG_PASS,
    NET_AP_TIMEOUT,
    NET_AP_CONNECTING,
    NET_AP_WRONG_SSID,
    NET_AP_WRONG_BSSID,
    NET_AP_PROFILE_NOT_FOUND,
    NET_AP_INVALID_KEY_LEN,
    NET_AP_INVALID_CONFIG,
  };

/*!
 * P2P listen
 * param = 1: enable, 0: disable
 * return error code
 */
#define IOCMD_NET_P2P_LISTEN  (((DEV_ETHERNET & 0xff00)<<16) | (7<<16))

/*!
 * get signal strength of connected AP
 * param = signed char *rssi (output in unit of dBm)
 * return error code (return -ENOTCONN if not connected)
 */
#define IOCMD_NET_GET_RSSI  (((DEV_ETHERNET & 0xff00)<<16) | (8<<16) | IOCMD_DIR_READ | 1)

/*!
 * Set auto connection to AP. (Powerup default value is 0)
 * param = retry period in seconds (set 0 to disable auto connect)
 * return error code
 */
#define IOCMD_NET_AUTO_CONN_AP  (((DEV_ETHERNET & 0xff00)<<16) | (9<<16))

// define bus id
enum
{
    BUS_ID_SYSTEM,
    BUS_ID_USB,
    MAX_BUSES
};

                /****** Structure for WiFi ******/

/** Wifi AP information when return by scan WiFi function*/
typedef struct WiFi_AP_Info
{
  signed char rssi;   ///< receive signal strength indicator
  unsigned char snr;    ///< signal to noise ratio
  unsigned char channel;  ///< channel number
  unsigned char types;    ///< connection type, encryption type
  char ssid[33];    ///< Service set ID, null terminate string
  unsigned char bssid[6]; ///< Basic Service set ID
  unsigned char rfu;    ///< for alignment
} T_WiFi_AP_Info;

/** define types: bit0-3 encryption type, bit4-7 mode */
#define WIFI_TYPE_ENC_OPEN  0
#define WIFI_TYPE_ENC_WEP 1
#define WIFI_TYPE_ENC_WPA_WPA2  2
#define WIFI_TYPE_ENC_WPS_PBC 3
#define WIFI_TYPE_ENC_WPS_PIN 4
#define WIFI_TYPE_MODE_INFRA  0x00
#define WIFI_TYPE_MODE_ADHOC  0x10

/** WiFi AP profile structure */
typedef struct WiFi_AP_Profile
{
  char ssid[33];    ///< ssid, null terminate string
  unsigned char key_index;  ///< for WEP only
  unsigned char enc_type; ///< encryption type (0-4 defined above)
  unsigned char rfu;
  char pass[64];    ///< password, null terminate string
} T_WiFi_AP_Profile;

                /****** DeviceInfo Structure ******/

typedef struct S_DeviceInfo
{
    char name[64];
    int ref_count;
    unsigned int dev_id;
    unsigned int bus_id;
    char bus_name[64];
    char drv_name[64];
    unsigned int drv_version;
    unsigned int drv_attr;
    char dev_data[1024];  //!< string in format <item1>:<value1>\n<item2>:<value2>\n...
} T_DeviceInfo;

typedef struct S_DevIterator
{
    unsigned int filter;    //!< bit31-24:bus id, bit15-6:device
    unsigned int filter_mask;
    int bus_id;
    int dev_index;
} T_DevIterator;

// filter mask defines
#define DI_FILTER_BUS   0xff000000
#define DI_FILTER_DEV   0xffc0

// int os_bus_start(int bus_id);
#define os_bus_start(x)     (int)sys_func1(OS_FN_BUS_START, (DWORD)x)

// int os_bus_stop(int bus_id);
#define os_bus_stop(x)      (int)sys_func1(OS_FN_BUS_STOP, (DWORD)x)

// int os_device_ioctl(int dev_id, unsigned int cmd, void *param)
#define os_device_ioctl(x,y,z)    (int)sys_func3(OS_FN_DEVICE_IOCTL, (DWORD)x, (DWORD)y, (DWORD)z)

// void os_device_search_init(T_DevIterator *dit, unsigned int filter, unsigned int mask)
#define os_device_search_init(x,y,z)  sys_func3(OS_FN_DEVICE_SEARCH_INIT, (DWORD)x, (DWORD)y, (DWORD)z)

// int os_device_search_next(T_DevIterator *dit, T_DeviceInfo *dev_info)
#define os_device_search_next(x,y)  (int)sys_func2(OS_FN_DEVICE_SEARCH_NEXT, (DWORD)x, (DWORD)y)

// int os_device_open(int dev_id, unsigned int mode)
#define os_device_open(x,y)   (int)sys_func2(OS_FN_DEVICE_OPEN, (DWORD)x,(DWORD)y)

// int os_device_mount(int dev_id, int lun, const char *mount_point)
#define os_device_mount(x,y,z)    (int)sys_func3(OS_FN_DEVICE_MOUNT, (DWORD)x, (DWORD)y, (DWORD)z)

// int os_device_unmount(const char *mount_point, int force)
#define os_device_unmount(x,y)    (int)sys_func2(OS_FN_DEVICE_UNMOUNT, (DWORD)x, (DWORD)y)

// int os_file_ioctl(DWORD a_fid, unsigned int cmd, void *param)
#define os_file_ioctl(x,y,z)    (int)sys_func3(OS_FN_FILE_IOCTL, (DWORD)x, (DWORD)y, (DWORD)z)

// network device API (system function, should be used by base only)
// int os_netdev_open(T_NETIF *nif, int dev_id)
#define os_netdev_open(x,y)     (int)sys_func2(OS_FN_NETDEV_OPEN, (DWORD)(x), (DWORD)(y))

// void os_netdev_close(T_NETIF *nif)
#define os_netdev_close(x)        (void)sys_func1(OS_FN_NETDEV_CLOSE, (DWORD)(x))

// int os_netdev_getlink(T_NETIF *nif)
#define os_netdev_getlink(x)      (int)sys_func1(OS_FN_NETDEV_GETLINK, (DWORD)(x))

//int os_netdev_ioctl(T_NETIF *nif, unsigned int cmd, void *data)
#define os_netdev_ioctl(x,y,z)  (int)sys_func3(OS_FN_NETDEV_IOCTL, (DWORD)x, (DWORD)y, (DWORD)z)

// int os_netdev_event(T_NetEvent*) // get event from netdev, or send event to netdev
#define os_netdev_event(x)      (int)sys_func1(OS_FN_NETDEV_EVENT, (DWORD)x)

typedef struct S_NetEvent {
  unsigned char event;  // bit-7: direction (0 - netdev to nif, 1 - nif to netdev)
  unsigned char rfu;
  unsigned short param;
  unsigned int value;
} T_NetEvent;

/// define T_NetEvent.event

/// get event from netdev
#define NEV_GET_EVENT 0

/// attach event: param = nif number, value = device id
#define NEV_ATTACH    1

/// detach event: param = nif number, value = device id
#define NEV_DETACH    2

/// send timeout event to netdev: param = nif number, value = nif pointer
#define NEV_TX_TIMEOUT  0x81

/// usb otg API

/// usb otg attached device, return from os_usbotg_get_state()
#define K_USBOTG_ATTACHED_NONE    0
#define K_USBOTG_ATTACHED_DEV_A   1
#define K_USBOTG_ATTACHED_DEV_B   2

// DWORD os_usbotg_get_state()
#define os_usbotg_get_state()    sys_func0(OS_FN_USBOTG_GET_STATE)

// DWORD os_get_mclk()
#define os_get_mclk()     sys_func0(OS_FN_GET_MCLK)

                /****** Usb Keyboard Functions ******/

//BOOLEAN usbkbd_open(void);
#define os_usbkbd_open()                   sys_func0(OS_FN_USBKBD_OPEN)

//void usbkbd_close(void);
#define os_usbkbd_close()                  sys_func0(OS_FN_USBKBD_CLOSE)

//BYTE usbkbd_inkey(void);
#define os_usbkbd_inkey()                  sys_func0(OS_FN_USBKBD_INKEY)

//int usbkbd_getkey(void);
#define os_usbkbd_getkey()                 sys_func0(OS_FN_USBKBD_GETKEY)

                /****** PCI-DSS Support Functions ******/

//BOOLEAN os_pic_dss_get_kvc(BYTE *a_kvc);
#define os_pci_dss_get_kvc(x)              sys_func1(OS_FN_PCI_DSS_GET_KVC,(DWORD)(x))

//BOOLEAN os_pci_dss_regen_key(BYTE *a_new_kvc);
#define os_pci_dss_regen_key(x)            sys_func1(OS_FN_PCI_DSS_REGEN_KEY,(DWORD)(x))

// a_mode_len - bit31 = 0 => encrypt
//                      1 => decrypt
//BOOLEAN os_pic_dss(BYTE *a_IV,BYTE *a_data,DWORD a_mode_len); // a_len must be multiple of 8
#define os_pci_dss(x,y,z)                  sys_func3(OS_FN_PCI_DSS,(DWORD)(x),(DWORD)(y),(DWORD)(z))

                /****** Mif Interface Functions ******/

typedef struct
{
  BYTE  b_len;
  BYTE  b_type;
  WORD  w_poll_time;
  BYTE  b_tx_power;
  BYTE  b_mod_power;
  BYTE  b_rx_gain;
  BYTE  b_rx_threshold;
}T_MIF_CMD;

// os_mif_card_type()
// bit-7 = type B
enum mif_card_list
{
  K_MIF_CARD_NONE = 0,
  K_MIF_CARD_MF1L10,
  K_MIF_CARD_MF1S50,
  K_MIF_CARD_MF1P60,
  K_MIF_CARD_TYPEB0 = 0x80,
  K_MIF_CARD_TYPEB1 = 0x81,
  K_MIF_CARD_UNKNOWN = 0xff
};

// os_mif_status()
enum mif_status_list
{
  K_MIF_STATUS_OK = 0,
  K_MIF_STATUS_CRC_ERR,
  K_MIF_STATUS_FRAME_ERR,
  K_MIF_STATUS_PARITY_ERR,
  K_MIF_STATUS_COLL_ERR,
  K_MIF_STATUS_CODE_ERR,
  K_MIF_STATUS_TRAN_ERR,
  K_MIF_STATUS_SNO_ERR,
  K_MIF_STATUS_TYPE_ERR,
  K_MIF_STATUS_AUTH_ERR,
  K_MIF_STATUS_NAUTH_ERR,
  K_MIF_STATUS_WRITE_ERR,
  K_MIF_STATUS_TRANS_ERR,
  K_MIF_STATUS_TIMEOUT,
  K_MIF_STATUS_IDLE = 0xff
};

// os_mif_info_get() Index
enum mif_info_list
{
  K_MIF_INFO_AS3911_DEFAULT = 0,
  K_MIF_INFO_AS3911_NUM,
};

// DWORD os_mif_open(BYTE *ap_init_str);
#define os_mif_open(x)      sys_func1(OS_FN_MIF_OPEN,(DWORD)(x))

// DWORD os_mif_close(void);
#define os_mif_close()      sys_func0(OS_FN_MIF_CLOSE)

// DWORD os_mif_cancel(void);
#define os_mif_cancel()     sys_func0(OS_FN_MIF_CANCEL)

// DWORD os_mif_halt(BYTE *ap_pupi);
#define os_mif_halt(x)      sys_func1(OS_FN_MIF_HALT,(DWORD)(x))

// DWORD os_mif_card_type(void);
#define os_mif_card_type()  sys_func0(OS_FN_MIF_CARD_TYPE)

// DWORD os_mif_tag(void);
#define os_mif_tag()        sys_func0(OS_FN_MIF_TAG)

// DWORD os_mif_status(void);
#define os_mif_status()     sys_func0(OS_FN_MIF_STATUS)

// DWORD os_mif_anticoll(void);
#define os_mif_anticoll()   sys_func0(OS_FN_MIF_ANTICOLL)

// DWORD os_mif_select(BYTE *ap_id);
#define os_mif_select(x)    sys_func1(OS_FN_MIF_SELECT,(DWORD)(x))

// DWORD os_mif_load_key(BYTE *ap_ikey);
#define os_mif_load_key(x)  sys_func1(OS_FN_MIF_LOAD_KEY,(DWORD)(x))

// DWORD os_mif_auth1(DWORD ad_key_type,DWORD ad_block,BYTE *ap_sno);
#define os_mif_auth1(x,y,z) sys_func3(OS_FN_MIF_AUTH1,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD os_mif_auth2(void);
#define os_mif_auth2()      sys_func0(OS_FN_MIF_AUTH2)

// DWORD os_mif_read(DWORD ad_block);
#define os_mif_read(x)      sys_func1(OS_FN_MIF_READ,(DWORD)(x))

// DWORD os_mif_write(DWORD ad_block,BYTE *ap_src);
#define os_mif_write(x,y)   sys_func2(OS_FN_MIF_WRITE,(DWORD)(x),(DWORD)(y))

// DWORD os_mif_inc(DWORD ad_block,BYTE *ap_value);
#define os_mif_inc(x,y)     sys_func2(OS_FN_MIF_INC,(DWORD)(x),(DWORD)(y))

// DWORD os_mif_dec(DWORD ad_block,BYTE *ap_value);
#define os_mif_dec(x,y)     sys_func2(OS_FN_MIF_DEC,(DWORD)(x),(DWORD)(y))

// DWORD os_mif_restore(DWORD ad_block);
#define os_mif_restore(x)   sys_func1(OS_FN_MIF_RESTORE,(DWORD)(x))

// DWORD os_mif_transfer(DWORD ad_block);
#define os_mif_transfer(x)  sys_func1(OS_FN_MIF_TRANSFER,(DWORD)(x))

// DWORD os_mif_dec_transfer(DWORD ad_block,BYTE *ap_value);
#define os_mif_dec_transfer(x,y) sys_func2(OS_FN_MIF_DEC_TRANS,(DWORD)(x),(DWORD)(y))

// DWORD os_mif_sno(void);
#define os_mif_sno()        sys_func0(OS_FN_MIF_SNO)

// DWORD os_mif_sendcommand(BYTE *ap_src, DWORD ad_len);
#define os_mif_sendcommand(x,y) sys_func2(OS_FN_MIF_SENDCMD,(DWORD)(x),(DWORD)(y))

// DWORD os_mif_get(BYTE *ap_dest, DWORD ad_len);
#define os_mif_get(x,y)         sys_func2(OS_FN_MIF_GET,(DWORD)(x),(DWORD)(y))

// void os_mif_info_set(DWORD ad_idx, BYTE *ap_src);
#define os_mif_info_set(x,y)    sys_func2(OS_FN_MIF_INFO_SET,(DWORD)(x),(DWORD)(y))

// void os_mif_info_get(DWORD ad_idx, BYTE *ap_dest);
#define os_mif_info_get(x,y)    sys_func2(OS_FN_MIF_INFO_GET,(DWORD)(x),(DWORD)(y))

                /****** Touch Pad Functions ******/

// BOOLEAN tpad_open(void) system only
#define os_tpad_open()          sys_func0(OS_FN_TPAD_OPEN)

// BOOLEAN tpad_close(void); system only
#define os_tpad_close()         sys_func0(OS_FN_TPAD_CLOSE)

// DWORD tpad_add_obj(T_TPAD_OBJ *a_obj);
#define os_tpad_add_obj(x)      sys_func1(OS_FN_TPAD_ADD_OBJ,(DWORD)(x))

// BOOLEAN tpad_del_obj(DWORD a_obj_id);
#define os_tpad_del_obj(x)      sys_func1(OS_FN_TPAD_DEL_OBJ,(DWORD)(x))

#define K_TpadNewData           0x00008000
#define K_TpadTouched           0x80000000

#define K_TpadRead          0
#define K_TpadPeek          1

// DWORD tpad_cpos(void);  bit 31 = 1 = touched, bit15 = new data, bit30-16 = hpos bit14-0 = vpos
#define os_tpad_cpos()          sys_func0(OS_FN_TPAD_CPOS)
#define os_tpad_peek_cpos()     sys_func1(OS_FN_TPAD_CPOS,K_TpadPeek) // same as os_tpad_cpos but without clear the new data flag

// void tpad_alert_return(void);
#define os_tpad_alert_return()  sys_func0(OS_FN_TPAD_ALERT_RETURN)

// BOOLEAN tpad_raw_pos(DWORD * a_hpos,DWORD *a_vpos) system only
#define os_tpad_raw_pos(x,y)    sys_func2(OS_FN_TPAD_RAW_POS,(DWORD)(x),(DWORD)(y))

// void tpad_set_calibrate(T_CAL_POINT *a_pt,T_CAL_POINT *aoffsest);  // system only
#define os_tpad_set_calibrate(x,y) sys_func2(OS_FN_TPAD_CALIBRATE,(DWORD)(x),(DWORD)(y))

                /****** Barcode Scanner Functions ******/
enum  {
  K_BarcodeNotReady,
  K_BarcodeOpen,
  K_BarcodeScanning,
  K_BarcodeDataReady,
  K_BarcodeTimeout,
  K_BarcodeAbort
};
// DWORD os_barcode_open(BYTE* a_init);
#define os_barcode_open(x)      sys_func1(OS_FN_BARCODE_OPEN, (DWORD)(x))

// void os_barcode_close(void);
#define os_barcode_close()      sys_func0(OS_FN_BARCODE_CLOSE)

// DWORD os_barcode_cmd(const char* a_cmd, DWORD a_len);
#define os_barcode_cmd(x,y)     sys_func2(OS_FN_BARCODE_CMD, (DWORD)(x), (DWORD)(y))

// DWORD os_barcode_scan(DWORD a_timeout);
#define os_barcode_scan(x)      sys_func1(OS_FN_BARCODE_SCAN, (DWORD)(x))

// DWORD os_barcode_read(char* buf);
#define os_barcode_read(x)      sys_func1(OS_FN_BARCODE_READ, (DWORD)(x))

// DWORD os_barcode_abort(void);
#define os_barcode_abort()      sys_func0(OS_FN_BARCODE_ABORT)

// DWORD os_barcode_status(void);
#define os_barcode_status()     sys_func0(OS_FN_BARCODE_STATUS)

// BOOLEAN os_barcode_enabled(void);
#define os_barcode_enabled()    (BOOLEAN)sys_func0(OS_FN_BARCODE_ENABLED)

typedef struct
{
  char *a;
  char *b;
  char *p;
  char *n;
  char *gx;
  char *gy;
} T_SM2_PARAMETER;

typedef struct
{
  char *m;
  int len;
  char *email;
  char *key;
  char *r;
  char *s;
} T_SM2_SIGN;

typedef struct
{
  char *mess;
  int len;
  char *email;
  char *keyx;
  char *keyy;
  char *mess_out;
  int len_mess_out;
} T_SM2_GET_MESS;

typedef struct
{
  char *m;
  int len;
  char *r;
  char *s;
  char *keyx;
  char *keyy;
  //char *rr;
} T_SM2_SIGN_VERIFY;

typedef struct
{
  char *mess;
  int len;
  char *keyx;
  char *keyy;
  char *c;
  int c_len;
} T_SM2_ENCRYPT;

typedef struct
{
  char *key;
  char *c;
  int c_len;
  char *m;
  int len;
} T_SM2_DECRYPT;

typedef struct
{
  DWORD s_sk[32];       /*!<  SM4 subkeys       */
} T_SM4_CONTEXT;


//void sm2_set_curve(T_SM2_PARAMETER *x)
#define os_sm2_set_curve(x)         sys_func1(OS_FN_SM2_SET_CURVE,(DWORD)x)
//void sm2_free_curve()
#define os_sm2_free_curve()         sys_func0(OS_FN_SM2_FREE_CURVE)
//void sm2_get_signature(T_SM2_SIGN *a_x,char *a_messOut,int *a_len)
#define os_sm2_get_signature(x,y,z) sys_func3(OS_FN_SM2_GET_SIGNATURE,(DWORD)x,(DWORD)y,(DWORD)z)
//int sm2_verify_signature(T_SM2_SIGN_VERIFY *x)
#define os_sm2_verify_signature(x)  sys_func1(OS_FN_SM2_VERIFY_SIGNATURE,(DWORD)x)
//int sm2_encrypt(T_SM2_ENCRYPT *x)
#define os_sm2_encrypt(x)           sys_func1(OS_FN_SM2_ENCRYPT,(DWORD)x)
//int sm2_decrypt(T_SM2_DECRYPT *x)
#define os_sm2_decrypt(x)           sys_func1(OS_FN_SM2_DECRYPT,(DWORD)x)
//int sm3_ascii(char *a_hash,char *a_mess)
#define os_sm3_ascii(x,y)           sys_func2(OS_FN_SM3_ASCII,(DWORD)x,(DWORD)y)
//int sm3(char *a_hash,char *a_mess,int len)
#define os_sm3(x,y,z)               sys_func3(OS_FN_SM3,(DWORD)x,(DWORD)y,(DWORD)z)
#define os_sm3_run_bin              os_sm3
#define os_sm3_run                  os_sm3_ascii
//void sm2_get_mess(T_SM2_GET_MESS *a_x)
#define os_sm2_get_mess(x)          sys_func1(OS_FN_SM2_GET_MESS,(DWORD)x)
//void os_sm4_set_key(T_SM4_CONTEXT *a_ctx,BYTE *a_key);
#define os_sm4_set_Key(x,y)         sys_func2(OS_FN_SM4_SET_KEY,(DWORD)x,(DWORD)y)
//void os_sm4_encrypt(T_SM4_CONTEXT a_ctx,BYTE * a_in, BYTE * a_out);
#define os_sm4_encrypt(x,y,z)       sys_func3(OS_FN_SM4_ENCRYPT,(DWORD)x,(DWORD)y,(DWORD)z)
//void os_sm4_decrypt(T_SM4_CONTEXT a_ctx,BYTE * a_in, BYTE * a_out);
#define os_sm4_decrypt(x,y,z)       sys_func3(OS_FN_SM4_DECRYPT,(DWORD)x,(DWORD)y,(DWORD)z)

// wifi control code
enum {
  WIFI_CTRL_TEST_DISABLE,
  WIFI_CTRL_TEST_ENABLE,
  WIFI_CTRL_SET_MAC,
  WIFI_CTRL_FW_DOWNLOAD,
  WIFI_CTRL_GET_REGULATORY_DOMAIN,
  WIFI_CTRL_SET_REGULATORY_DOMAIN,
  WIFI_CTRL_CORE_INFO = 100
};

// wifi regulatory domain code
enum {
  WIFI_REGULATORY_DOMAIN_NONE = 0,
  WIFI_REGULATORY_DOMAIN_ETSI = 1,
  WIFI_REGULATORY_DOMAIN_FCC, //!< default
  WIFI_REGULATORY_DOMAIN_JAPAN,
};
/*
// wifi state
enum {
    WIFI_ST_OFF,        //!< driver haven't init yet
    WIFI_ST_SHUTDOWN,   //!< shutdown mode: turns OFF most parts of the circuit
    WIFI_ST_WAKEUP,     //!< enabled wireless lan
    WIFI_ST_INIT,       //!< sending primitive init sequence
    WIFI_ST_DELAY,      //!< delay between primitive init and first command
    WIFI_ST_RUNNING,    //!< normal running
    WIFI_ST_DUMMY_RX = 99,  //!< for recovery
    WIFI_ST_RESET = 100,    //!< for recovery
    WIFI_ST_RECONNECT = 101,//!< for recovery
};*/

// wifi function
//int os_wifi_control(unsigned int ctrl, void *param) // for system process only
#define os_wifi_control(x,y)    (int)sys_func2(OS_FN_WIFI_CONTROL,(DWORD)(x), (DWORD)(y))

// int os_wifi_send_command(char *cmd, unsigned int len)    // for system process only
#define os_wifi_send_command(x,y) (int)sys_func2(OS_FN_WIFI_SEND_COMMAND,(DWORD)(x), (DWORD)(y))

//unsigned int os_wifi_recv_response(char *resp_buf, unsigned int len)    // for system process only
#define os_wifi_recv_response(x,y)  sys_func2(OS_FN_WIFI_RECV_RESPONSE,(DWORD)(x), (DWORD)(y))

// unsigned int os_wifi_status(void);
#define os_wifi_status()    sys_func0(OS_FN_WIFI_STATUS)

//int os_wifi_profiles_read(T_WiFi_AP_Profile *ap)    // for signed app only
#define os_wifi_profiles_read(x)  (int)sys_func1(OS_FN_WIFI_PROFILES_READ,(DWORD)(x))

//int os_wifi_profiles_write(T_WiFi_AP_Profile *ap, unsigned int numAP) // for signed app only
#define os_wifi_profiles_write(x,y) (int)sys_func2(OS_FN_WIFI_PROFILES_WRITE,(DWORD)(x), (DWORD)(y))

//Wifi region:   WIFI_REGULATORY_DOMAIN_NONE = 0,  WIFI_REGULATORY_DOMAIN_ETSI = 1,  WIFI_REGULATORY_DOMAIN_FCC =2, WIFI_REGULATORY_DOMAIN_JAPAN =3
//unsigned int wifi_get_regulatory_domain()
#define os_wifi_get_regulatory_domain() (int)sys_func0(OS_FN_WIFI_GET_REGULATORY_DOMAIN)

//int wifi_set_regulatory_domain(unsigned int region)
#define os_wifi_set_regulatory_domain(x) (int)sys_func1(OS_FN_WIFI_SET_REGULATORY_DOMAIN,(DWORD)(x))

/*--------------wifi ATWINC1500 functions -------------*/
//int os_wifi_socket_listen(DWORD socket, T_TCP_PORT * a_tcp, T_UDP_PORT * a_udp)
#define os_wifi_socket_listen(x,y,z)   (int)sys_func3(OS_FN_WIFI_SOCKET_LISTEN,(DWORD)(x), (DWORD)(y), (DWORD)(z))

//int os_wifi_socket_connect(DWORD socket, T_TCP_PORT * a_tcp, T_UDP_PORT * a_udp)
#define os_wifi_socket_connect(x,y,z)   (int)sys_func3(OS_FN_WIFI_SOCKET_CONNECT,(DWORD)(x), (DWORD)(y), (DWORD)(z))

//int os_wifi_socket_disconnect(DWORD socket,BOOLEAN isTcp)
#define os_wifi_socket_disconnect(x,y)    (int)sys_func2(OS_FN_WIFI_SOCKET_DISCONNECT,(DWORD)(x),(DWORD)(y))

//int os_wifi_socket_get_state(DWORD tcp_socket)
#define os_wifi_socket_get_state(x)    (int)sys_func1(OS_FN_WIFI_SOCKET_GET_STATE,(DWORD)(x))

//DWORD os_wifi_socket_get_status(DWORD tcp_socket)
#define os_wifi_socket_get_status(x)    (DWORD)sys_func1(OS_FN_WIFI_SOCKET_GET_STATUS,(DWORD)(x))

//DWORD os_wifi_socket_send_tcp(DWORD tcp_socket, T_BUF *txSocketBuf)
#define os_wifi_socket_send_tcp(x,y)    (int)sys_func2(OS_FN_WIFI_SOCKET_SEND_TCP,(DWORD)(x), (DWORD)(y))

//DWORD os_wifi_socket_send_udp(DWORD udp_socket, T_BUF *txSocketBuf)
#define os_wifi_socket_send_udp(x,y)    (int)sys_func2(OS_FN_WIFI_SOCKET_SEND_UDP,(DWORD)(x), (DWORD)(y))


#define _A5_AES_      1

#ifdef _A5_AES_

typedef struct {
  DWORD s_key[8];
  WORD  w_size;     // 16,24,32
  WORD  w_mode;    //  not use
} T_AES_CONTEXT;
#define os_AES_Setup_Encrypt_Key    os_AES_Setup_Key
#define os_AES_Setup_Decrypt_Key    os_AES_Setup_Key
#define os_AES_Setup_Key(x,y,z)       sys_func3(OS_FN_AES_SETUP_KEY,(DWORD)(x),(DWORD)(y),(DWORD)(z))

#else

// Basic AES Block Cipher Functions
//
// t_ctx         - storage structure for unrolled block cipher operation.
// a_userkey     - 16 , 24 or 32 bytes opaque byte array of AES user key
//                 in big endian notation.
// nbits         - number of bits of AES user keys, 128, 192 or 256 bits.
// a_intxt       - 128 bits raw input bytes for encryption and decryption
//                 operations.
// a_outxt       - 128 bits output bytes after encryption and decryption
//                 operations.
typedef struct {
  DWORD rk[4 * (14 + 1)];     // unrolled AES cipher operations
  DWORD rounds;               // rounds of iteration for single block op
} T_AES_CONTEXT;
#define os_AES_Setup_Encrypt_Key(x,y,z)   sys_func3(OS_FN_AES_SETUP_ENCRYPT_KEY,(DWORD)(x),(DWORD)(y),(DWORD)(z))
#define os_AES_Setup_Decrypt_Key(x,y,z)   sys_func3(OS_FN_AES_SETUP_DECRYPT_KEY,(DWORD)(x),(DWORD)(y),(DWORD)(z))

#endif // _A5_AES_

#define K_AES_CIPHER_BLOCK_SIZE  (128/8)

// BOOLEAN os_AES_Setup_Encrypt_Key(T_AES_CONTEXT *t_ctx,
//                                  BYTE *a_userkey,
//                                  int nbits)
// BOOLEAN os_AES_Setup_Decrypt_Key(T_AES_CONTEXT *t_ctx,
//                                  BYTE *a_userkey,
//                                  int nbits)
//   - given user key, unroll cipher operations and initialize rk
//     and rounds in T_AES_KEY structure for subsequence encryption
//     and decryption operations.
//
// void os_AES_Encrypt(T_AES_CONTEXT *t_ctx,
//                     BYTE* a_intxt,
//                     BYTE* a_outxt)
// void os_AES_Decrypt(T_AES_CONTEXT *t_ctx,
//                     BYTE* a_intxt,
//                     BYTE* a_outxt)
//   - perform AES encryption and decryption block cipher operation
//     over 128 bits input byte array.
#define os_AES_Encrypt(x,y,z)             sys_func3(OS_FN_AES_ENCRYPT_BLOCK,(DWORD)(x),(DWORD)(y),(DWORD)(z))
#define os_AES_Decrypt(x,y,z)             sys_func3(OS_FN_AES_DECRYPT_BLOCK,(DWORD)(x),(DWORD)(y),(DWORD)(z))


enum nfc_status_list
{
  K_NFC_STATUS_OK = 0,
  K_NFC_STATUS_WAIT_RES,
  K_NFC_STATUS_CRC_ERR,
  K_NFC_STATUS_FRAME_ERR,
  K_NFC_STATUS_PARITY_ERR,
  K_NFC_STATUS_COLL_ERR,
  K_NFC_STATUS_CODE_ERR,
  K_NFC_STATUS_TIMEOUT,
  K_NFC_STATUS_IDLE = 0xff
};

// os_nfc_info_get() Index
enum nfc_info_list
{
  K_NFC_INFO_DEFAULT = 0,
  K_NFC_INFO_INIT_COMPLETE,
};

/* NFC Function */

//DWORD os_nfc_open_felica(BYTE* ap_initstr)
#define os_nfc_open_felica(x)               sys_func1(OS_FN_NFC_OPEN_FELICA, (DWORD)(x))

//DWORD os_nfc_send_felica_command(BYTE* ap_src, BYTE b_len, WORD w_timeout)
#define os_nfc_send_felica_command(x,y,z)   sys_func3(OS_FN_NFC_SEND_FELICA_COMMAND, (DWORD)(x),(DWORD)(y),(DWORD)(z))

//DWORD os_nfc_cancel(void)
#define os_nfc_cancel()                     sys_func0(OS_FN_NFC_CANCEL)

//DWORD os_nfc_close(void)
#define os_nfc_close()                      sys_func0(OS_FN_NFC_CLOSE)

//DWORD os_nfc_get_valid_resp(BYTE* ap_dest, BYTE* p_len)
#define os_nfc_get_valid_resp(x,y)        sys_func2(OS_FN_NFC_GET_VALID_RESP,(DWORD)(x),(DWORD)(y))

//DWORD os_nfc_info_get(DWORD ad_idx, BYTE *ap_dest)
#define os_nfc_info_get(x,y)                sys_func2(OS_FN_NFC_INFO_GET, (DWORD)(x),(DWORD)(y))

//DWORD os_nfc_info_set(DWORD ad_idx, BYTE *ap_src)
#define os_nfc_info_set(x,y)                sys_func2(OS_FN_NFC_INFO_SET, (DWORD)(x),(DWORD)(y))

//DWORD os_nfc_ic_reg_direct_write(BYTE b_addr, BYTE b_val)
#define os_nfc_ic_reg_direct_write(x,y)   sys_func2(OS_FN_NFC_IC_REG_DIRECT_WRITE, (DWORD)(x),(DWORD)(y))

//DWORD os_nfc_ic_reg_direct_read (BYTE b_addr, BYTE* ap_val)
#define os_nfc_ic_reg_direct_read(x,y)    sys_func2(OS_FN_NFC_IC_REG_DIRECT_READ, (DWORD)(x),(DWORD)(y))

/* GPS Function */
typedef struct {
  BYTE b_gps_time[7];        /* "HHMMSS" */
  BYTE b_gps_date[7];        /* "ddmmyy" */
  BYTE b_gps_fix[7];         /* "NO FIX", "ES FIX"(Estimated fix) or "2D FIX" or "3D FIX" or "CE FIX" */
  BYTE b_gps_latitude[16];   /* "NDDMM.MMMM" or "SDDMM.MMMM" or "-DD.DDDDDD" or "DD.DDDDDD" */
  BYTE b_gps_longitude[16];  /* "EDDDMM.MMMM" or "WDDDMM.MMMM" or "-DDD.DDDDDD" or "DDD.DDDDDD" */
//  BYTE b_gps_altitude[6];    /* "+mmmm" or "-mmmm" */
//  BYTE b_gps_speed[4];       /* "sss" */
}T_GPS_INFO;

typedef struct {
  BYTE b_apn[32];            /* APN */
  BYTE b_username[32];       /* Username */
  BYTE b_password[32];       /* Password */
  BYTE b_gps_fix[7];         /* "CE FIX" */
  BYTE b_gps_latitude[16];   /* "NDDMM.MMMM" or "SDDMM.MMMM" or "-DD.DDDDDD" or "DD.DDDDDD" */
  BYTE b_gps_longitude[16];  /* "EDDDMM.MMMM" or "WDDDMM.MMMM" or "-DDD.DDDDDD" or "DDD.DDDDDD" */
}T_GPRS_GPS_INFO;

//DWORD os_gps_on(void);
#define os_gps_on()                       sys_func0(OS_FN_GPS_ON)

//DWORD os_gps_off(void);
#define os_gps_off()                       sys_func0(OS_FN_GPS_OFF)

//DWORD os_gps_get_info(T_GPS_INFO *a_gps_info);
#define os_gps_get_info(x)                  sys_func1(OS_FN_GPS_GET_INFO, (DWORD)x)

//DWORD os_gprs_get_gps_info(T_GPRS_GPS_INFO *a_gprs_gps_info);
#define os_gprs_get_gps_info(x)             sys_func1(OS_FN_GPRS_GET_GPS_INFO, (DWORD)x)

// DWORD os_gps_cmd(BYTE *a_cmd);
#define os_gps_cmd(x)                       sys_func1(OS_FN_GPS_CMD,(DWORD)(x))

// DWORD os_gps_resp(BYTE *a_resp,DWORD a_maxlen);
#define os_gps_resp(x,y)                    sys_func2(OS_FN_GPS_RESP,(DWORD)(x),(DWORD)(y))

enum gps_mode_list
{
  K_GPS_MODE_INIT = 0,    // system only
  K_GPS_MODE_NORMAL,
  K_GPS_MODE_ROUTER,      // system only
  K_GPS_MODE_UPDATE,      // system only

  K_GPS_HOT_RESET = 0x81,
  K_GPS_WARM_RESET,
  K_GPS_COLD_RESET,
  K_GPS_FULL_COLD_RESET,
};

// DWORD os_gps_reset(DWORD a_mode);
#define os_gps_reset(x)                   sys_func1(OS_FN_GPS_RESET,(DWORD)(x))

// DWORD os_gps_enabled(void);
#define os_gps_enabled()                  sys_func0(OS_FN_GPS_ENABLED)


                /****** DTimer ******/
// It will be used by CLICC when sending command.
// If CLICC is not used, it can be used.
// void os_dtimer_latch_start(void)
#define os_dtimer_latch_start()           sys_func0(OS_FN_DTIMER_LATCH_START)

// void os_dtimer_latch_stop(void)
#define os_dtimer_latch_stop()            sys_func0(OS_FN_DTIMER_LATCH_STOP)

// DWORD os_dtimer_latch_time(void)
#define os_dtimer_latch_time()            sys_func0(OS_FN_DTIMER_LATCH_TIME)

/******* USB FingerPrint *******/
enum{
USBFPRT_MODEL_UNKNOWN,
USBFPRT_MODEL_FS80H,
};

enum{
USBFPRT_GENERAL_INFO_MODEL,         // r
USBFPRT_GENERAL_INFO_IMG_WIDTH,     // r
USBFPRT_GENERAL_INFO_IMG_HEIGHT,    // r
USBFPRT_GENERAL_INFO_IMG_BPP,       // r
USBFPRT_GENERAL_INFO_END=0x100,
};

// FS80H
enum{
USBFPRT_FS80H_INFO_IMG_WIDTH = USBFPRT_GENERAL_INFO_END, // r
USBFPRT_FS80H_INFO_IMG_HEIGHT,                           // r
USBFPRT_FS80H_INFO_IMG_BPP,                              // r
USBFPRT_FS80H_INFO_DOSAGE,                               // rw
USBFPRT_FS80H_INFO_GREEN_GIMMICK_DIODE,                  // rw
USBFPRT_FS80H_INFO_RED_GIMMICK_DIODE,                    // rw
};

// int os_fprt_open(void);
#define os_fprt_open()                   sys_func0(OS_FN_FPRT_OPEN)

// void os_fprt_close(void);
#define os_fprt_close()                   sys_func0(OS_FN_FPRT_CLOSE)

// int os_fprt_scanimg(BYTE** s_img);
#define os_fprt_scan_img(x)               sys_func1(OS_FN_FPRT_SCAN_IMG,(DWORD)(x))

// int os_fprt_info_set(unsigned int idx, void* ap_src);
#define os_fprt_info_set(x,y)             sys_func2(OS_FN_FPRT_INFO_SET,(DWORD)(x),(DWORD)(y))

// int os_prt_info_get(unsigned int idx, void* ap_dest);
#define os_fprt_info_get(x,y)             sys_func2(OS_FN_FPRT_INFO_GET,(DWORD)(x),(DWORD)(y))

//
// Primitive EC cryptography functions required by SSL/TLS in BASE
// module for ECDHE key-exchange procedure.
//

// List of supported EC curves (prime field)
#define K_ECC_CURVENAME_ID_secp192r1 0x0013 /* matched with SSL/TLS EC named curve Id */
#define K_ECC_CURVENAME_ID_secp256r1 0x0017 /* matched with SSL/TLS EC named curve Id */

// int os_ec_crypto_keypair_generate(BYTE *out,
//                                   DWORD out_len,
//                                   DWORD curve_name_id)
//
// Generate a valid pair of elliptic curve key-pair based on the named
// EC curve.  The key-pair consists of a private key (a scalar on the
// EC curve field) and a public key (a point residing on the EC curve
// field).  Returned key-pair is a 3M bytes block arranged as follows,
//
//   struct output {
//     BYTE private_key[M];   // EC curve private key
//     BYTE public_key[2M];   // EC cuvre public key
//   }
//
// For example, no. of bits of the private key of a SECG "secp192r1"
// EC curve is 192 bits.  Therefore, the M for this curve is 24.
//
// out           - output buffer
// out_len       - length of the output buffer.  Make sure this argument
//                 is more than 3M.
// curve_name_id - the ID of EC curve.
//
// Return negative values on error.  Or, no. of written bytes on
// success.
//
#define os_ec_crypto_keypair_generate(x,y,z)  (int)sys_func3(OS_FN_ECC_KEYPAIR_GENERATE,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// int os_ec_crypto_pubkey_verify(const BYTE *in,
//                                DWORD in_len,
//                                DWORD curve_name_id)
//
// Validate a public key based on the named EC curve.  Return TRUE if
// the public key is a valid point on the named EC cuvre; Otherwise,
// return FALSE.
//
// in            - a public key of the named EC curve.
// in_len        - length of the public key in no. of bytes.
// curve_name_id - the ID of the EC curve.
//
// The public key from input arguement "in" is arranged as follows,
//
//   struct input {
//     BYTE public_key[2M];
//   }
//
// M is the no. of bytes required for storing the private key of
// specified EC curve.
#define os_ec_crypto_pubkey_verify(x,y,z)     (int)sys_func3(OS_FN_ECC_PUBKEY_VERIFY,(DWORD)(x),(DWORD)(y),(DWORD)(z))

typedef struct {
  DWORD       dw_bytes;   // no. of bytes for storing a single ordinate (aka, the M)
  const BYTE *bp_d;       // input scalar d
  const BYTE *bp_P_x;     // x-ordinate of input point P in MSB byte order
  const BYTE *bp_P_y;     // y-ordinate of input point P in MSB byte order
  BYTE       *bp_Q_x;     // x-ordinate of output point Q in MSB byte order
  BYTE       *bp_Q_y;     // y-ordinate of output point Q in MSB byte order
} T_ECC_POINT_SCALAR_MULTIPLY;

// int os_ec_crypto_scalar_multiply(T_ECC_POINT_SCALAR_MULTIPLY *param, DWORD curve_name_id)
//
// Perform the EC crypto scalar multiplication operation Q = d dot P,
// where d is a scalar and P is a point on the EC curve.
//
// param         - a structure contains info of d, P and output buffer.
// curve_name_id - the ID of EC curve
//
// Return 0 on success; otherwise, return negative value on return.
#define os_ec_crypto_scalar_multiply(x,y)    (int)sys_func2(OS_FN_ECC_SCALAR_MULTIPLY, (DWORD)(x), (DWORD)(y))
                    /******* GCM AES *******/
#define K_GcmAesBlkSize     16
#define K_GcmAesModeShift   31
#define K_GcmAesDecrypt     1                    
#define K_GcmAesEncrypt     0

#define K_GCM_AES_OK              (0)
#define K_GCM_AES_ERR_ACC         (-1)
#define K_GCM_AES_ERR_GENERAL     (-2)
#define K_GCM_AES_ERR_TAG         (-3)
#define K_GCM_AES_ERR_SEQ         (-4)
#define K_GCM_AES_KEY_LEN_ERR     (-5)
               
typedef struct {
  DWORD d_status;
  DWORD d_mode;       // K_GcmAesDecrypt/K_GcmAesEncrypt
  T_AES_CONTEXT s_aes;
  BYTE *p_omess;      // out message point
  DWORD d_aad_len;
  DWORD d_c_len;
  DWORD s_j0[4];
  DWORD s_ghash[4];
  DWORD d_aes_ctr;
  DWORD d_tot_len;
  DWORD d_len;
  BYTE  s_block[K_GcmAesBlkSize];
} T_GCM_AES_CONTEXT;

// int os_gcm_aes_open(T_GCM_AES_CONTEXT * a_ctx); // input AES key
#define os_gcm_aes_open(x)            (int)sys_func1(OS_FN_GCM_AES_OPEN,(DWORD)x)      

// int os_gcm_aes_init(T_GCM_AES_CONTEXT * a_ctx,BYTE * a_iv, BYTE a_iv_len); 
#define os_gcm_aes_init(x,y,z)        (int)sys_func3(OS_FN_GCM_AES_INIT,(DWORD)x,(DWORD)y,(DWORD)z)

// int os_gcm_aes_process_aad(T_GCM_AES_CONTEXT * a_ctx, BYTE* a_aad, DWORD a_len); // aad not multiple of 16 will be 0's filled
#define os_gcm_aes_process_aad(x,y,z) (int)sys_func3(OS_FN_GCM_AES_PROCESS_AAD,(DWORD)x,(DWORD)y,z)

// int os_gcm_aes_process(T_GCM_AES_CONTEXT * a_ctx, BYTE* a_mess, DWORD a_len);
#define os_gcm_aes_process(x,y,z)     (int)sys_func3(OS_FN_GCM_AES_PROCESS,(DWORD)x,(DWORD)y,z)

// init os_gcm_aes_finish(T_GCM_AES_CONTEXT * a_ctx, BYTE * a_auth_tag) // mess not multiple of 16 will be 0's filled
#define os_gcm_aes_finish(x,y)        (int)sys_func2(OS_FN_GCM_AES_FINISH,(DWORD)x,(DWORD)y)

// init os_gcm_aes_close(T_GCM_AES_CONTEXT * a_ctx)
#define os_gcm_aes_close(x)           (int)sys_func1(OS_FN_GCM_AES_CLOSE,(DWORD)x)

/******* Image Sensor *******/

#define YUYV 0x56595559 // ASCII: YUYV
#define Y800 0x30303859 // ASCII: Y800

typedef struct{
  DWORD type;
  DWORD width;
  DWORD height;
  DWORD size;
}T_IMAGE_FRAME;

enum{
  SENSOR_MODE_QRCODE_1,
  SENSOR_MODE_QRCODE_2,
  SENSOR_MODE_FULL_SCREEN_1 = 0x40,
  SENSOR_MODE_FULL_SCREEN_2,
  SENSOR_MODE_PHOTO = 0x80,
  SENSOR_MODE_SP_0 = 0x100,
};

// int os_image_sensor_open(DWORD mode, T_LCDG* preview_cfg, T_IMAGE_FRAME* cfg);
#define os_image_sensor_open(x,y,z)             (int)sys_func3(OS_FN_IMG_SENSOR_OPEN, (DWORD)(x), (DWORD)(y), (DWORD)(z))

// int os_image_sensor_get_frame(BYTE *ptr);
#define os_image_sensor_get_frame(x)            (int)sys_func1(OS_FN_IMG_SENSOR_GET_FRAME, (DWORD)(x))

// int os_image_sensor_get_frame_complete(void);
#define os_image_sensor_get_frame_complete()    (int)sys_func0(OS_FN_IMG_SENSOR_GET_FRAME_COMPLETE)

// void os_image_sensor_close(void);
#define os_image_sensor_close()                 sys_func0(OS_FN_IMG_SENSOR_CLOSE)

// int image_sensor_led_open(DWORD mode);
#define os_image_sensor_led_open(x)             (int)sys_func1(OS_FN_IMG_SENSOR_LED_OPEN, (DWORD)x)

#endif
