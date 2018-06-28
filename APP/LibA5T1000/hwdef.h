//-----------------------------------------------------------------------------
//  File          : hwdef.h
//  Module        : hwdef
//  Description   : 
//  Author        : Pody
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |         Class define : Leading C                                         |
// |                Class : Leading c                                         |
// |        Struct define : Leading T                                         |
// |               Struct : Leading s                                         |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g                                         |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |            Char size : Leading c                                         |
// |             Int size : Leading i                                         |
// |            Byte size : Leading b                                         |
// |            Word size : Leading w                                         |
// |           Dword size : Leading d                                         |
// |          DDword size : Leading dd                                        |
// |                Array : Leading s, (sb = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//                Pody        Initial Version.
//  2011/09/15	  Lewis       Port T800 Color/Mono Support Flag.
//  2012/03/01	  Lewis       - Remove "#include system.h"
//                            - Add T1000.
//                            - Add PR500.
//  2013/05/30    Lewis       - Add T600 
//  2013/09/13    Lewis       - Support CREON PCI V4.0
//  2014/06/26    Lewis       - ADD WIFI_SUPPORT/FLASH_SUPPORT.
//  2014/11       Pody        - Add SP530
//  2015/03/10    Lewis       - Add GPS_SUPPORT 
//                            - T1000 support MLCD
//  2015/10/01    Lewis       Restruct defines
//  2015/12/01    Lewis       Re-Write the defined stuff, which may cause issue.
//  2016/02/15    Pody				- Add T300, T810
//  2016/07/21    Jacky       - Add SP530 to AUX1_SUPPORT
//  2017/03/21    Lewis       - T300 Support Camera
//-----------------------------------------------------------------------------
#ifndef _HWDEF_H_
#define _HWDEF_H_
#include "common.h"
#include "system.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Base on HW
//-----------------------------------------------------------------------------
#define ALL_MODELS        (PR608|TIRO|R700|T800|PR608D|T700|T1000|SR300|PR500|CR600|T600|CREON4|T1000CT|SP510|SP530|A5T1000|T300|T810)
#if (ALL_MODELS == 0)
	NEW_TERMINAL_HERE{} : add and handle new terminal HW configuration
#endif
#define CLCD_SUPPORT      (T800|T1000|A5T1000|T300|T810)
#define MLCD_SUPPORT      (PR608|TIRO|R700|T800|PR608D|T700|T1000|SR300|PR500|CR600|T600|CREON4|T1000CT|SP510|SP530) 
#define DOCK_SUPPORT      (PR608D|CREON4)
#define RTC_SUPPORT       (ALL_MODELS)
#define RAM_SUPPORT       (SP530==0)
#define LPT_SUPPORT       (TIRO|PR608D)
#define TMLPT_SUPPORT     (R700|T800|PR608D|T700|T1000|T600|CREON4|T1000CT|A5T1000|T300|T810)
#define USBS_SUPPORT      (ALL_MODELS)
#define GPRS_SUPPORT      (TIRO|T800|T700|T1000|T600|A5T1000|T300|T810)
#define SIM2_NSUPPORT     (TIRO|T600)
#define AUXD_SUPPORT      (TIRO|T800|T700|T1000|SR300|PR500|CR600|T1000CT|SP510|A5T1000|T300|T810)
#define AUX1_SUPPORT      (PR608|TIRO|R700|T800|PR608D|T700|T1000|T600|T1000CT|A5T1000|T300|T810|SP530)
#define AUX2_SUPPORT      (PR608|TIRO|R700|SP510)
#define ECR1_SUPPORT      (PR608D|CREON4)
#define ECR2_SUPPORT      (PR608D|CREON4)
#define ECR3_SUPPORT      (CREON4)
#define LAN_SUPPORT       (PR608|TIRO|R700|T800|PR608D|T1000|CREON4|T1000CT|A5T1000|T300|T810)
#define CICC_SUPPORT      (PR608|TIRO|R700|T800|PR608D|T1000|CR600|CREON4|T1000CT|SP530|A5T1000|T300|T810)
#define CLICC_SUPPORT     (PR608|R700|T1000|SR300|SP530|A5T1000|T300|T810)
#define SAM1_SUPPORT      (PR608|TIRO|R700|T800|PR608D|T1000|SR300|PR500|CR600|CREON4|T1000CT|SP510|SP530|A5T1000|T300|T810)
#define SAM2_SUPPORT      (PR608|TIRO|R700|T800|PR608D|T1000|SR300|PR500|CREON4|T1000CT|SP510|A5T1000|T300|T810)
#define SAM3_SUPPORT      (PR608|TIRO|R700|T800|PR608D|T1000|SR300|PR500|CREON4|T1000CT|SP510|A5T1000)
#define SAM4_SUPPORT      (TIRO|T800|T1000|A5T1000)
#define MDM_SUPPORT       (PR608|TIRO|R700|T800|PR608D|T1000|CREON4|T1000CT|A5T1000|T300|T810)
#define CDC_SUPPORT       (PR608|PR608D|T1000|T1000CT|A5T1000|T300|T810)
#define PN532_SUPPORT     (T800)
#define KBD_SUPPORT       (SR300==0)
#define MALLOC_SUPPORT    (SP530==0)
#define PN512_SUPPORT     (PR608|PR608D|T1000|PR500|CREON4|T1000CT|SP510)
#define FLASH_SUPPORT     (SP530==0)
#define SFL_SUPPORT       (SP530)
#define CAM_SUPPORT       (A5T1000|T300)

//-----------------------------------------------------------------------------
// Base on Software variances
//-----------------------------------------------------------------------------
// Support Graphic Libraries
#define GLIB_NOT_SUPPORT  (SR300|SP530)
// Do Not Support TR31 2010
#define TR31_NOT_SUPPORT  (TIRO|T800|T700|SR300|CR600|T600)
//-----------------------------------------------------------------------------
// Based on Headers variances
//-----------------------------------------------------------------------------
// os_key_save(x,y,z)
#define KEY_SAVE_V0_SUPPORT  (PR608|TIRO|R700|T800|PR608D|T700|SR300|CR600|T600)
// os_kbd_open()
#define KBD_OPEN_V1_SUPPORT  (SP530)
// os_beep_open(x)
#define BUZ_OPEN_V0_SUPPORT  (PR608|TIRO|R700|T800|PR608D|T700|SR300|PR500|CR600|T600|SP510)
// os_sam_open()
#define SAM_OPEN_V0_SUPPORT  (PR608|TIRO|R700|T800|PR608D|SR300|PR500|CR600|SP510)
// bs_dns_open()
#define DNS_OPEN_V0_SUPPORT  (PR608|TIRO|R700|PR608D|CREON4)
// T_APP_INFO w_checksum
#define CHKSUM16_SUPPORT     (PR608|TIRO|R700|T800|PR608D|T700|T1000|SR300|PR500|CR600|T600|CREON4|T1000CT|SP510)
// bitwise error status
#define CDC_STS_V0_SUPPORT   (PR608|PR608D)
// bs_line_status()
#define LINESTS_V0_SUPPORT   (PR608|TIRO|R700|T800|PR608D|CREON4)
// T_DUKPT_KB = T_DUKPT_KEYBANK;
#define DUKPT_KB0_SUPPORT    (PR608|R700|PR608D|T1000|PR500|CREON4|T1000CT|SP510|SP530)
// os_sm4_xxcrypt(T_SM4_XXCRYPT *)
#define SM4_V0_SUPPORT    (PR608|R700|PR608D|T1000|PR500|T1000CT)
//typedef struct {
//  WORD    w_req_len;
//  BYTE    s_uri[K_HttpMaxUriLen];
//  BYTE    b_method;
//  BYTE    b_close;   
//  WORD    w_max_rlen;
//  WORD    w_timeout; 
//} T_HTTP_REQ;
#define H_REQ_V0_SUPPORT (PR608|TIRO)
//typedef struct {
//  WORD    w_req_len;
//  BYTE    s_uri[K_HttpMaxUriLen];
//  BYTE    b_method;
//  BYTE    b_close;    
//  WORD    w_max_rlen; 
//  WORD    w_timeout;  
//  BYTE    s_type[K_HttpMaxTypeLen];
//} T_HTTP_REQ;
#define H_REQ_V1_SUPPORT (R700|PR608D|CREON4)
//typedef struct {
//  WORD    w_req_len;
//  char    *p_uri;
//  BYTE    b_method;
//  BYTE    b_close;    
//  WORD    w_max_rlen; 
//  WORD    w_timeout;  
//  BYTE    s_type[K_HttpMaxTypeLen];
//} T_HTTP_REQ;
#define H_REQ_V2_SUPPORT (T800|T700|T1000|T600|A5T1000|T300|T810)
//typedef struct {
//  WORD    w_req_len;
//  BYTE    b_method;
//  BYTE    b_close;    
//  WORD    w_max_rlen; 
//  WORD    w_timeout;  
//  char    *p_uri;
//  char    *p_cust_hdr;
//  BYTE    s_type[K_HttpMaxTypeLen];
//} T_HTTP_REQ;
#define H_REQ_V3_SUPPORT (T1000CT)
// able to print rmb symbol
#define PRN_RMB_SUPPORT   (T1000|A5T1000|T300|T810)

// ppp device defines
//-----------------------------------------------------------------------------
// LINE+ECR1+ECR2+ECR3+GPRS
#define PPPDEV_V6_SUPPORT (CREON4)
// LINE+AUX1+AUXD+GPRS
#define PPPDEV_V5_SUPPORT (T700|T1000|T600|T1000CT|A5T1000|T300|T810)
// LINE+ECR1+ECR2+GPRS
#define PPPDEV_V4_SUPPORT (PR608D)
// GPRS+AUX1+AUXD+LINE
#define PPPDEV_V3_SUPPORT (T800)
// LINE+AUX1+AUX2+GPRS
#define PPPDEV_V2_SUPPORT (R700)
// LINE+AUX1+AUX2+AUX0+GPRS
#define PPPDEV_V1_SUPPORT (TIRO)
// LINE+AUX1+AUX2
#define PPPDEV_V0_SUPPORT (PR608)


// lpt_open init string
//-----------------------------------------------------------------------------
// len + pwr adj + batt adj + pwr contrast + batt contrast
#define LPTCFG_V3_SUPPORT (T1000|CREON4|T1000CT|A5T1000|T300|T810)
// len + pwr adj + pwr contrast
#define LPTCFG_V2_SUPPORT (PR608D)
// len + mode + pwr speed + batt speed + pwr adj + batt adj
#define LPTCFG_V1_SUPPORT (T800)
// len + mode + speed + pwr adj + batt adj
#define LPTCFG_V0_SUPPORT (R700|T700|T600)

// mdm line speed defines
//-----------------------------------------------------------------------------
// 300,F1200,2400,F9600,auto,autov42,union,14400,33600,56000
#define MDMSPD_V3_SUPPORT (CREON4|T1000CT)
// 300,F1200,2400,F9600,auto,autov42,union,14400,33600,56000,9600
#define MDMSPD_V2_SUPPORT (T1000|A5T1000|T300|T810)
// 300,F1200,F2400,2400,F9600,auto,autov42,union,14400,33600,56000
#define MDMSPD_V1_SUPPORT (R700|PR608D)
// 300,1200,2400,9600,auto,autov42,union,14400,33600,56000
#define MDMSPD_V0_SUPPORT (PR608|TIRO|T800|T700|T600)

//-----------------------------------------------------------------------------
// Based on OS func
//-----------------------------------------------------------------------------
#ifdef os_usbhost_status
  #define USBH_SUPPORT      1
#else
  #define USBH_SUPPORT      0
#endif
#if defined(os_mmc_open) || defined(os_usd_open)
  #define MSD_SUPPORT      1
#else
  #define MSD_SUPPORT      0
#endif
#ifdef os_msr_open
  #define MSR_SUPPORT      1
#else
  #define MSR_SUPPORT      0
#endif
#ifdef os_tile_open
  #define TILT_SUPPORT      1
#else
  #define TILT_SUPPORT      0
#endif
#ifdef os_usbotg_get_state
  #define OTG_SUPPORT      1
#else
  #define OTG_SUPPORT      0
#endif
#ifdef os_tpad_open 
  #define TPAD_SUPPORT      1
#else
  #define TPAD_SUPPORT      0
#endif
#ifdef os_barcode_open
  #define BRDR_SUPPORT      1
#else
  #define BRDR_SUPPORT      0
#endif
#ifdef os_wifi_status
  #define WIFI_SUPPORT      1
#else
  #define WIFI_SUPPORT      0
#endif
#ifdef os_gps_on
  #define GPS_SUPPORT      1
#else
  #define GPS_SUPPORT      0
#endif
#ifdef os_mif_open
  #define MIF_SUPPORT      1
#else
  #define MIF_SUPPORT      0
#endif
#ifdef os_file_open 
  #define FILE_SUPPORT      1
#else
  #define FILE_SUPPORT      0
#endif
#ifdef os_record_file_create
  #define RFILE_SUPPORT      1
#else
  #define RFILE_SUPPORT      0
#endif
#ifdef os_data_rec_length
  #define DREC_SUPPORT      1
#else
  #define DREC_SUPPORT      0
#endif
#ifdef os_mcp_open
  #define MCP_SUPPORT      1
#else
  #define MCP_SUPPORT      0
#endif
#ifdef os_bt_get_status
  #define BLUETOOTH_SUPPORT      1
#else
  #define BLUETOOTH_SUPPORT      0
#endif
#ifdef os_mcp_start_ssl
  #define MCP_SUPPORT_SSL        1
#else
  #define MCP_SUPPORT_SSL        0
#endif

#if ((ALL_MODELS)==0)
ADD_NEW_MODELS_REQUIRED{}
#endif         

#if (SP530)
  //mapping os_config_xxx func to os_data_rec_xxx
  #define K_CF_LcdContrast        K_DR_LcdContrast
  #define K_CF_LcdTimeout         K_DR_LcdTimeout
  #define K_CF_LcdBrightness      K_DR_LcdBrightness
  #define os_config_length(x)           os_data_rec_length(x)
  #define os_config_read(x,y)           (os_data_rec_read(x,y)==K_Ok)
  #define os_config_write(x,y)          (os_data_rec_update(x,y)==K_Ok)
  #define os_config_update()            TRUE
  //mapping os_mmc_xx to os_usd_xx
  #define T_MMC                   T_BLK
  #define os_mmc_open()                 os_usd_open()        
  #define os_mmc_close()                os_usd_close()       
  #define os_mmc_block_size()           os_usd_block_size()  
  #define os_mmc_size()                 os_usd_size()        
  #define os_mmc_pstatus()              os_usd_pstatus()     
  #define os_mmc_block_read(x)          os_usd_block_read(x) 
  #define os_mmc_block_write(x)         os_usd_block_write(x)
  #define os_mmc_read(x,y)              os_usd_read(x,y)     
  #define os_mmc_write(x,y)             os_usd_write(x,y)    
  #define os_mmc_inserted()             os_usd_inserted()    
  //mapping os_malloc and os_free
  #ifndef _S0_HWDEF_H_ //PC1602 disable in AllInOne (s0_hwdef.h include this file)
		extern void *MemAlloc(int aSize); //sysutil.c
		extern void MemFree(void *aPtr);  //sysutil.c
		#define os_malloc(x)            MemAlloc(x)
		#define os_free(x)              MemFree(x)
	#endif
#endif

//----------------------------------------------------------------------------
// Constant
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Generic API
//----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
#endif //_HWDEF_H_

