//-----------------------------------------------------------------------------
//  File          : midware.h
//  Module        : ARM9TERM
//  Description   : Header for Middleware.
//  Author        : Lewis
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |         Struct/Array : Leading s                                         |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g  (variable use outside one C file)      |
// |                                                                          |
// |   BYTE( 8 Bits) size : Leading b                                         |
// |   CHAR( 8 Bits) size : Leading c                                         |
// |   WORD(16 Bits) size : Leading w                                         |
// |  DWORD(32 Bits) size : Leading d                                         |
// |    int(32 Bits) size : Leading i                                         |
// | DDWORD(64 Bits) size : Leading dd                                        |
// |              Pointer : Leading p                                         |
// |                                                                          |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |                                                                          |
// | Examples:                                                                |
// |                Array : Leading s, (sb = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  01 Apr  2009  Lewis       Initial Version.
//  13 Jul  2011  Lewis       T1000_SYSPACK_V1_0.
//	29 Sept 2011  Lewis       Add Stuff from Qian for T800.
//	19 Mar  2012  Lewis       - Bug Fix: R700-P Undefine issue.
//	                          - Support PR500
//	26 Jun  2012  Lewis       Fix MW_KEY data size.
//	12 Sept 2012  Lewis       Add LPT0.
//	14 Sept 2012  Lewis       Modify MW_TCP_PORT struct for T1000.
//	15 Oct  2012  Lewis       Support CR600
//	05 Dec  2012  Lewis       PR608D v1.3
//	                          - Support New Http Request data structure.
//	08 Mar  2013  Lewis       T1000 v1.4
//	                          - Support barcode scanner.
//	27 May  2013  Lewis       Update Modem configure descriptions.
//	30 Jul  2013  Lewis       T1000 v1.5
//	10 Sept 2013  Lewis       CREON PCI 4.0 V1.0
//  21 Nov  2013  Lewis       Support T1000CT
//  11 Dec  2013  Lewis       Support SP510
//                            - Add 2 file functions.
//                              FRENAMEMW,
//                              FTRUNCATEMW,
//  25 Mar 2014   Lewis       Add HTTP_REQ2
//                            -T1000 V1.7
//                            -T1000CT V1.0
//  29 Jul 2014   Lewis       - Add FFTRUNCATEMW
//                            - Support WIFI.
//                            - T1000 V1.8
//  22 Sep 2014   Lewis       - T700 3G Phase I V1.4
//  29 Dec 2014   Lewis       - CREON4 new http structure.
//  12 Mar 2015   Lewis       - T1000 support TCP/FTP/HTTP Connect2.
//  16 Mar 2015   Lewis       - T800/T700 support TCP/FTP/HTTP Connect2.
//  24 Mar 2015   Lewis       - T700 support TCP/FTP/HTTP Connect2.
//  27 Jul 2015   Lewis       - T1000
//                              - Add LPT paperout bug fix from John.
//                              - Merge dukpt enh from John.
//                              - Support new file create attrib from John.
//  01 Oct 2015   Lewis       - Support A5T1000
//  22 Feb 2016   Pody        - Support T300, T810
//  08 Feb 2017   Harris      - Support A5T1000
//                              - Match "MW_TCP_PORT" structure with T_TCP_PORT.
//                              - Add Struct "MW_NIF_INFO2".
//                              - MMCSD Support Blk Size 4096. (MMC_EXT_DATA_SECT_4KIB)
//                              - Enhance/Bug Fix for WiFi Setup/AP Scan functions for WiFi_ATWINC1500.
//                              - Delay10ms() -> Use Time tick to replace SleepMW() count for timing. SleepMW() can't be used for time counting.
//  22 Mar 2017   Harris      - Fix UDP Test Check Lan cable bug with WiFi.
//                            - Modify WiFi ScanAP ("IO_WIFI_SCAN") return value & Error return value.
//                            - Restore back the EOPTION definition. (i.e. Match "MW_TCP_PORT" structure with T_TCP_PORT)
//  01 Jun 2017   Lewis       - add IO_TCP_SAVE_TLS_SESSION support
//  16 Jun 2017   Lewis       - add IO_HTTP_RESPONSE_HEADER for CREON4
//-----------------------------------------------------------------------------
#ifndef _INC_MIDWARE_H_
#define _INC_MIDWARE_H_

#include <string.h>
#include "stdio.h"
#include "common.h"
#include "system.h"
#include "basecall.h"
#include "hwdef.h"

//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#define MIDWARE_ID          0x0F
#if (T800|T1000|A5T1000|T300|T810)
 #define SUPPORT_320X240      TRUE
#else
 #define SUPPORT_128X64       TRUE
#endif

//***********************************
// => Keyboard
//***********************************
// Key code define
#define MWKEY_ENTER           0x10
#define MWKEY_CANCL           0x11
#define MWKEY_CLR             0x12
#define MWKEY_FUNC            0x13
#define MWKEY_TRANS           0x14
#define MWKEY_SETTLE          0x15
#define MWKEY_FEED            0x16
#define MWKEY_OTHERS          0x17
#define MWKEY_OCARD           0x18
#define MWKEY_BKSP            0x19
#define MWKEY_EPS             0x1A
#define MWKEY_ALPHA           0x1B
#define MWKEY_MENU            0x1C
#define MWKEY_REPRN           0x1D
#define MWKEY_0               0x30
#define MWKEY_1               0x31
#define MWKEY_2               0x32
#define MWKEY_3               0x33
#define MWKEY_4               0x34
#define MWKEY_5               0x35
#define MWKEY_6               0x36
#define MWKEY_7               0x37
#define MWKEY_8               0x38
#define MWKEY_9               0x39
#define MWKEY_00              0x3A
#define MWKEY_UP              0x41
#define MWKEY_DN              0x42
#define MWKEY_LEFT            0x43
#define MWKEY_RIGHT           0x44
#define MWKEY_FUNC1           0x45
#define MWKEY_FUNC2           0x46
#define MWKEY_FUNC3           0x47
#define MWKEY_FUNC4           0x48
#define MWKEY_FUNC5           0x49
#define MWKEY_FUNC6           0x4A
#define MWKEY_FUNC7           0x4B
#define MWKEY_FUNC8           0x4C
#define MWKEY_SELECT          0x4D
#define MWKEY_DOT             0x4E
#define MWKEY_LEFT1           0x4F
#define MWKEY_LEFT2           0x50
#define MWKEY_LEFT3           0x51
#define MWKEY_LEFT4           0x52
#define MWKEY_RIGHT1          0x53
#define MWKEY_RIGHT2          0x54
#define MWKEY_RIGHT3          0x55
#define MWKEY_RIGHT4          0x56
#define MWKEY_FBACK           0x57
#define MWKEY_POWER           0x58
#if (R700)
#define MWKEY_ASTERISK        MWKEY_LEFT
#define MWKEY_SHARP           MWKEY_RIGHT
#else
#define MWKEY_ASTERISK        0x59
#define MWKEY_SHARP           0x5A
#endif
#define MWKEY_LEFT5           MWKEY_UP
#define MWKEY_RIGHT5          MWKEY_DN

// void KbdFlushMW();
#define KbdFlushMW()                os_kbd_getkey()


//***********************************
// => Real time clock
//***********************************
// 14 bytes RTC format = CCYYMMDDHHMMSS
// BOOLEAN RtcGetMW(BYTE * aRTC);
#define RtcGetMW(x)                 os_rtc_get(x)
// BOOLEAN RtcSetMW(BYTE * aRTC);
#define RtcSetMW(x)                 os_rtc_set(x)


//***********************************
// => Timer
//***********************************
// DWORD TimerOpenMW();
#define TimerOpenMW()              os_timer_open()
// void TimerCloseMW(DWORD aHandle);
#define TimerCloseMW(x)            os_timer_close(x)
// void TimerSetMW(DWORD aHandle,DWORD aValue);
#define TimerSetMW(x,y)            os_timer_set(x,y)
// DWORD TimerGetMW(DWORD aHandle);
#define TimerGetMW(x)              os_timer_get(x)


//***********************************
// => Buzzer
//***********************************
#if BUZ_OPEN_V0_SUPPORT
#define BeepOpenMW()               os_beep_open(0)
// BOOLEAN BeepOpenMW(DWORD aPitch);
#define BeepOpen1MW(x)             os_beep_open(x)
#else
#define BeepOpenMW()               os_beep_open()
#define BeepOpen1MW(x)             os_beep_open()
#endif
// void BeepCloseMW();
#define BeepCloseMW()              os_beep_close()
// void BeepMW(WORD aOnTime,WORD aOffTime,WORD aCount);
#define BeepMW(x,y,z)              os_beep_sound(x,y,z)
// void BeepOnOffMW(DWORD aState);
#define BeepOnOffMW(x)             os_beep_state(x)


//***********************************
// => Encryption
//***********************************
// RSA key struct define
struct MW_RSA_KEY {
  DWORD  d_exponent;
  DWORD  d_keysize;
  DWORD  sd_key[64];
  DWORD  d_key_idx;
};		//T_KEY

// SSL
struct MW_MD5_CONTEXT {
  DWORD sd_total[2];
  DWORD sd_state[4];
  BYTE  sb_buffer[64];
};		//T_MD5_CONTEXT

struct MW_MD4_CONTEXT {
  DWORD sd_total[2];
  DWORD sd_state[4];
  BYTE  sb_buffer[64];
};		//T_MD4_CONTEXT

struct MW_SHA1_CONTEXT {
  DWORD sd_total[2];
  DWORD sd_state[5];
  BYTE  sb_buffer[64];
};	//T_SHA1_CONTEXT

struct MW_RC4_STATE {
  DWORD d_x;
  DWORD d_y;
  DWORD sd_m[256];
};	//T_RC4_STATE

// BOOLEAN RsaMW(void *aMsg, struct MW_RSA_KEY *aKey);
#define RsaMW(x,y)                  os_rsa(x, y)
// void Sha1MW(void *aOut, void *aIn,DWORD aLen);
#define Sha1MW(x,y,z)               os_sha1(x, y, z)
// void MD5InitMW(struct MW_MD5_CONTEXT *aContext);
#define MD5InitMW(x)                os_md5_init(x)
// void MD5ProcessMW(struct MW_MD5_CONTEXT *aContext, BYTE *aInput, DWORD aLen);
#define MD5ProcessMW(x,y,z)         os_md5_process(x,y,z)
// void MD5FinishMW(struct MW_MD5_CONTEXT *aContext, BYTE *aDigest);
#define MD5FinishMW(x,y)            os_md5_finish(x,y)
// void Sha1InitMW(MW_SHA1_CONTEXT *aContext);
#define Sha1InitMW(x)               os_sha1_init(x)
// void Sha1ProcessMW(struct MW_SHA1_CONTEXT *aContext, BYTE *aInput, DWORD aLen);
#define Sha1ProcessMW(x,y,z)        os_sha1_process(x,y,z)
// void Sha1FinishMW(struct MW_SHA1_CONTEXT *aContext, BYTE *aDigest);
#define Sha1FinishMW(x,y)           os_sha1_finish(x,y)
// void RC4SetupMW(struct MW_RC4_STATE *aContext, struct MW_RSA_KEY *aInput, DWORD aLen);
#define RC4SetupMW(x,y,z)           os_rc4_setup(x,y,z)
// void RC4EncryptMW(struct MW_RC4_STATE *aContext, BYTE *aInput, DWORD aLen);
#define RC4EncryptMW(x,y,z)         os_rc4_encrypt(x,y,z)
// void MD4InitMW(struct MW_MD4_CONTEXT *aContext);
#define MD4InitMW(x)                os_md4_init(x)
// void MD4ProcessMW(struct MW_MD4_CONTEXT *aContext, BYTE *aInput, DWORD aLen);
#define MD4ProcessMW(x,y,z)         os_md4_process(x,y,z)
// void MD4Finish(struct MW_MD4_CONTEXT *aContext, BYTE *aDigest);
#define MD4FinishMW(x,y)            os_md4_finish(x,y)


//***********************************
// => Thread
//***********************************
#define ThreadCreateMW(x,y)     os_thread_create(x,y)
// BOOLEAN ThreadDeleteMW(DWORD aAddr);
#define ThreadDeleteMW(x)       os_thread_delete(x)
//void ThreadLockMW();
#define ThreadLockMW()          os_thread_lock()
//void ThreadUnLock();
#define ThreadUnlockMW()        os_thread_unlock()
//void ThreadReturnMW();
#define ThreadReturnMW()        os_thread_return()


//***********************************
// => Application
//***********************************
struct MW_APPL_INFO {  //T_APP_INFO
  WORD w_checksum;
  WORD w_disp_checksum;
  BYTE sb_agent_id[8];
  BYTE sb_app_name[13];
  BYTE b_version;
  BYTE b_sub_version;
  BYTE b_type;
  BYTE b_appid;
};
struct MW_APPL_INFO1 {  //T_APP_INFO
  DWORD d_cs32;
  DWORD d_disp_cs32;
  BYTE sb_agent_id[8];
  BYTE sb_app_name[13];
  BYTE b_version;
  BYTE b_sub_version;
  BYTE b_type;
  BYTE b_appid;
  BYTE b_rfu;
  BYTE w_appid;
};
// BOOLEAN AppInfoGetMW(DWORD aAppId, struct MW_APP_INFO *aDat);
#define AppInfoGetMW(x,y)     lib_app_call(MIDWARE_ID, APPINFO_GETMW, x, (DWORD)y, 0)
// BOOLEAN AppInfo1GetMW(DWORD aAppId, struct MW_APP_INFO1 *aDat);
#define AppInfoGet1MW(x,y)    lib_app_call(MIDWARE_ID, APPINFO_GETMW, x, (DWORD)y, 1)
// BOOLEAN IsAppIdExistMW(DWORD aAppId);
#define IsAppIdExistMW(x)     os_app_search_id(x)
// BOOLEAN IsAppExitMW(BYTE *aName, DWORD aCtrl);
#define IsAppExistMW(x,y)     os_app_search_name(x,y)
// BOOLEAN IsAppVirgin(DWORD aAppId);
#define IsAppVirginMW(x)      os_app_get_virgin(x)
// void AppVirginOffMW(DWORD aAppId);
#define AppVirginOffMW(x)     os_app_clear_virgin(x)
//void SwitchAppMW(DWORD aAppId, DWORD aParam1, DWORD aParam2, DWORD aParam3)
#define SwitchAppMW(id,x,y,z) os_call_app(id, x, y, z)


//***********************************
// => Dynamic memory
//***********************************
// void *MallocMW(DWORD aSize);
#define MallocMW(x)           (void *) os_malloc(x)
// void FreeMW(void *aPtr);
#define FreeMW(x)             os_free(x)


//***********************************
// => system utility
//***********************************
//void SprintfMW(const char *aFormatStr[,aArgument....])
#define SprintfMW            	sprintf
// BOOLEAN RandMW(void *aDest);
#define RandMW(x)             os_rand(x)
// void SleepMW(void);
#define SleepMW()             os_sleep()
// void ResetMW(void);
#define ResetMW()             os_reset()

#if defined(os_onbase)
  #define OnBaseMW()            os_onbase()
#else
  #define OnBaseMW()            (1)
#endif

#if defined(os_power_off)
  #define PowerOffMW()          os_power_off()
#elif defined(os_off)
  #define PowerOffMW()          os_off()
#elif defined(os_power_sleep)
  #define PowerOffMW()          os_power_sleep()
#else
  #define PowerOffMW()          ResetMW()
#endif

// DWORD os_free_run(void);
#define FreeRunMW()           os_free_run()

// define system config index
// Note: some K_CF_XXX may not be defined. If compile error, means such config is not supported.
#define   MW_SYSCFG_MAX             K_CF_Max
#define   MW_SYSCFG_SERIAL_NO      	K_CF_SerialNo      // [08] read only
#define   MW_SYSCFG_TID            	K_CF_Tid           // [08] read only
#define   MW_SYSCFG_ETHER_ID        K_CF_EtherID       // [06] read only
#define   MW_SYSCFG_IP             	K_CF_IP            // [04] r/w
#define   MW_SYSCFG_NETMASK        	K_CF_Netmask       // [04] r/w
#define   MW_SYSCFG_GATEWAY        	K_CF_Gateway       // [04] r/w
#define   MW_SYSCFG_DNS1            K_CF_DNS1          // [04] r/w
#define   MW_SYSCFG_DNS2            K_CF_DNS2          // [04] r/w
#define   MW_SYSCFG_UDP_PORT        K_CF_UDPPort       // [02] r/w
#define   MW_SYSCFG_TCP_PORT        K_CF_TCPPort       // [02] r/w
#define   MW_SYSCFG_DHCP_ENABLE    	K_CF_DHCPEnable    // [01] r/w
#define   MW_SYSCFG_LOGO           	K_CF_Logo          // [XX] write only
#define   MW_SYSCFG_TAMPERED        K_CF_Tampered      // [01] r/w
#define   MW_SYSCFG_RDLLINF         K_CF_RdllInf       // [XX] r/w
#define   MW_SYSCFG_HWCFG           K_CF_HwConfig      // [30] read only (Product code)
#define   MW_SYSCFG_MDM_COUNTRY     K_CF_MdmCountry    // [01] r/w
#define   MW_SYSCFG_MDM_TX_PWR      K_CF_MdmTxPwr      // [01] r/w
#define   MW_SYSCFG_LCDCONTRAST     K_CF_LcdContrast   // []
#define   MW_SYSCFG_PPPIP           K_CF_PppFixIP      // [04] r/w
#define   MW_SYSCFG_MDM_DT_PWR      K_CF_MdmDTPwr      // [01] r/w
#define   MW_SYSCFG_MDM_DTMF_PWR    K_CF_MdmDTMFPwr    // [01] r/w
#define   MW_SYSCFG_MDMU80          K_CF_MdmU80        // [02] r/w
#define   MW_SYSCFG_LCDB            K_CF_LcdBrightness // [01] read only
#define   MW_SYSCFG_LCDTIMEOUT      K_CF_LcdTimeout    // [01] read only
#define   MW_SYSCFG_SIMSEL          K_CF_SimSelect     // [01] read only
#define   MW_SYSCFG_KBDBL           K_CF_KbdBL         // [01] r/w

// DWORD config_length(DWORD a_idx);
#define SysCfgLenMW(x)    		sys_func1(OS_FN_CONFIG_LENGTH,(DWORD)(x))
// BOOLEAN config_read(DWORD a_idx,BYTE * a_dest);
#define GetSysCfgMW(x,y)    	sys_func2(OS_FN_CONFIG_READ,(DWORD)(x),(DWORD)(y))
// BOOLEAN config_write(DWORD a_idx,BYTE * a_src);
#define PutSysCfgMW(x,y)   		sys_func2(OS_FN_CONFIG_WRITE,(DWORD)(x),(DWORD)(y))
// BOOLEAN config_update(void);
#define UpdSysCfgMW()     		sys_func0(OS_FN_CONFIG_UPDATE)

struct MW_NIF_INFO {
  DWORD d_ip;
  DWORD d_netmask;
  DWORD d_gateway;
  DWORD s_dns[2];
};

struct MW_NIF_INFO2 {
  IPaddr d_ip;             // stored in big endian
  IPaddr d_netmask;        // stored in big endian
  IPaddr d_gateway;        // stored in big endian
  IPaddr s_dns[2];         // stored in big endian
  BYTE MACAddress[6];
  BYTE DHCP_Enabled;
  BYTE nif_type;            // net interface type, defined in system.h
  unsigned int dev_id;
  unsigned int bus_id;
  char name[64];
};

#define MW_NIF_ETHERNET        K_NIF_ETHERNET
#define MW_NIF_PPP             K_NIF_PPP
#define MW_NIF_WIFI            K_NIF_WIFI

// DWORD NetInfo(DWORD aNif, MW_NIF_INFO *aNif)
#ifdef bs_ppp_get_nif //PC1210 add for support WCDMA
	#define NetInfoMW(x,y)         base_func2(BS_FN_NET_NIF_INFO, (DWORD)((x == MW_NIF_PPP) ? bs_ppp_get_nif() : x), (DWORD)(y))
	#define NetInfo2MW(x,y)        (int)base_func2(BS_FN_NET_NIF_INFO2, (DWORD)((x == MW_NIF_PPP) ? bs_ppp_get_nif() : x), (DWORD)(y))
#else
	#define NetInfoMW(x,y)         base_func2(BS_FN_NET_NIF_INFO,(DWORD)(x),(DWORD)(y))
	#define NetInfo2MW(x,y)        (int)base_func2(BS_FN_NET_NIF_INFO2,(DWORD)(x),(DWORD)(y))
#endif


//***********************************
// => file system
//***********************************
// seek location for fSeekMW()
#define MW_FSEEK_SET         0
#define MW_FSEEK_CUR         (DWORD)-2
#define MW_FSEEK_END         (DWORD)-1

// create file
#define MW_FCREATE_SHARE            1       // bit 0
#define MW_FCREATE_ENCRYPT          2       // bit 1
#define MW_FCREATE_USR_CTRL         4       // bit 2

struct MW_FILE_HDR {
  DWORD d_attr;
  DWORD d_filelen;
  BYTE  sb_name[20];
  BYTE  sb_create_date[7];
  BYTE  sb_last_update[7];
};


//-----------------------------------------------------------------------------
//   Standard IO Device Defines
//-----------------------------------------------------------------------------
// Device file name
#define DEV_MSR1 "/dev/msr1"
#define DEV_ICC1 "/dev/icc1"
#define DEV_SD1  "/dev/sd1"
#define DEV_LPT0 "/dev/lpt0"
#define DEV_LPT1 "/dev/lpt1"
#define DEV_LPT2 "/dev/lpt2"
#define DEV_AUX0 "/dev/aux0"
#define DEV_AUX1 "/dev/aux1"
#define DEV_AUX2 "/dev/aux2"
#define DEV_TCP1 "/dev/tcp1"
#define DEV_UDP1 "/dev/udp1"
#define DEV_ICMP "/dev/icmp"
#define DEV_PPP1 "/dev/ppp1"
#define DEV_FTP1 "/dev/ftp1"
#define DEV_MDM1 "/dev/mdm1"
#define DEV_CDC1 "/dev/cdc1"
#define DEV_USBS "/dev/usbs"
#define DEV_DNS1 "/dev/dns1"
#define DEV_HTTP "/dev/http"
#define DEV_GPRS "/dev/gprs"
#define DEV_ECR1 "/dev/ecr1"
#define DEV_ECR2 "/dev/ecr2"
#define DEV_ECR3 "/dev/ecr3"
#define DEV_LAN1 "/dev/lan1"
#define DEV_BRDR "/dev/brdr"
#define DEV_CLIC "/dev/clic"
#define DEV_CLED "/dev/cled"
#define DEV_WIFI "/dev/wifi"

// standard IO For OpenMW
#define MW_CLOSE            0x0000
#define MW_OPEN             0x0001
#define MW_RDONLY           0x0002
#define MW_WRONLY           0x0004
#define MW_RDWR             0x0006
#define MW_SEEK             0x0008
#define MW_CREAT            0x0100
#define MW_TRUNC            0x0200
#define MW_APPEND           0x0300

// standard IO For LSeekMW
#define MW_SEEK_SET         0
#define MW_SEEK_CUR         1
#define MW_SEEK_END         2

// standard IO Operation Error Code
#define MWESUCCESS        0
#define MWEINVFD         -1
#define MWEINVLEN        -2
#define MWEINVOPR        -3
#define MWEOPENED        -4
#define MWECLOSED        -5
#define MWEUNDEFINE      -6
#define MWEOFFBASE       -7

// => MSR define
//***********************************
// MSR IOCtl
#define IO_MSR_CFG          0x0001
#define IO_MSR_STATUS       0x0002
#define IO_MSR_RXLEN        0x0003
#define IO_MSR_RESET        0x0004

// Msr Config MODE
#define MW_TRACK2        0x01
#define MW_TRACK1        0x02
#define MW_TRACK3        0x04

// MSR status
#define MW_TRACK2_ERR    0x0100
#define MW_TRACK1_ERR    0x0200
#define MW_TRACK3_ERR    0x0400

// MSR special chars
#define START2_SENTINEL 0x3B
#define START3_SENTINEL 0x3B
#define START1_SENTINEL 0x25
#define END_SENTINEL    0x3F
#define SEPERATOR2      0x3D

// MSR configuration
struct MW_MSR_CFG {
  BYTE b_mode;
  BYTE b_speed;
};

// Track 1 data
#ifndef K_Trk1BufLen
  #define K_Trk1BufLen          158
#endif
struct MW_TRK1BUF {
  BYTE b_len;
  BYTE sb_content[K_Trk1BufLen];
};

// Track 2 data
#ifndef K_Trk2BufLen
  #define K_Trk2BufLen          80
#endif
struct MW_TRK2BUF {
  BYTE b_len;
  BYTE sb_content[K_Trk2BufLen];
};

// Track 3 data
#ifndef K_Trk3BufLen
  #define K_Trk3BufLen          214
#endif
struct MW_TRK3BUF {
  BYTE b_len;
  BYTE sb_content[K_Trk3BufLen];
};

// => ICC define
//***********************************
// ICC IOCtl
#define IO_ICC_SELECT       0x0001
#define IO_ICC_INSERTED     0x0002
#define IO_ICC_ON           0x0003
#define IO_ICC_OFF          0x0004
#define IO_ICC_CHANGE       0x0005
#define IO_ICC_T1           0x0006
#define IO_ICC_MEMON        0x0007
#define IO_ICC_MEMOFF       0x0008
#define IO_ICC_MEMHIST      0x0009
#define IO_ICC_MEMT0        0x000A

// ICC type T1
struct MW_ICC_T1 {	// T_T1
  BYTE *pb_sad_buf;
  WORD  w_sad_len;
  BYTE  b_sad;
  BYTE  b_rfu1;
  BYTE *pb_dad_buf;
  WORD  w_dad_len;
  BYTE  b_dad;
  BYTE  b_rfu2;
};

// ICC type T0
struct MW_ICC_T0 {	// T_T0
  BYTE  b_output;
  BYTE  b_class;
  BYTE  b_command;
  BYTE  b_p1;
  BYTE  b_p2;
  BYTE  b_p3;
  WORD  w_response;
  BYTE *pb_mblk;
};

// => MMC/SD define
//***********************************
// MMC/SD IOCtl
#define IO_SD_BLKSIZE       0x0001
#define IO_SD_PSTATUS       0x0002
#define IO_SD_GETCID        0x0003
#define IO_SD_GETCSD        0x0004
#define IO_SD_CARDSIZE      0x0005

// => LPT define
//***********************************
// For IOCtlMW
// LPT
#define IO_LPT_RESET        0x0001
#define IO_LPT_COMMAND      0x0002

// LPT STATE (input)
#if (TIRO|PR608|PR500|SP510)
#define K_LptStatusRead         0
#define K_LptStatusRestart      1
#define K_LptStatusResetError   1
#define K_LptStatusReset        2
#endif

#ifdef K_LptStatusRead
  #define LPT_STS_GET           K_LptStatusRead
#endif
#ifdef K_LptStatusRestart
  #define LPT_STS_ERESTART      K_LptStatusRestart
#endif
#ifdef K_LptStatusResetError
  #define LPT_STS_ERESET        K_LptStatusResetError
#endif
#ifdef K_LptStatusReset
  #define LPT_STS_RESET         K_LptStatusReset
#endif

#define MW_LPT_BOLD_OFF         "\x1B\x42\x30"  //ESC B
#define MW_LPT_BOLD_ON          "\x1B\x42\x31"
#define MW_LPT_UNDERLINE_OFF    "\x1B\x55\x30"  //ESC U
#define MW_LPT_UNDERLINE_ON     "\x1B\x55\x31"
#define MW_LPT_2WIDTH_OFF       "\x1B\x57\x30"  //ESC W
#define MW_LPT_2WIDTH_ON        "\x1B\x57\x31"
#define MW_LPT_FEED_N           "\x1B\x4A"      //ESC J feed paper n lines (insert number N after command string)
#define MW_LPT_GRAPHIC          "\x1B\x4B"      //ESC K print graphic (insert [height][width][image])
#define MW_LPT_RESET            "\x1B\x40"      //ESC @ reset
#define MW_LPT_HALFDOT_OFF      "\x1B\x44\x30"  //ESC D
#define MW_LPT_HALFDOT_ON       "\x1B\x44\x31"
#define MW_LPT_FONT_SEL			"\x1B\x46"		//ESC F

#if (TMLPT_SUPPORT)
#define MW_LPT_INVERTED_OFF     "\x1B\x49\x30"  //ESC I
#define MW_LPT_INVERTED_ON      "\x1B\x49\x31"
#define MW_LPT_2HEIGHT_OFF      "\x1B\x48\x30"  //ESC H
#define MW_LPT_2HEIGHT_ON       "\x1B\x48\x31"
#define MW_LPT_FONT_NORMAL      "\x1B\x46\x30"  //ESC F 16(H)x12(W)
#define MW_LPT_FONT_SMALL       "\x1B\x46\x31"  //      16(H)x8(W)
#define MW_LPT_FONT_ENLARGED    "\x1B\x46\x32"  //      16(H)x12(W)
#define MW_LPT_FONT_DEFAULT     "\x1B\x46\x33"  //      16(H)x8(W)
#define MW_LPT_FONT_SP_NORMAL   "\x1B\x46\x34"  //      size returned by SPFONT
#define MW_LPT_FONT_SP_T        "\x1B\x46\x35"  //      size returned by SPFONT
#define MW_LPT_FONT_SP_SP       "\x1B\x46\x36"  //      size returned by SPFONT
#endif

#if (LPT_SUPPORT)
//#define MW_LPT_2HEIGHT_ON       "\x1B\x48"      //ESC H
#define MW_LPT_GRID_OFF         "\x1B\x47\x30"  //ESC G
#define MW_LPT_GRID_ON          "\x1B\x47\x31"
#define MW_LPT_HALFHEIGHT       "\x1B\x54"      //ESC T Font height control for Double Byte Character
  #define MW_LPT_HH_CHINESE_OFF   "\x1B\x54\x30"    // Chinese
  #define MW_LPT_HH_CHINESE_ON    "\x1B\x54\x31"
  #define MW_LPT_HH_SP_OFF        "\x1B\x54\x32"    // SP Font
  #define MW_LPT_HH_SP_ON         "\x1B\x54\x33"
#endif

// LPT status output
#if (TIRO|PR608|PR500|SP510)
  #define K_LptOk                 0
  #define K_ErrTempHigh           0xFF
  #define K_ErrPaperout           3
  #define K_ErrPrinting           0xFE
  #define K_ErrCarriageStop       0xFD
  #define K_LptTempHigh           0x8000
  #define K_LptPaperout           0x0200
  #define K_LptPrinting           0x0800
  #define K_LptFinish             0x1000
  #define K_LptErrVoltLow         0x800000
#endif
#if (R700)
  #define K_ErrVoltLow            10
  #define K_ErrPrinting           11
  #define K_LptErrVoltLow         0x800000
#endif
// short status output
#ifdef K_LptOk
  #define MW_LPT_STAT_START_FINISH        K_LptOk
#endif
#ifdef K_LptErrTempHigh
  #define MW_LPT_STAT_HIGH_TEMP           K_LptErrTempHigh
#elif defined K_ErrTempHigh
  #define MW_LPT_STAT_HIGH_TEMP           K_ErrTempHigh
#endif
#ifdef K_LptErrVoltLow
  #define MW_LPT_STAT_LOW_VOL             K_LptErrVoltLow
#elif defined K_ErrVoltLow
  #define MW_LPT_STAT_LOW_VOL             K_ErrVoltLow
#endif
#ifdef K_LptErrPaperout
  #define MW_LPT_STAT_PAPER_OUT           K_LptErrPaperout
#elif defined K_ErrPaperout
  #define MW_LPT_STAT_PAPER_OUT           K_ErrPaperout
#endif
#ifdef K_LptErrPrint
  #define MW_LPT_STAT_UNDEFINE_ERR        K_LptErrPrint
#elif defined K_ErrPrinting
  #define MW_LPT_STAT_UNDEFINE_ERR        K_ErrPrinting
#elif defined K_ErrPrint
  #define MW_LPT_STAT_UNDEFINE_ERR        K_ErrPrint
#endif
#define MW_LPT_STAT_NOT_START             4
#define MW_LPT_STAT_IN_PROGRESS           5
#define MW_LPT_STAT_LOW_VOL_RESUME        6
// long status output
#ifdef K_LptTempHigh
  #define MW_LPT_TEMP_HIGH                K_LptTempHigh
#endif
#ifdef K_LptPaperout
  #define MW_LPT_PAPER_OUT                K_LptPaperout
#endif
#ifdef K_LptLowVolt
  #define MW_LPT_LOW_VOLT                 K_LptLowVolt
#endif
#ifdef K_LptPrinting
  #define MW_LPT_PRINTING                 K_LptPrinting
#endif
#ifdef K_LptFinish
  #define MW_LPT_FINISH                   K_LptFinish
#endif

// => AUX define
//***********************************
// For IOCtlMW
#define IO_AUX_CFG          0x0001
#define IO_AUX_TX_RDY       0x0002
#define IO_AUX_RX_RDY       0x0003

//aux communcation mode
#define MW_AUX_AUTO           0x00
#define MW_AUX_STX            0x01
#define MW_AUX_ENAC           0x02
#define MW_AUX_SOH_LRC        0x03
#define MW_AUX_SOH_CRC        0x04
#define MW_AUX_NO_PARITY      0x05    // default
#define MW_AUX_EVEN_PARITY    0x06
#define MW_AUX_ODD_PARITY     0x07

// aux speed
#define MW_AUX_9600           0       // default
#define MW_AUX_2400           1
#define MW_AUX_4800           2
#define MW_AUX_19200          3
#define MW_AUX_28800          4
#define MW_AUX_38400          5
#define MW_AUX_57600          6
#define MW_AUX_115200         7
#define MW_AUX_230400         8

struct MW_AUX_CFG {
  BYTE b_len;
  BYTE b_mode;
  BYTE b_speed;
  BYTE b_rx_gap;
  BYTE b_rsp_gap;
  BYTE b_tx_gap;
  BYTE b_retry;
};

// => TCP define
//***********************************
// For IOCtlMW
#define IO_TCP_CONNECT          0x0001
#define IO_TCP_DISCONNECT       0x0002
#define IO_TCP_CONNECT2         0x0003
#define IO_TCP_SAVE_TLS_SESSION 0x0004

// tcp mode
#define MW_TCP_PPP_MODE         0x01
#define MW_TCP_NO_CLOSE_WAIT    0x02
#define MW_TCP_SSL_MODE         0x04
#define MW_FTP_SSL_MODE         0x08

// tcp status - MW_TCP_STATUS
#define MW_TCP_STAT_IDLE            0
#define MW_TCP_STAT_CONNECTING      1
#define MW_TCP_STAT_CONNECTED       2
#define MW_TCP_STAT_CLOSING         3
#define MW_TCP_STAT_CLOSED          4
#define MW_TCP_STAT_SSL_CONNECTED   0x8000
#define MW_TCP_STAT_ALERT           0x4000

// tcp status selector
#define MW_TCP_STATUS               0

// tcp status select for tcp tx buffer left
#define MW_TCP_TXBUF_LEFT       1

// tcp status select for tcp rx buffer use
#define MW_TCP_RXBUF_USED       2

// tcp status select to tcb error status
#define MW_TCP_ERR_STATUS       3

// tcp connect parameter define
struct MW_TCP_PORT {
  DWORD d_ip;
  WORD  w_port;
  BYTE  b_sslidx;
  BYTE  b_option;
  #if (T1000|CREON4|PR608D)
  BYTE    b_certidx;
  BYTE    b_keyidx;
  BYTE    b_eoption;
  BYTE    s_rfu[1];  // should fill 0's
  #endif
};

// tcp connect2 parameter define
struct MW_TCP_PORT2 {
  DWORD d_size;         // must equal sizeof(struct MW_TCP_PORT2)
  DWORD d_xctrl;        // extra control handle
  DWORD d_ip;
  WORD  w_port;
  BYTE  b_sslidx;
  BYTE  b_option;
  BYTE  b_certidx;
  BYTE  b_keyidx;
  BYTE  b_eoption;
  BYTE  s_rfu[1];  // should fill 0's
};

// => UDP define
//***********************************
// For IOCtlMW
#define IO_UDP_CONNECT          0x0001
#define IO_UDP_DISCONNECT       0x0002

// udp mode
#define MW_UDP_SERVER_MODE      0x01
#define MW_UDP_PPP_MODE         0x80

// udp status selector
#define MW_UDP_STATUS               0

// udp status - MW_UDP_STATUS
#define MW_UDP_STAT_IDLE            0
#define MW_UDP_STAT_CONNECTED       1

// udp connect parameter define
struct MW_UDP_PORT {
  DWORD d_ip;
  WORD  w_port;
  BYTE  b_mode;
};

// => ICMP define
//***********************************
struct MW_ICMP_PORT {
  DWORD d_ip;
  BYTE  b_option;
};
// For IOCtlMW
#define IO_ICMP_CONFIG           0x0001
#define IO_ICMP_CONNECT          0x0002
#define IO_ICMP_DISCONNECT       0x0003

// For Option
#define MW_ICMP_PPP_MODE              1

// => FTP define
//***********************************
// For IOCtlMW
#define IO_FTP_CONNECT          0x0001
#define IO_FTP_DISCONNECT       0x0002
#define IO_FTP_CMD_SEND         0x0003
#define IO_FTP_CMD_READ         0x0004
#define IO_FTP_CONNECT2         0x0005

// ftp status - MW_FTP_STATUS
#define MW_FTP_STAT_IDLE            0
#define MW_FTP_STAT_CONNECTING      1
#define MW_FTP_STAT_CONNECTED       2
#define MW_FTP_STAT_OPENING         3
#define MW_FTP_STAT_OPENED          4
#define MW_FTP_STAT_DATA_CONN       5
#define MW_FTP_STAT_DATA_CONNED     6
#define MW_FTP_STAT_CLOSING         7
#define MW_FTP_STAT_SSL_CONNECTED   0x8000
#define MW_FTP_STAT_ALERT           0x4000

// ftp status selector
#define MW_FTP_STATUS               0
#define MW_FTP_CMD_STATUS           1
#define MW_FTP_DATA_STATUS          2
#define MW_FTP_CMD_TXBUF_LEFT       3
#define MW_FTP_DATA_TXBUF_LEFT      4
#define MW_FTP_CMD_RXBUF_USED       5
#define MW_FTP_DATA_RXBUF_USED      6

// ftp disconnect
#define MW_FTP_DATA_CHNL          0
#define MW_FTP_CMD_CHNL           1


// => Modem define
//***********************************
// For IOCtlMW
enum {
  IO_MDM_CONNECT             = 1,
  IO_MDM_DISCONNECT             ,
  IO_MDM_PHONE_DETECT_ON        ,
  IO_MDM_PHONE_DETECT_OFF       ,
  IO_MDM_PHONE_LINE             ,
  IO_MDM_PHONE_DIAL             ,
  IO_MDM_CID_DETECT             ,
  IO_MDM_CID_READ               ,
  IO_MDM_DIALSTR                ,
  IO_MDM_DIALCHAR               ,
  IO_MDM_GET_STATUS             ,
  IO_MDM_GET_SPEED              ,
  IO_MDM_ONLINE                 ,
  IO_MDM_HSET_STS               ,
  IO_MDM_CID_STATE              ,
  IO_MDM_CID_STATUS             ,
  IO_MDM_DISC_STATUS            ,
  IO_MDM_DIAL_STATUS            ,
  IO_MDM_SET_OPR_MODE           ,
  IO_MDM_GET_OPR_MODE           ,
  IO_MDM_RINGIN                 ,
  IO_MDM_PLAY_END               ,
  IO_MDM_REC_END                ,
  IO_MDM_PLAY_VOL               ,
  IO_MDM_SPKPH_STATE            ,
  IO_MDM_SPKPH_STATUS           ,
  IO_MDM_ANS                    ,
  IO_MDM_PB_GAIN                ,
  IO_MDM_REC_GAIN               ,
  IO_MDM_SPR_GAIN               ,
  IO_MDM_MPH_GAIN               ,
  IO_MDM_PLAY                   ,
  IO_MDM_REC                    ,
  IO_MDM_PBREC_ABORT            ,
  IO_MDM_CONNECT_66             ,
};

// For Protocol
#define MW_SDLC                     1
#define MW_ASYNC                    2

// For Mode
#define MW_CCITT                    1
#define MW_BELL                     2

// For Speed
#if (MDMSPD_V1_SUPPORT)
  #define MW_300                      0
  #define MW_1200                     1
  #define MW_F2400                    2
  #define MW_2400                     3
  #define MW_9600                     4
  #define MW_AUTO                     5
  #define MW_AUTO_42                  6
  #define MW_AUTO_42_CIR              7
  #define MW_14400                    8
  #define MW_33600                    9
  #define MW_56000                   10
#elif (MDMSPD_V3_SUPPORT|MDMSPD_V2_SUPPORT|MDMSPD_V0_SUPPORT|SP510|CR600)
  #define MW_300                      0
  #define MW_1200                     1
  #define MW_2400                     2
  #define MW_F2400                    4
  #define MW_9600                     3
  #define MW_AUTO                     4
  #define MW_AUTO_42                  5
  #define MW_AUTO_42_CIR              6
  #define MW_14400                    7
  #define MW_33600                    8
  #define MW_56000                    9
#else
  NEW_TERMINAL_HERE{}
#endif
#define MW_UNIONPAY_STX            0x80

// For detect mode
#define MW_BUSY_TONE_DETOFF         0x01
#define MW_NO_LINE_CHECK            0x02
#define MW_BLIND_DIALING            0x04
#define MW_SHORT_DIALTONE           0x08

// For Line Status
#define MW_LINE_IDLE               0
#define MW_LINE_DIALING_P1         1
#define MW_LINE_DIALING_P2         2
#define MW_LINE_CONNECTING_P1      3
#define MW_LINE_CONNECTING_P2      4
#define MW_LINE_CONNECTED          5
#define MW_LINE_CD_LOST            0x80
#define MW_LINE_HOST_OFFLINE       0x81
#define MW_LINE_INACT_TIMEOUT      0x82
#define MW_LINE_CONN_FAIL          0x83
#define MW_LINE_CONN_TIMEOUT       0x84
// sticky status, cleared after being read
#define MW_LINE_OCCUPIED           0x00000100
#define MW_LINE_NO_LINE            0x00000200
#define MW_LINE_NO_DAILTONE        0x00000400
#define MW_LINE_WRONG_NO           0x00000800
#define MW_LINE_BUSY               0x00001000
#define MW_LINE_NO_CD              0x00002000
#define MW_LINE_CONNECT_FAIL       0x00004000
#define MW_LINE_MDM_BUSY           0x00008000
#define MW_LINE_MSG_TXED           0x00010000
#define MW_LINE_WAIT_DISC          0x00020000
#define MW_LINE_WAIT_DIAL          0x00040000
#define MW_LINE_READY              0x00080000

#if (T1000CT|T1000|R700)
#define MW_TELNO_LEN             (34+32)
#else
#define MW_TELNO_LEN             (34)
#endif

struct MW_MDM_PORT {
  BYTE b_protocol;               /* Default SDLC */
                                 /* 0 - Visa1 */
                                 /* 1 - SDLC  */
                                 /* 2 - Async */
  BYTE b_ccitt;                  /* 0 - Bell */
                                 /* 1 - CCITT */
  BYTE b_speed;                  //R700: 0 - 300bps, 1 - 1200bps, 2 - 2400bps fast, 3 - 2400bps, 4 - 9600bps, 5 - auto
                                 // 6 - auto with V.42 & MNP enable
                                 // 7 - auto with V.42 & MNP enableand using circular buffer supporting Unionpay STX message format in RX
                                 // 8 - 14400bps, 9 - 33600bps, 10 - 56000bps
                                 // bit 7 set for support Unionpay STX message format
								 //
                                 //Others: 0 - 300bps, 1 - 1200bps, 2 - 2400bps, 3 - 9600bps, 4 - auto
                                 // 5 - auto with V.42 & MNP enable
                                 // 6 - auto with V.42 & MNP enableand using circular buffer supporting Unionpay STX message format in RX
                                 // 7 - 14400bps, 8 - 33600bps, 9 - 56000bps
                                 // bit 7 set for support Unionpay STX message format
  BYTE b_inactivity_tval;        /* in unit of 2.5 sec */
  BYTE b_redial_pause_tval;      /* Pause time between redial in 100 msec*/
  BYTE b_ptel_len;               /* Primary telno length */
  BYTE s_ptel[MW_TELNO_LEN];     /* Primary telno */
  BYTE b_pconn_limit;            /* # of connection attempt */
  BYTE b_pconn_tval;             /* Time allowed to wait CD in sec */
  BYTE b_stel_len;               /* Secondary telno length */
  BYTE s_stel[MW_TELNO_LEN];     /* Secondary telno */
  BYTE b_sconn_limit;            /* # of connection attempt */
  BYTE b_sconn_tval;             /* Time allowed to wait CD in sec */
  BYTE b_cd_on_qualify_tval;     /* cd on qualify time in 0.25 sec,default 3 */
  BYTE b_cd_off_qualify_tval;    /* cd off qualify time in 0.25 sec,default 4 */
  BYTE b_message_qualify_tval;   /* Message qualify time in 10msec, default 10 */
  BYTE b_wait_online_tval;       /* wait for link online in sec, default 15 */
  BYTE b_tone_duration;          /* tone duration in 10msec */
  BYTE b_holdline_tval;          /* hold line time in second */
  BYTE b_detect_mode;            /* bit 0 - 0 = busy tone detection */
                                 /*         1 = disable busy tone detection */
                                 /* bit 1 - 0 = check line before dial */
                                 /*         1 = no check line */
                                 /* bit 2 - 0 = blind dialing disable */
                                 /*         1 = blind dialing enable */
                                 /* bit 3 - 0 = normal dial tone duration */
                                 /*         1 = short dial tone duration */
};

struct MW_MDM_PORT_66 {
  BYTE b_protocol;               /* Default SDLC */
                                 /* 0 - Visa1 */
                                 /* 1 - SDLC  */
                                 /* 2 - Async */
  BYTE b_ccitt;                  /* Default CCITT */
  BYTE b_speed;                  //R700: 0 - 300bps, 1 - 1200bps, 2 - 2400bps fast, 3 - 2400bps, 4 - 9600bps, 5 - auto
                                 // 6 - auto with V.42 & MNP enable
                                 // 7 - auto with V.42 & MNP enableand using circular buffer supporting Unionpay STX message format in RX
                                 // 8 - 14400bps, 9 - 33600bps, 10 - 56000bps
                                 // bit 7 set for support Unionpay STX message format
                                 //
                                 //Others: 0 - 300bps, 1 - 1200bps, 2 - 2400bps, 3 - 9600bps, 4 - auto
                                 // 5 - auto with V.42 & MNP enable
                                 // 6 - auto with V.42 & MNP enableand using circular buffer supporting Unionpay STX message format in RX
                                 // 7 - 14400bps, 8 - 33600bps, 9 - 56000bps
                                 // bit 7 set for support Unionpay STX message format
  BYTE b_inactivity_tval;        /* in unit of 2.5 sec */
  BYTE b_redial_pause_tval;      /* Pause time between redial in 100 msec*/
  BYTE b_ptel_len;               /* Primary telno length */
  BYTE s_ptel[66];               /* Prime telno */
  BYTE b_pconn_limit;            /* # of connection attempt */
  BYTE b_pconn_tval;             /* Time allowed to wait CD in sec */
  BYTE b_stel_len;               /* Secondary telno length */
  BYTE s_stel[66];               /* Secondary telno */
  BYTE b_sconn_limit;            /* # of connection attempt */
  BYTE b_sconn_tval;             /* Time allowed to wait CD in sec */
  BYTE b_cd_on_qualify_tval;     /* cd on qualify time in 0.25 sec,default 3 */
  BYTE b_cd_off_qualify_tval;    /* cd off qualify time in 0.25 sec,default 4 */
  BYTE b_message_qualify_tval;   /* Message qualify time in 10msec, default 10 */
  BYTE b_wait_online_tval;       /* wait for link online in sec, default 15 */
  BYTE b_tone_duration;          /* tone duration in 10msec */
  BYTE b_holdline_tval;          /* hold line time in second */
  BYTE b_detect_mode;            /* bit 0 - 0 = busy tone detection */
                                 /*         1 = disable busy tone detection */
                                 /* bit 1 - 0 = check line before dial */
                                 /*         1 = no check line */
                                 /* bit 2 - 0 = blind dialing disable */
                                 /*         1 = blind dialing enable */
                                 /* bit 3 - 0 = normal dial tone duration */
                                 /*         1 = short dial tone duration */
};

// for cid detect mode
#define MW_CID_MODE_NONE             0
#define MW_CID_MODE_USA              1
#define MW_CID_MODE_UK               2
#define MW_CID_MODE_JAPAN            3
#define MW_CID_DEFAULT               1

// for phone detection status
#define MW_PHONE_ONHOOK           0
#define MW_PHONE_OFFHOOK          1
#define MW_PHONE_NOLINE           2
#define MW_PHONE_DETECTNOTENABLE  3


// IO_MDM_GET_STATUS

// IO_MDM_PLAY
#define MW_MDM_PLAYRAM   0
#define MW_MDM_PLAYFILE  1
struct PLAYBACK {
  BYTE bMode;
  BYTE *pbSrc;          // full path filename or pointer to ram
  DWORD dwOffset;
  DWORD dwLen;
};
// IO_MDM_RECORD
struct MDM_RECORD {
  BYTE *pbDat;          // pointer record buffer
  DWORD dwLen;          // size of record buffer
  DWORD dwCtrl;         // Control for driver to stop recording.
                        // bit  0-15 : silent time control in unit of 100ms,
                        //             0 => use default
                        // bit 16-31 : silent level control,
                        //             0 => use default,
                        //             0xffff => auto that use first 4 data average for silent reference
};

// IO_MDM_GET_STATUS
#define MWMDM_Ready              0
#define MWMDM_Connected          1
#define MWMDM_RingIn             2
#define MWMDM_NoCarrier          3
#define MWMDM_NoDialTone         4
#define MWMDM_LineInUse          5
#define MWMDM_NoLine             6
#define MWMDM_Busy               7
#define MWMDM_Intrusion          8
#define MWMDM_Dialing            9
#define MWMDM_Cid                10
// Voice mode state
#define MWMDM_VoiceReady            0x20
#define MWMDM_VoiceDialing          0x21
#define MWMDM_VoiceSpPh             0x22    // speaker phone
#define MWMDM_VoiceFrSpPh           0x23    // speaker phone then handset lift up
#define MWMDM_VoiceHs               0x24    // normal handset dialing
#define MWMDM_VoiceSpPhFrHs         0x25    // handset dialing changes to speaker phone
#define MWMDM_VoiceRecFrMic         0x26
#define MWMDM_VoicePlybkToSp        0x27
#define MWMDM_VoiceAutoAns          0x28
#define MWMDM_VoicePlybkToLineSp    0x29
#define MWMDM_VoiceRecFRLineSp      0x2A

// => PPP define
//***********************************
// For IOCtlMW
enum {
  IO_PPP_SET_DEVICE         ,
  IO_PPP_SET_MTU            ,
  IO_PPP_SET_MODE           ,
  IO_PPP_GET_SPEED          ,
  IO_PPP_SET_SPEED          ,
  IO_PPP_GET_PHASE          ,
  IO_PPP_SET_LOGINID        ,
  IO_PPP_SET_DIAL_SCRIPT    ,
  IO_PPP_SET_ID_STR         ,
  IO_PPP_SET_LOGIN_SCRIPT   ,
  IO_PPP_SET_PHONE          ,
  IO_PPP_SET_DIAL_PARAM     ,
  IO_PPP_SET_REDIAL         ,
  IO_PPP_SET_RECONNECT      ,
  IO_PPP_LOGIN_FINISH       ,
  IO_PPP_SET_TIMEOUT        ,
  IO_PPP_UPTIME             ,
  IO_PPP_SET_ALIVETIME      ,
  IO_PPP_CONNECT            ,
  IO_PPP_DISCONNECT         ,
  IO_PPP_LOGIN_READ         ,
  IO_PPP_LOGIN_WRITE        ,
};

//Expect-Send struct
//It is possible to specify some special `values' in the expect-send script as follows:
//  \c  When used as the last character in a `send' string, this indicates that a newline
//      should not be appended.
//	\d  When the chat script encounters this sequence, it delays two seconds.
//  \p  When the chat script encounters this sequence, it delays for one quarter of a second.
//  \T  This is replaced by the current phone number (see ``set phone'' below).
//  \P  This is replaced by the current authkey value (see ``set authkey'' above).
//  \U  This is replaced by the current authname value (see ``set authname'' above).
//  \j<r1[r2][r3]...>  Reletive jump to other expect_send pair.
//      if jump to the location grater than total expect-send, script done.
//      if jump to location less than 0, script failed.
//
// script contains "expect-send" pair strings for modem communication
//
struct MW_EXPECT_SEND {
  char ** p_expect;     // expect receive strings, the last one must NULL
  WORD  w_min_len;      // min. expect len
  WORD  w_max_len;      // max. expect len
  signed char * p_send; // reply string
  char * p_abort;       // expect receive abort string
  short w_timeout;      // timeout for expect
  BYTE  b_retry;        // no. of retry
  char  b_flags;        // flags
};

#define MW_DEFAULT_SCRIPTS  0xFF    // use default script when no of pair = 0xff

struct MW_PPP_DATA {
  DWORD device;                             // Device id
  DWORD dev_speed;                          // Device Connection Speed
  DWORD id;
  DWORD mtu;                                // Maximun Receive Unit Value
  DWORD mode;                               // PPP mode
  DWORD connect_speed;                      // PPP connection speed
  char  *username;                          // Login User name
  char  *userpwd;                           // Login User password
  struct MW_EXPECT_SEND *dial_scripts;      // Dialup Scirpts
  DWORD  dial_pair;                         // Dialup scripts pairs,  0xFF => Use Default
  char  *id_str;                            // id message
  struct MW_EXPECT_SEND *login_scripts;     // Login Scirpts
  DWORD  login_pair;                        // Login script pairs,    0xFF => Use default
  char  *phone1;                            // Primary phone number
  char  *phone2;                            // Secondary phone number
  struct MW_MDM_PORT line_config;           // Dial parameters
  BYTE  *redial_param;                      // Redial parameters
  DWORD reconnect_waittime;                 // wait time in second before reconnect. (second)
  DWORD reconnect_retry;                    // reconnect retry times
  DWORD login_finish;                       // Send Login finish. 1=> Success 0: User Abort
  DWORD idle_timeout;                       // Idle timeout (second)
  DWORD min_timeout;                        // Minimum Up Time (second)
  DWORD alive_report_time;                  // time to send ping reply (10ms)
};

// PPP device
#define MW_PPP_DEVICE_NONE              0
#define MW_PPP_DEVICE_LINE              1
#define MW_PPP_DEVICE_AUX1              2
#define MW_PPP_DEVICE_AUX2              3
#define MW_PPP_DEVICE_AUX0              4
#define MW_PPP_DEVICE_GPRS              5
#define MW_PPP_DEVICE_ECR1              6
#define MW_PPP_DEVICE_ECR2              7
#define MW_PPP_DEVICE_ECR3              8
#define MW_PPP_DEVICE_OPENNEG           K_PppDeviceOpenNeg
#define MW_PPP_DEVICE_FIXIP             K_PppDeviceFixIp

// PPP mode
#define MW_PPP_MODE_NONE            0x00
#define MW_PPP_MODE_INTERACTIVE     0x01   /* Manual link */
#define MW_PPP_MODE_NORMAL          0x02
#define MW_PPP_MODE_DIRECT          0x04   /* already established */
#define MW_PPP_MODE_DDIAL           0x08   /* Dial immediately, stay connected (dedicate dial) */

// ppp phase
#define MW_PPP_PHASE_IDLE           -1   /* idle not open */
#define	MW_PPP_PHASE_DEAD            0   /* Link is dead */
#define	MW_PPP_PHASE_ESTABLISH       1   /* Establishing link */
#define	MW_PPP_PHASE_AUTHENTICATE    2   /* Being authenticated */
#define	MW_PPP_PHASE_NETWORK         3   /* We're alive ! */
#define	MW_PPP_PHASE_TERMINATE       4   /* Terminating link */
#define MW_PPP_PHASE_READY           5   /* ppp is in a usable state */


// => Codec define
//***********************************
// For IOCtlMW
#define IO_CDC_PLAYFILE       1
#define IO_CDC_PLAYMEM        2
#define IO_CDC_STOP           3

// for Play
struct CODEC_DATA {
  BYTE *content;
  DWORD offset;
  DWORD len;
};

// For Codec Status cmd
#define MW_CODEC_STATUS         0
#define MW_CODEC_VOL            0x80000000

// Codec Status
#define MW_CODEC_OPENED           0x00000001
#define MW_CODEC_PLAYING          0x00000002
#define MW_NOT_WAVEFILE           0x00000100
#define MW_FORMAT_NOTSUPPORT      0x00000200
#define MW_INTERNAL_ERROR         0x00000400
#define MW_FILEOPEN_ERROR         0x00000800
#define MW_FILEREAD_ERROR         0x00001000
#define MW_CODECINIT_ERROR        0x00002000
#define MW_MEMALLOC_ERROR         0x00004000
#define MW_FILESEEK_ERROR         0x00008000
#define MW_READ_ERROR             0x00010000
#define MW_NOTFINISH_ERROR        0x00020000
#define MW_CODEC_ERROR            0x00040000

// USB Serial defines
//***********************************
// For IOCtlMW
//
//
// USBS Status
// bit0-7: Port status, bit8-15: USB state, bit16-23: error code, bit24-31: reserved
#define MW_USB_RXRDY     0x01
#define MW_USB_TXRDY     0x02
// state
#define MW_USB_ATTACHED    	0x0100
#define MW_USB_POWERED   	0x0200
#define MW_USB_DEFAULT   	0x0400
#define MW_USB_ADDRESS   	0x0800
#define MW_USB_CONFIGURED  	0x1000
#define MW_USB_SUSPENDED   	0x2000
// error code
#define MW_USB_LOCKED    0x010000  // end point busy or not available
#define MW_USB_ABORTED   0x020000  // transfer aborted
#define MW_USB_RESET     0x030000  // end point or device reset

// USBSer status selector
#define MW_USBS_STATUS               0


// DNS defines
//***********************************
// For IOCtlMW
#define IO_DNS_REQUEST      1
#define IO_DNS_RESPONSE     2
#define IO_DNS_SET_NIF      3

#define MW_DNS_DEFAULT_PORT      53
#if ((A5T1000|T300|T810)==0)
#define MW_DNS_MAX_LEN           100
#else
#define MW_DNS_MAX_LEN           K_DnsMaxLen
#endif
#if (PR608|TIRO)
  #define DNS_FMT           1
  struct DNS_REQ_DATA {
    BYTE  *p_name;
    BYTE  sb_ip[4];
    DWORD d_port;
    DWORD d_timeout;  // !RS RESERVE FOR FUTURE timeout in 10ms, default = 10 secs.
    BYTE  b_nif;          // network interface (Ethernet / PPP)   // v1.2
  };
#else //R700|T800|PR608D|T700|T1000|T600|CREON4|T1000CT|A5T1000|T300|T810
  #define DNS_FMT           2
  struct DNS_REQ_DATA {
    BYTE  sb_name[MW_DNS_MAX_LEN];
    BYTE  sb_ip[4];
    DWORD d_port;
    DWORD d_timeout;      // timeout value in 10ms, default 10sec
    union {
    BYTE  b_nif;          // network interface (Ethernet / PPP)   // v1.2
    BYTE  b_option;
    };
  };
#endif

// HTTP defines
//***********************************
// For IOCtlMW
#define IO_HTTP_CONNECT      1
#define IO_HTTP_DISCONNECT   2
#define IO_HTTP_REQUEST      3
#define IO_HTTP_RESPONSE     4
#define IO_HTTP_REQUEST_CH   5
#define IO_HTTP_REQUEST2     6
#define IO_HTTP_CONNECT2     7
#define IO_HTTP_RESPONSE_HEADER 8  // CREON4 only

// DNS port
#define MW_HTTP_DEFAULT_PORT    80
#define MW_HTTPS_DEFAULT_PORT   443

// Version
#define MW_HTTP_VER1_0     0
#define MW_HTTP_VER1_1     1

#ifdef K_HttpMaxHostLen
#define HTTP_MAX_HOSTBUF       K_HttpMaxHostLen
#define HTTP_MAX_URIBUF        K_HttpMaxUriLen
#define HTTP_MAX_CONTENT       K_HttpMaxTypeLen
#define HTTP_MAX_CUSTHDR       K_HttpMaxCustHdrLen
#else
#define HTTP_MAX_HOSTBUF       300
#define HTTP_MAX_URIBUF        100
#define HTTP_MAX_CONTENT       200
#define HTTP_MAX_CUSTHDR       300
#endif

struct HTTP_INFO {
  BYTE    sb_dns_ip[4];
  WORD    w_dns_port;
  WORD    w_host_port;
  BYTE    s_host[HTTP_MAX_HOSTBUF];
  BYTE    b_sslidx;   // same as tcp
  BYTE    b_option;   // same as tcp
  BYTE    b_version;  // http version
  #if (T1000|PR608D|CREON4)
  BYTE    b_certidx;  // client cert idx
  BYTE    b_keyidx;   // client cert private key idx
  BYTE    b_eoption;
  BYTE    s_rfu[1];
  #endif
};

// http parameters for connect2
struct HTTP_INFO2 {
  DWORD d_size;         // must equal sizeof(struct HTTP_INFO2)
  DWORD d_xctrl;        // extra control handle
  BYTE  sb_dns_ip[4];
  WORD  w_dns_port;
  WORD  w_host_port;
  BYTE  s_host[HTTP_MAX_HOSTBUF];
  BYTE  b_sslidx;   // same as tcp
  BYTE  b_option;   // same as tcp
  BYTE  b_version;  // http version
  BYTE  b_certidx;  // client cert idx
  BYTE  b_keyidx;   // client cert private key idx
  BYTE  b_eoption;
  BYTE  s_rfu[1];
};


// Method
#define MW_HTTP_GET               0
#define MW_HTTP_POST              1

#if (PR608|TIRO|R700|PR608D|CREON4)
struct HTTP_REQ {
  WORD    w_req_len;
  BYTE    s_uri[HTTP_MAX_URIBUF];
  BYTE    b_method;
  BYTE    b_close;    // set pack close http connection at the end
  BYTE    *p_data;
  WORD    w_max_rlen; // max rx buffer length
  WORD    w_timeout;  // response timeout in unit 10msec, 0 = default 1000 for 10sec
  BYTE    *p_output;
  #if (R700|PR608D|CREON4)
  BYTE    s_type[HTTP_MAX_CONTENT]; // Content Type p_content[0] == 0
  #endif
};
#else
struct HTTP_REQ {
  WORD    w_req_len;
  BYTE    *s_uri;
  BYTE    b_method;
  BYTE    b_close;    // set pack close http connection at the end
  BYTE    *p_data;
  WORD    w_max_rlen; // max rx buffer length
  WORD    w_timeout;  // response timeout in unit 10msec, 0 = default 1000 for 10sec
  BYTE    *p_output;
  BYTE    s_type[HTTP_MAX_CONTENT]; // Content Type p_content[0] == 0
};
#endif

#if (PR608D|CREON4)
struct HTTP_REQ_CH {
  WORD    w_req_len;
  BYTE    s_uri[HTTP_MAX_URIBUF];
  BYTE    b_method;
  BYTE    b_close;    // set pack close http connection at the end
  BYTE    *p_data;
  WORD    w_max_rlen; // max rx buffer length
  WORD    w_timeout;  // response timeout in unit 10msec, 0 = default 1000 for 10sec
  BYTE    *p_output;
  BYTE    s_type[HTTP_MAX_CONTENT]; // Content Type p_content[0] == 0
  BYTE    *p_cust_hdr; // customize header fields
};
#else
#ifndef K_HttpMaxTypeLen
#define K_HttpMaxTypeLen  200
#endif
struct HTTP_REQ2 {
  WORD    w_req_len;
  BYTE    b_method;
  BYTE    b_close;    // set pack close http connection at the end
  WORD    w_max_rlen; // max rx buffer length
  WORD    w_timeout;  // response timeout in unit 10msec, 0 = default 1000 for 10sec
  char    *p_uri;
  char    *p_cust_hdr;
  BYTE    s_type[K_HttpMaxTypeLen];
  BYTE    *p_data;
  BYTE    *p_output;
};
#endif

// Http status selector

#define MW_HTTP_STATUS              0

// bit0-7 = http status
#define MW_HTTP_IDLE                0
#define MW_HTTP_OPENED              1
#define MW_HTTP_DNSRESOLVE          2
#define MW_HTTP_CONNECTING          3
#define MW_HTTP_CONNECTED           4
#define MW_HTTP_REQUESTED           5

// GPRS defines
//***********************************
// For IOCtlMW
#define IO_GPRS_SETRING             0
#define IO_GPRS_SIGNAL              1
#define IO_GPRS_CMDSEND             2
#define IO_GPRS_CMDRESP             3
#define IO_GPRS_RESET               4
#define IO_GPRS_SMS_DECODE          5
#define IO_GPRS_SMS_ACK             6
#define IO_GPRS_SMS_SEND            7
#define IO_GPRS_SIM_SEL             8
#define IO_GPRS_SLEEP               9
#define IO_GPRS_SMS_ENB             10
#define IO_GPRS_SMS_READ            11
#define IO_GPRS_GET_CID             12
#define IO_GPRS_READ_USSD           13  //PC1211 T1000WCDMA use struct GPRS_CMDRESP

// GPRS status
#define MW_GPRS_STAT_UNDEF           0
#ifdef K_GsmError
  #define MW_GSTS_ERR                K_GsmError              // gsm initial error
#else
  #define MW_GSTS_ERR                MW_GPRS_STAT_UNDEF
#endif
#ifdef K_GsmOn
  #define MW_GSTS_ON                 K_GsmOn                  // gsm module turned on
#else
  #define MW_GSTS_ON                 K_GPRS_STATUS_PSSUP
#endif
#ifdef K_GsmSleep
  #define MW_GSTS_SLEEP              K_GsmSleep              // gsm in sleep mode
#else
  #define MW_GSTS_SLEEP              K_GPRS_STATUS_SLEEP
#endif
#ifdef K_GsmRxReady
  #define MW_GSTS_RXREADY            K_GsmRxReady            // data receive ready
#else
  #define MW_GSTS_RXREADY            K_GPRS_STATUS_RXRDY2
#endif
#ifdef K_GsmTxReady
  #define MW_GSTS_TXREADY            K_GsmTxReady            // data transmit ready
#else
  #define MW_GSTS_TXREADY            K_GPRS_STATUS_TXRDY2
#endif
#ifdef K_GsmAtCmdReady
  #define MW_GSTS_ATCMDRDY           K_GsmAtCmdReady         // AT command send ready
#else
  #define MW_GSTS_ATCMDRDY           K_GPRS_STATUS_TXRDY1
#endif
#ifdef K_GsmAtRespReady
  #define MW_GSTS_ATRESPRDY          K_GsmAtRespReady        // at command response ready
#else
  #define MW_GSTS_ATRESPRDY          K_GPRS_STATUS_RXRDY1
#endif
#ifdef K_GsmAtReady1
  #define MW_GSTS_ATREADY1           K_GsmAtReady1           // initialized, ready to accept AT commands except photobooks, AOC, SMS
#else
  #define MW_GSTS_ATREADY1           K_GPRS_STATUS_READY
#endif
#ifdef K_GsmAtReady2
  #define MW_GSTS_ATREADY2           K_GsmAtReady2           // ready to accept all AT commands
#else
  #define MW_GSTS_ATREADY2           K_GPRS_STATUS_SMS_READY  //PC1211 change CBM->SMS, some module may not support CBM
#endif
#ifdef K_GsmSimInserted
  #define MW_GSTS_SIMINSERTED        K_GsmSimInserted        // sim card inserted
#else
  #define MW_GSTS_SIMINSERTED        K_GPRS_STATUS_READY
#endif
#ifdef K_GsmNetworkAvailable
  #define MW_GSTS_NETWORKAVAIL       K_GsmNetworkAvailable   // gsm network service available
#else
  #define MW_GSTS_NETWORKAVAIL       K_GPRS_STATUS_NETWORK_READY
#endif
#ifdef K_GsmCalling
  #define MW_GSTS_CALLING            K_GsmCalling            // gsm is calling, set after ATD until no carrier
#else
  #define MW_GSTS_CALLING            MW_GPRS_STAT_UNDEF
#endif
#ifdef K_GsmGrpsUp
  #define MW_GSTS_GPRSUP             K_GsmGrpsUp             // GPRS is connected, set after <CONNECT> until <NO CARRIER>
#else
  #define MW_GSTS_GPRSUP             MW_GPRS_STAT_UNDEF
#endif
#ifdef K_GsmSmsNewMsg
  #define MW_GSTS_SMSNEWMSG          K_GsmSmsNewMsg          // sms NEW message, will be cleared if read
#else
  #define MW_GSTS_SMSNEWMSG          K_GPRS_STATUS_SMSRXRDY
#endif
#ifdef K_GsmRi
  #define MW_GSTS_RI                 K_GsmRi                 // gsm ring indicator pin, will be cleared if read
#else
  #define MW_GSTS_RI                 K_GPRS_STATUS_RING
#endif
#ifdef K_GPRS_STATUS_CID
  #define MW_GSTS_CID                K_GPRS_STATUS_CID       // Caller ID Ready
#else
  #define MW_GSTS_CID                MW_GPRS_STAT_UNDEF
#endif

struct SMS_ADDR
{
  BYTE b_len;       // len = no. of phone no. digits
  BYTE b_type;
  char sb_addr[20];  // ascii code
}
#if (!__IAR_SYSTEMS_ICC__)
__attribute__((packed))
#endif
;

struct SMS_PDU
{
  struct SMS_ADDR s_smsc;        // sms sevice center address
  BYTE            b_pdu_len;     // pdu length (start from tp_flags)
  BYTE            b_tp_flags;    // sms pdu first octat
  struct SMS_ADDR s_tp_a;        // deliver:originating address, submit:destination address
  BYTE            b_tp_mr;       // message reference
  BYTE            b_tp_st;       // status
  BYTE            b_tp_fcs;      // tp-failure-cause (for RP-ERROR only)
  BYTE            b_tp_pi;       // tp-parameter indicator
  BYTE            b_tp_pid;      // protocol identifier
  BYTE            b_tp_ct;       // command type
  BYTE            b_tp_dcs;      // data coding scheme
  BYTE            sb_tp_time[7]; // deliver:service center time stamp, submit:validity period
  BYTE            sb_tp_dt[7];   // discharge time
  BYTE            b_tp_udl;      // user data len, max. 160 for 7-bit data, othwise max. 140
  BYTE            sb_tp_ud[160]; // user data (ascii code)
}
#if (!__IAR_SYSTEMS_ICC__)
__attribute__((packed))
#endif
;

#define MAX_ATCMD_SIZE  1500

// For GPRS_CMDSEND
struct GPRSATCMD {
  DWORD d_timeout;
  BYTE  sb_content[MAX_ATCMD_SIZE+1];
};

// For GPRS_CMDRESP
struct GPRSATRESP {
  DWORD d_maxlen;
  BYTE  sb_content[MAX_ATCMD_SIZE+1];
};

// SMSs AckType
#define MW_NO_RESP          0       // no reply
#define MW_RESP_ACK         1       // reply ack (with or without pdu)
#define MW_RESP_ERR         2       // reply error (must with pdu)

// SMS destination
#define MW_SMS_DEST         'S'
#define MW_SMS_MEM          'W'

// For GPRS_SMS_xxxx
struct GPRSSMSDATA {
  struct SMS_PDU *s_pdu;
  BYTE           *s_str;
  DWORD          *d_len;
  DWORD          d_ack_type;
  DWORD          d_text_mode;
  DWORD          d_dest;
};

// wmode defines
#define MW_PDUMODE         0
#define MW_TXTMODE         1

// possible values of TP_MTI        Directions
#define MW_SMS_DELIVER      0       // receive
#define MW_SMS_DELREP       0       // send
#define MW_SMS_SUBMIT       1       // send
#define MW_SMS_SUBREP       1       // receive
#define MW_SMS_STSREP       2       // receive
#define MW_SMS_CMD          2       // send

// Thermal Printer defines
//***********************************
// For IOCtlMW
#define IO_TMLPT_CFG                0
#define IO_TMLPT_RESET              2

// For IO_TMLPT_CFG
#ifdef K_LptNormalMode
  #define MW_TMLPT_MODE_NORMAL        K_LptNormalMode
#endif
#ifdef K_LptBarcodeMode
  #define MW_TMLPT_MODE_BARCODE       K_LptBarcodeMode
#endif

struct TMLPTCFG {
  BYTE b_len;               // 5
  BYTE b_mode;              // 0-Normal,  1-barcode
  BYTE b_pwr_speed;         // print speed for power in,    0-4 (22 to 14 lines/sec)
  BYTE b_bat_speed;         // print speed in battary mode  0-4 (16 to 8 lines/sec)
  BYTE b_pwr_strobe;        // strobe width adjustment for power in (+- 0 to 7)
  BYTE b_bat_strobe;        // strobe width adjustment for battery  (+- 0 to 7)
};

// For IO_TMLPT_STATUS (input)
#ifdef K_LptStatusRead
  #define MW_TMLPT_NORSTATUS          K_LptStatusRead               // Normal Status
#endif
#ifdef K_LptStatusRestart
  #define MW_TMLPT_ERR_RESTART        K_LptStatusRestart            // Restart on error
#endif
#ifdef K_LptStatusResetError
  #define MW_TMLPT_ERR_RESET          K_LptStatusResetError         // Reset On error
#endif
#ifdef K_LptStatusReset
  #define MW_TMLPT_RESET              K_LptStatusReset              // Reset
#endif
#ifdef K_LptStatusTemperature
  #define MW_TMLPT_TEMP               K_LptStatusTemperature        // Get Temperature
#endif

// Status output
// short status (output)
#ifdef K_LptOk
  #define MW_TMLPT_SOK                K_LptOk
#endif
#ifdef K_LptErrTempHigh
  #define MW_TMLPT_STEMPHIGH          K_LptErrTempHigh
#elif defined K_ErrTempHigh
  #define MW_TMLPT_STEMPHIGH          K_ErrTempHigh
#endif
#ifdef K_LptErrPaperout
  #define MW_TMLPT_SPAPEROUT          K_LptErrPaperout
#elif defined K_ErrPaperout
  #define MW_TMLPT_SPAPEROUT          K_ErrPaperout
#endif
#ifdef K_LptErrVoltLow
  #define MW_TMLPT_SVOLTLOW           K_LptErrVoltLow
#elif defined K_ErrVoltLow
  #define MW_TMLPT_SVOLTLOW           K_ErrVoltLow
#else
  #define MW_TMLPT_SVOLTLOW           10
#endif
#ifdef K_LptErrPrint
  #define MW_TMLPT_SERROR             K_LptErrPrint
#elif defined K_ErrPrinting
  #define MW_TMLPT_SERROR             K_ErrPrinting
#endif
#ifdef K_LptErrTime
  #define MW_TMLPT_STIME              K_LptErrTime
#endif
// long status (output)
#ifdef K_LptTempHigh
  #define MW_TMLPT_FTEMPHIGH          K_LptTempHigh
#endif
#ifdef K_LptPaperout
  #define MW_TMLPT_FPAPEROUT          K_LptPaperout
#endif
#ifdef K_LptLowVolt
  #define MW_TMLPT_FVOLTLOW           K_LptLowVolt
#else
  #define MW_TMLPT_FVOLTLOW           0x800000
#endif
#ifdef K_LptPrinting
  #define MW_TMLPT_FPRINTING          K_LptPrinting
#endif
#ifdef K_LptFinish
  #define MW_TMLPT_FFINISH            K_LptFinish
#endif

// LPT0 defines
//***********************************
// For IOCtlMW
enum {
  IO_LPT0_STS_GET=0,
  IO_LPT0_CFG,
  IO_LPT0_RESET,
  IO_LPT0_RESTART,
  IO_LPT0_RESETERR,
  IO_LPT0_CMD,
};

enum {
  MW_LPT0_STAT_NOTSTART=0,
  MW_LPT0_STAT_PRINTING,
  MW_LPT0_STAT_FINISH,
  MW_LPT0_STAT_HIGHTEMP,
  MW_LPT0_STAT_LOWVOLT,
  MW_LPT0_STAT_PAPEROUT,
  MW_LPT0_STAT_ERROR,
};

// => ECR define
//***********************************
// For IOCtlMW
#define IO_ECR_CFG          0x0001
#define IO_ECR_TX_RDY       0x0002
#define IO_ECR_RX_RDY       0x0003

//aux communcation mode
#define MW_ECR_AUTO           0x00
#define MW_ECR_STX            0x01
#define MW_ECR_ENAC           0x02
#define MW_ECR_SOH_LRC        0x03
#define MW_ECR_SOH_CRC        0x04
#define MW_ECR_NO_PARITY      0x05    // default
#define MW_ECR_EVEN_PARITY    0x06
#define MW_ECR_ODD_PARITY     0x07

// aux speed
#define MW_ECR_9600           0       // default
#define MW_ECR_2400           1
#define MW_ECR_4800           2
#define MW_ECR_19200          3
#define MW_ECR_28800          4
#define MW_ECR_38400          5
#define MW_ECR_57600          6
#define MW_ECR_115200         7
#define MW_ECR_230400         8

struct MW_ECR_CFG {
  BYTE b_len;
  BYTE b_mode;
  BYTE b_speed;
  BYTE b_rx_gap;
  BYTE b_rsp_gap;
  BYTE b_tx_gap;
  BYTE b_retry;
};

// => Barcode Reader  (brdr)
//***********************************
// IOCtl
enum {
  IO_BRDR_CMD      = 1,
  IO_BRDR_SCAN,
  IO_BRDR_ABORT,
  IO_BRDR_ENABLE,
};

struct MW_BRDR_CMD {
  BYTE  *sb_cmd;
  DWORD dw_len;
};

// Status return value
enum {
  MW_BRDR_STAT_NOTRDY,
  MW_BRDR_STAT_OPEN,
  MW_BRDR_STAT_SCANNING,
  MW_BRDR_STAT_DATARDY,
  MW_BRDR_STAT_TIMEOUT,
  MW_BRDR_STAT_ABORT,
};


// Contactless ICC defines
//***********************************
// IOCtl
enum {
  IO_CLIC_POLL      = 1,
  IO_CLIC_REMOVE,
  IO_CLIC_CMD,
};

struct MW_CLIC_CMD_DAT {
  BYTE* a_in;
  BYTE* a_out; /* buffer size at least K_TBufSize+K_AppendSize */
  WORD  a_len;
};


// Color LED defines
//***********************************
// IOCtl
enum {
  IO_CLED_STATUS = 1,
  IO_CLED_SET       ,
  IO_CLED_CLEAR     ,
  IO_CLED_ON        ,
};

// led id
enum {
  MW_CLED_GREEN,
  MW_CLED_BLUE,
  MW_CLED_YELLOW,
  MW_CLED_RED,
  MW_CLED_MAX,
};

struct MW_CLED_ON_DAT {
  DWORD a_which;
  DWORD a_on_off;
  DWORD a_cycle;
};

#define MW_CLIC_STAT_OK          (0)
#define MW_CLIC_STAT_NO_CARD     (-1)
#define MW_CLIC_STAT_COLLISION   (-2)
#define MW_CLIC_STAT_TRANS       (-3)
#define MW_CLIC_STAT_PROTOCOL    (-4)
#define MW_CLIC_STAT_TIMEOUT     (-5)
#define MW_CLIC_STAT_INTERNAL    (-6)
#define MW_CLIC_STAT_NOT_READY   (-7)
#define MW_CLIC_STAT_INPUT       (-8)
#define MW_CLIC_STAT_WTX_TIMEOUT (-9)

// => WIFI
//***********************************
#define MW_MAX_AP_LIST      MAX_SCANNED_WIFI_AP

// wifi state
#define MW_WIFI_ON          K_LanEnabled
#define MW_WIFI_CONNECTED   K_LanPlugIn

// WIFI connction/encryption Type
#define MW_WIFI_TYPE_OPEN   WIFI_TYPE_ENC_OPEN
#define MW_WIFI_TYPE_WEP    WIFI_TYPE_ENC_WEP
#define MW_WIFI_TYPE_WPA2   WIFI_TYPE_ENC_WPA_WPA2
#define MW_WIFI_TYPE_PBC    WIFI_TYPE_ENC_WPS_PBC
#define MW_WIFI_TYPE_PIN    WIFI_TYPE_ENC_WPS_PIN
#define MW_WIFI_TYPE_INFRA  WIFI_TYPE_MODE_INFRA
#define MW_WIFI_TYPE_ADHOC  WIFI_TYPE_MODE_ADHOC

struct MW_AP_INFO {
  signed char   rssi;       ///< receive signal strength indicator
  unsigned char snr;		///< signal to noise ratio
  unsigned char channel;	///< channel number
  unsigned char types;		///< connection type, encryption type
  char          ssid[33];   ///< Service set ID, null terminate string
  unsigned char bssid[6];	///< Basic Service set ID
  unsigned char rfu;		///< for alignment
};

struct MW_AP_PROFILE {
  char          ssid[33];   ///< ssid, null terminate string
  unsigned char key_index;  ///< for WEP only
  unsigned char enc_type;   ///< encryption type
  unsigned char rfu;
  char          pass[64];   ///< password, null terminate string
};

// IO_WIFI_STATUS
#define MW_AP_DISCONNECTED      NET_AP_DISCONNECTED
#define MW_AP_CONNECTED         NET_AP_CONNECTED
#define MW_AP_WRONG_PASS        NET_AP_WRONG_PASS
#define MW_AP_TIMEOUT           NET_AP_TIMEOUT
#define MW_AP_CONNECTING        NET_AP_CONNECTING

// IOCtl
enum {
  IO_WIFI_FW_VER  =0,       // ret=IOCtlMW(aFd,IO_WIFI_FW_VER, BYTE *);    ret -ve => Error
  IO_WIFI_ON        ,       // ret=IOCtlMW(aFd,IO_WIFI_ON,NULL);    ret -ve => Error
  IO_WIFI_OFF       ,       // ret=IOCtlMW(aFd,IO_WIFI_OFF,NULL);    ret -ve => Error
  IO_WIFI_SCAN      ,       // ret=IOCtlMW(aFd,IO_WIFI_SCAN, struct MW_AP_INFO *);  ret = number of scanned ap or Error Code
  IO_WIFI_CONNECT   ,       // ret=IOCtlMW(aFd,IO_WIFI_CONNECT, struct MW_AP_PROFILE *);  -ve => error
  IO_WIFI_DISCONNECT,       // ret=IOCtlMW(aFd,IO_WIFI_DISCONNECT, NULL);  0=>OK, -ve=>Error Code
  IO_WIFI_RECONNECT ,       // ret=IOCtlMW(aFd,IO_WIFI_RECONNECT, DWORD reconnect_time_in_sec);  -ve => Error
  IO_WIFI_STATUS    ,       // status=IOCtlMW(aFd, IO_WIFI_STATUS, struct MW_AP_PROFILE *);
};


//-----------------------------------------------------------------------------
//   Middleware DLL Functions define
//-----------------------------------------------------------------------------
enum {
  STARTUPMW,          // std IO
  SHUTDOWNMW,
  OPENMW,
  CLOSEMW,
  READMW,
  WRITEMW,
  LSEEKMW,
  IOCTLMW,
  STATMW,
  GETLASTERRMW,
  FSYSINITMW,         // file system
  FCREATEMW,
  FDELETEMW,
  FOPENMW,
  FCLOSEMW,
  FLENGTHMW,
  FSEEKMW,
  FREADMW,
  FWRITEMW,
  FCOMMITALLMW,
  FFLUSHMW,
  FDSEARCHMW,
  GETCHARMW,          // keyboard util
  KBDSCODEMW,
  DESKEYMW,           // encrytion util
  DESMW,
  DES2MW,
  DISPLOGOMW,         // LCD display
  ICCINSERTEDMW,      // ICC inserted
  SCHNLSETUPMW,       // Setup Secure Channel with PP300
  SCHNLGETPINMW,      // Get Pin Via Secure Channel
  LLSEEKMW,           //
  DISP_PUTCMW,
  DISP_PUTNMW,
  DISP_CLRLINEMW,
  DISP_CLRBELOWMW,
  DISP_GOTOMW,
  DISP_LINEMW,
  DISP_SETPMW,
  DISP_GETGMW,
  DISP_PUTGMW,
  GET_LCD_TYPEMW,
  OS_CALL_MW,
  BS_CALL_MW,
  SMS_ENB_MW,
  SMS_RX_MW,
  SMS_TX_MW,
  SMS_COUNT_MW,
  SMS_CLEAR_ALL_MW,
  FRENAMEMW,
  FTRUNCATEMW,
  FFTRUNCATEMW,
  APPINFO_GETMW,
  MW_FUNC_COUNT       // Must be the last item
};

//=> standard I/O api
//extern int Startup(void);
#define StartupMW()           (int) lib_app_call(MIDWARE_ID, STARTUPMW, (DWORD)0, (DWORD)0, (DWORD)0)
//extern int Shutdown(void);
#define ShutdownMW()          (int) lib_app_call(MIDWARE_ID, SHUTDOWNMW, (DWORD)0, (DWORD)0, (DWORD)0)
//extern int Open(char *aDevName, DWORD aFlag);
#define OpenMW(x,y)           (int) lib_app_call(MIDWARE_ID, OPENMW, (DWORD)x, (DWORD)y, 0)
//extern int Close(int aFileDesc);
#define CloseMW(x)            (int) lib_app_call(MIDWARE_ID, CLOSEMW, (DWORD)x, 0, 0)
//extern int Read(int aFileDesc, BYTE *aBuf, int aLen);
#define ReadMW(x,y,z)         (int) lib_app_call(MIDWARE_ID, READMW, (DWORD)x, (DWORD)y, (DWORD)z)
//extern int Write(int aFileDesc, BYTE *aBuf, int aLen);
#define WriteMW(x,y,z)        (int) lib_app_call(MIDWARE_ID, WRITEMW, (DWORD)x, (DWORD)y, (DWORD)z)
//extern int LSeek(int aFileDesc, int aOffSet, int aMode);
#define LSeekMW(x,y,z)        (int) lib_app_call(MIDWARE_ID, LSEEKMW, (DWORD)x, (DWORD)y, (DWORD)z)
//extern int IOCtl(int aFileDesc, int aCmd, void *aParam);
#define IOCtlMW(x,y,z)        (int) lib_app_call(MIDWARE_ID, IOCTLMW, (DWORD)x, (DWORD)y, (DWORD)z)
//extern int Stat(int aFileDesc, int aCmd, void *aParam);
#define StatMW(x,y,z)         (int) lib_app_call(MIDWARE_ID, STATMW, (DWORD)x, (DWORD)y, (DWORD)z)
//extern int GetLastErr(void);
#define GetLastErrMW()        (int) lib_app_call(MIDWARE_ID, GETLASTERRMW, 0, 0, 0)

//=> file system
//extern int fSysInit(void);
#define fSysInitMW()          (int) lib_app_call(MIDWARE_ID, FSYSINITMW, (DWORD)0, (DWORD)0, (DWORD)0)
//extern int fCreate(const char* aFileName, int aAttr);
#define fCreateMW(x,y)        (int) lib_app_call(MIDWARE_ID, FCREATEMW, (DWORD)x, (DWORD)y, (DWORD)0)
//extern int fDelete(const char* aFileName);
#define fDeleteMW(x)          (int) lib_app_call(MIDWARE_ID, FDELETEMW, (DWORD)x, (DWORD)0, (DWORD)0)
//extern int fOpen(const char* aFileName);
#define fOpenMW(x)            (int) lib_app_call(MIDWARE_ID, FOPENMW, (DWORD)x, (DWORD)0, (DWORD)0)
//extern int fClose(int aFileHandle);
#define fCloseMW(x)           (int) lib_app_call(MIDWARE_ID, FCLOSEMW, (DWORD)x, (DWORD)0, (DWORD)0)
//extern int fLength(int aFileHandle);
#define fLengthMW(x)          (int) lib_app_call(MIDWARE_ID, FLENGTHMW, (DWORD)x, (DWORD)0, (DWORD)0)
//extern int fSeek(int aFileHandle, int aOffset);
#define fSeekMW(x,y)          (int) lib_app_call(MIDWARE_ID, FSEEKMW, (DWORD)x, (DWORD)y, (DWORD)0)
//extern int fRead(int aFileHandle, char* aBuf, int aLen);
#define fReadMW(x,y,z)        (int) lib_app_call(MIDWARE_ID, FREADMW, (DWORD)x, (DWORD)y, (DWORD)z)
//extern int fWrite(int aFileHandle, char* aBuf, int aLen);
#define fWriteMW(x,y,z)       (int) lib_app_call(MIDWARE_ID, FWRITEMW, (DWORD)x, (DWORD)y, (DWORD)z)
//extern void fCommitAll(void);
#define fCommitAllMW()        (void) lib_app_call(MIDWARE_ID, FCOMMITALLMW, (DWORD)0, (DWORD)0, (DWORD)0)
//extern int fFlush(int aFileHandle);
#define fFlushMW(x)           (int) lib_app_call(MIDWARE_ID, FFLUSHMW, (DWORD)x, (DWORD)0, (DWORD)0)
//extern int fDSearch(int aReset, struct MW_FILE_HDR* aOutHdr);
#define fDSearchMW(x,y)       (int) lib_app_call(MIDWARE_ID, FDSEARCHMW, (DWORD)x, (DWORD)y, (DWORD)0)
//extern int fRename(const char* aNewFileName, const char* aOldFileName);
#define fRenameMW(x,y)        (int) lib_app_call(MIDWARE_ID, FRENAMEMW, (DWORD)x, (DWORD)y, (DWORD)0)
//extern int fTruncate(const char* aFileName, DWORD aLen);
#define fTruncateMW(x,y)      (int) lib_app_call(MIDWARE_ID, FTRUNCATEMW, (DWORD)x, (DWORD)y, (DWORD)0)
//extern int fFTruncate(int aFileHandle, int aLen);
#define fFTruncateMW(x,y)     (int) lib_app_call(MIDWARE_ID, FFTRUNCATEMW, (DWORD)x, (DWORD)y, (DWORD)0)

//=> Keyboard functions
//extern DWORD GetChar(void);
#define GetCharMW()           (DWORD) lib_app_call(MIDWARE_ID, GETCHARMW, 0, 0, 0)
//extern DWORD KbdScode(void);
#define KbdScodeMW()          (DWORD) lib_app_call(MIDWARE_ID, KBDSCODEMW, 0, 0, 0)

//=> encrytion functions
//extern void DesKey(void* aKey, DWORD aKeyLen);
#define DesKeyMW(x,y)         (void) lib_app_call(MIDWARE_ID, DESKEYMW, (DWORD)x, (DWORD)y, (DWORD)0)
//extern void Des(void* aData);
#define DesMW(x)              (void) lib_app_call(MIDWARE_ID, DESMW, (DWORD)x, (DWORD)0, (DWORD)0)
//extern void Des2(void* aData);
#define Des2MW(x)            	(void) lib_app_call(MIDWARE_ID, DES2MW, (DWORD)x, (DWORD)0, (DWORD)0)

//=> LCD display functions
//BOOLEAN DispLogo(BYTE const *aPicLogo, BYTE aPosX, BYTE aPosY)
#define DispLogoMW(x,y,z)     (BOOLEAN) lib_app_call(MIDWARE_ID, DISPLOGOMW, (DWORD)x, (DWORD)y, (DWORD)z)

//=> ICC inserted functions
// extern DWORD ICCInserted(void);	// bitwise index output
#define ICCInsertedMW()				(DWORD) lib_app_call(MIDWARE_ID, ICCINSERTEDMW, (DWORD)0, (DWORD)0, (DWORD)0)

//=> MMC/SD inserted functions
// extern BOOLEAN MMCInserted(void);
#ifdef os_mmc_inserted
  #define MMCInsertedMW()				os_mmc_inserted()
#else
  #define MMCInsertedMW()				(BOOLEAN) FALSE
#endif

//=> Lan Cable inserted functions
// extern BOOLEAN LanCableInserted(void);
#ifdef bs_lan_on
  #define LanCableInsertedMW()        (BOOLEAN) bs_lan_on()
#elif defined(bs_lan_status)
  #define LanCableInsertedMW()        (BOOLEAN) (bs_lan_status(0)&K_LanPlugIn)
#else
  #define LanCableInsertedMW()        (BOOLEAN) FALSE
#endif

#ifndef K_MaxKeyRsaModulusBits
#define K_MaxKeyRsaModulusBits  2048
#endif

struct MW_KEY {
  DWORD  d_exponent;
  DWORD  d_keysize;
  DWORD  s_key[K_MaxKeyRsaModulusBits/32];  // !2012-06
  DWORD  d_key_idx;
};

#define MW_4KBITS            4096
struct MW_KEY_4K {
  DWORD  d_exponent;
  DWORD  d_keysize;
  DWORD  s_key[MW_4KBITS/32];
  DWORD  d_key_idx;
};
//=> CA Key Injection Functions
// BOOLEAN InjectCAKey(struct MW_KEY * a_key);
#define InjectCAKeyMW(x)        base_func1(BS_FN_CAKEY_INJECT,(DWORD)(x))

#ifndef K_RsaModulus4kBits
#define InjectCAKey2MW(x,y)      (FALSE)
#else
// BOOLEAN InjectCAKey2MW(DWORD aType, void *aKey)
#define InjectCAKey2MW(x,y)      base_func2(BS_FN_CAKEY_INJECT2,(DWORD)(x),(DWORD)(y))
#endif

//=> Secur Channel (Aux Port) use with PP300
//BOOLEAN SChnlSetup(char *aDevStr, BOOLEAN aInjectNewKey);
#define SChnlSetupMW(x, y)    (BOOLEAN) lib_app_call(MIDWARE_ID, SCHNLSETUPMW, (DWORD)x, (DWORD)y, 0)

// GetPin Mode Support
#define MW_PIN_DEF    0x00
#define MW_PIN_CUP    0x10
#define MW_PIN_BJ     0x20
#define MW_PIN_SZEPS  0x30
#define MW_PIN_HKEPS1 0x40
#define MW_PIN_HKEPS2 0x50
#define MW_PIN_BOC    0x60

// aIOBlk Data = timeout(1) + mode(1) + dummy(13) + BCD Amount(6) + Prompt Msg(?)
// Refer to PP300 manaual for details (KBD Input PIN section)
//DWORD SChnlGetPin(BYTE *aIOBlk, BYTE aBlkLen);
#define SChnlGetPinMW(x, y)   (DWORD) lib_app_call(MIDWARE_ID, SCHNLGETPINMW, (DWORD)x, (DWORD)y, 0)
//extern int LLSeek(int aFileDesc, DDWORD *aOffSet, int aMode);
#define LLSeekMW(x,y,z)        (int) lib_app_call(MIDWARE_ID, LLSEEKMW, (DWORD)x, (DWORD)y, (DWORD)z)

//----------------------------------------------------------------------------------------
// Obsolete for New Terminal Start
//----------------------------------------------------------------------------------------
#if (R700)
#ifdef SUPPORT_128X64
  //***********************************
  // 128x64 LCD display
  //***********************************
  // Note: some K_SelXXX may not be defined. If compile error, means such font is not supported.
  #define MWCUR_RIGHT             K_CurRight
  #define MWCUR_LEFT              K_CurLeft
  #define MWCUR_DOWN              K_CurDown
  #define MWCUR_UP                K_CurUp
  #define MWDEL_EOL               K_DelEol
  #define MWBELL                  K_Bell
  #define MWBACK_SP               K_BackSp
  #define MWNEW_LINE              K_NewLine
  #define MWCUR_HOME              K_CurHome
  #define MWCLR_HOME              K_ClrHome
  #define MWC_RETURN              K_CReturn
  #define MWPUSH_CURSOR           K_PushCursor
  #define MWPOP_CURSOR            K_PopCursor
  #define MWSEL_SMFONT            K_SelSmFont
  #define MWSEL_BIGFONT           K_SelBigFont
  #define MWSEL_SPFONT            K_SelSpFont
  #define MWSEL_SPCHNFONT         K_SelSpChnFont
  #define MWSEL_BIGUSERFONT       K_SelUser8x16
  #define MWSEL_SMUSERFONT        K_SelUser8x8
  #define MWSEL_SPUSERFONT        K_SelUser6x8
  #define MWSEL_BIGUSERFONT_T     K_SelUser8x16T
  #define MWESC                   K_Esc
  #define MWREV_ON                K_RevOn
  #define MWREV_OFF               K_RevOff

  #define MWLINE1        0x00
  #define MWLINE2        0x20
  #define MWLINE3        0x40
  #define MWLINE4        0x60
  #define MWLINE5        0x80
  #define MWLINE6        0xA0
  #define MWLINE7        0xC0
  #define MWLINE8        0xE0

  #if (T800)
    //void PrintfMW(const char *aFormatStr[,aArgument....])
    extern int printfmw(const unsigned char *format, ...);
    #define PrintfMW            printfmw
    #define PutCharMW(x)                lib_app_call(MIDWARE_ID, DISP_PUTCMW, (DWORD)(x), 0, 0)
    #define PutNCharMW(x,y)    {BYTE *ptr = os_malloc(y);\
                                if (ptr != NULL) {\
                                  memcpy(ptr, (BYTE *)x, y);\
                                  lib_app_call(MIDWARE_ID, DISP_PUTNMW, (DWORD)ptr, (DWORD)y, 0);\
                                  os_free(ptr);}}
  #elif (PR608D|R700|T700|TIRO|PR608|T600)
    #define PrintfMW            printf
    #define PutCharMW(x)        sys_func1(OS_FN_DISP_PUTC,(DWORD)(x))
    #define PutNCharMW(x,y)     sys_func2(OS_FN_DISP_PUTN,(DWORD)(x),(DWORD)(y))
  #else
    NEW_TERMINAL_HERE {}
  #endif

  #define DispFlushMW()               sys_func1(OS_FN_DISP_PUTC,(DWORD)(MWCLR_HOME))
  #define DispContSetMW(x)            os_disp_cont(x)
  /*
  icon image bit pattern
  bit 0 - signal 0
  bit 1 - signal 1
  bit 2 - signal 2
  bit 3 - signal 3
  bit 4 - signal 4
  bit 5 - antenna
  bit 6 - left triangle outside
  bit 7 - left triangle inside and right triangle outside
  bit 8 - right triangle inside
  bit 9 - left square outside
  bit 10- left square inside and right square outside
  bit 11- right square inside
  bit 12- left circle outside
  bit 13- left circle inside and right circle outside
  bit 14- right circle inside
  bit 15- left star outside
  bit 16- left star inside and right star outside
  bit 17- right star inside
  bit 18- left down triangle outside
  bit 19- left down triangle inside and right down triangle outside
  bit 20- right down triangle inside
  bit 21- left cross star outside
  bit 22- left cross star inside and right cross star outside
  bit 23- right cross star inside
  bit 24- battery frame
  bit 25- battery level 1
  bit 26- battery level 2
  bit 27- battery level 3
  bit 28- battery level 4
  */
  // DWORD os_disp_icon_set(DWORD aImage);
  #define DispIconSetMW(x)            sys_func1(OS_FN_DISP_ICON_SET,(DWORD)(x))
  // DWORD os_disp_icon_clear(DWORD aImage);
  #define DispIconClearMW(x)          sys_func1(OS_FN_DISP_ICON_CLEAR,(DWORD)(x))
  // void os_backlight_control(DWORD a_state);
  #define DispBlOnOffMW(x)            sys_func1(OS_FN_BACKLIGHT_CONTROL,(DWORD)(x))
  // void os_backlight_time(DWORD a_time);
  #define DispBlTimerSetMW(x)         sys_func1(OS_FN_BACKLIGHT_TIME,(DWORD)(x))
#endif // SUPPORT_128X64

#ifdef SUPPORT_320X240
  //***********************************
  // => 320x240 LCD display
  //***********************************
  // command define
  #define MW_LCUR_RIGHT             0x01
  #define MW_LCUR_LEFT              0x02
  #define MW_LCUR_DOWN              0x03
  #define MW_LCUR_UP                0x04
  #define MW_LDEL_EOL               0x05
  #define MW_LBELL                  0x07
  #define MW_LBACK_SP               0x08
  #define MW_LNEW_LINE              0x0A
  #define MW_LCUR_HOME              0x0B
  #define MW_LCLR_HOME              0x0C
  #define MW_LC_RETURN              0x0D
  #define MW_LPUSH_CURSOR           0x0E
  #define MW_LPOP_CURSOR            0x0F
  #define MW_LSEL_SMFONT            0x10
  #define MW_LSEL_BIGFONT           0x11
  #define MW_LSEL_SPFONT            0x12
  #define MW_LSEL_BIGUSERFONT       0x13
  #define MW_LSEL_SMUSERFONT        0x14
  #define MW_LSEL_SPUSERFONT        0x15
  #define MW_LSEL_BIGUSERFONT_T     0x16
  #define MW_LSET_TXTCOLOR          0x17
  #define MW_LSET_BKCOLOR           0x18
  #define MW_LESC                   0x1B
  #define MW_LREV_ON                0x1C
  #define MW_LREV_OFF               0x1D

  // Line define
  #define MW_LLINE1                 0x01
  #define MW_LLINE2                 0x02
  #define MW_LLINE3                 0x03
  #define MW_LLINE4                 0x04
  #define MW_LLINE5                 0x05
  #define MW_LLINE6                 0x06
  #define MW_LLINE7                 0x07
  #define MW_LLINE8                 0x08
  #define MW_LLINE9                 0x09

  // column define
  #define MW_LCOL1                  0x01
  #define MW_LCOL2                  0x02
  #define MW_LCOL3                  0x03
  #define MW_LCOL4                  0x04
  #define MW_LCOL5                  0x05
  #define MW_LCOL6                  0x06
  #define MW_LCOL7                  0x07
  #define MW_LCOL8                  0x08
  #define MW_LCOL9                  0x09
  #define MW_LCOL10                 0x0A
  #define MW_LCOL11                 0x0B
  #define MW_LCOL12                 0x0C
  #define MW_LCOL13                 0x0D
  #define MW_LCOL14                 0x0E
  #define MW_LCOL15                 0x0F
  #define MW_LCOL16                 0x10
  #define MW_LCOL17                 0x11
  #define MW_LCOL18                 0x12
  #define MW_LCOL19                 0x13
  #define MW_LCOL20                 0x14
  #define MW_LCOL21                 0x15
  #define MW_LCOL22                 0x16
  #define MW_LCOL23                 0x17
  #define MW_LCOL24                 0x18
  #define MW_LCOL25                 0x19
  #define MW_LCOL26                 0x1A

  //void PrintfMW(const char *aFormatStr[,aArgument....])
  //extern int printf(const unsigned char *format, ...);
  #define CPrintfMW                 printf
  // BYTE CPutCharMW(DWORD a_c);
  #define CPutCharMW(x)             sys_func1(OS_FN_DISP_PUTC,(DWORD)(x))
  // void CPutCharMW(void * a_str,DWORD a_count);
  #define CPutNCharMW(x,y)          sys_func2(OS_FN_DISP_PUTN,(DWORD)(x),(DWORD)(y))
  #define CDispFlushMW()            sys_func1(OS_FN_DISP_PUTC,(DWORD)(K_ClrHome))
  #define CDispSetPos(x,y)          sys_func1(OS_FN_DISP_PUTP,(DWORD)(((x<<8)|(y&0xFF))&0xFFFF))

  // DWORD os_disp_brightness(DWORD a_brightness); // 0-7, 7 = brightest
  #define DispBrightMW(x)               sys_func1(OS_FN_DISP_BRIGHTNESS, (DWORD)x)
  // DWORD os_disp_bl_control(DWORD a_time); // 0 = off now, 0xff = on now
  #define DispBLCtrlMW(x)               sys_func1(OS_FN_DISP_BL_CONTROL, (DWORD)x)

  // DWORD os_disp_icon_set(DWORD a_image);
  #define DispIconSetMW(x)            ()
  // DWORD os_disp_icon_clear(DWORD a_image);
  #define DispIconClearMW(x)          ()
  // void os_backlight_control(DWORD a_state);
#endif // SUPPORT_320X240
#endif
struct CGRAPH {
  WORD  w_hstart; // start pixel horizontal start position
  WORD  w_vstart; // start pixel vertical start position
  WORD  w_hwidth; // horizontal pixel width, x
  WORD  w_vwidth; // vertical pixel width, y
  BYTE  sb_data[1];  // pointer to graphical data to be saved or displayed, len = x * y WORD
};

// void DispGetG(struct CGRAPH *aDat);
#define DispGetGMW(x)                 sys_func1(OS_FN_DISP_GETG,(DWORD)(x))
// void DispPutG(struct CGRAPH *aDat);
#define DispPutGMW(x)                 sys_func1(OS_FN_DISP_PUTG,(DWORD)(x))
//----------------------------------------------------------------------------------------
// Obsolete for New Terminal End
//----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Display Functions
//-----------------------------------------------------------------------------
// Line number defines
#define MW_LINE1                 0x0100
#define MW_LINE2                 0x0200
#define MW_LINE3                 0x0300
#define MW_LINE4                 0x0400
#define MW_LINE5                 0x0500
#define MW_LINE6                 0x0600
#define MW_LINE7                 0x0700
#define MW_LINE8                 0x0800
#define MW_LINE9                 0x0900
#ifdef SUPPORT_128X64
  #define MW_NOR_LINESIZE          (16)
  #define MW_MAX_LINESIZE          (21)
  #define MW_MAX_LINE              MW_LINE8
#elif defined SUPPORT_320X240
  #define MW_NOR_LINESIZE          (20)
  #define MW_MAX_LINESIZE          (26)
  #define MW_MAX_LINE              MW_LINE9
#endif

//-----------------------------------
// SU_SMFONT  => LCD=8xNormal Line Size
// SU_BIGFONT => LCD=4xNormal Line Size
// SU_SPFONT  => LCD=8xMax Line Size
#define MW_SMFONT               0x0000
#define MW_BIGFONT              0x1000
#define MW_SPFONT               0x2000
#define MW_SPCHI                0x3000
#define MW_8X16FONT             0x4000
#define MW_8X8FONT              0x5000
#define MW_6X8FONT              0x6000
#define MW_8X16TFONT            0x7000
#define MAX_MW_FONT             0x08

// Control
#define MW_LEFT                  0x00
#define MW_RIGHT                 0x01
#define MW_CENTER                0x02

// Option
#define MW_REVERSE               0x10
#define MW_CLREOL                0x20
#define MW_CLRDISP               0x80

#define FONT_IDX(x)            (x>>12)

// Display Control Character
#define MW_CLR_DISP          K_ClrHome
#define MW_REV_ON            K_RevOn
#define MW_REV_OFF           K_RevOff
#define DispCtrlMW(x)        os_disp_putc(x)
#define ClearDispMW()        os_disp_putc(K_ClrHome)
#define DispPutCMW(x)        os_disp_putc(x)
#define DispPutNCMW(x, y)    os_disp_putn(x,y)

#define PrintfMW            printf
//extern void ClearLineMW(DWORD aLineNo);
//extern void ClrBelowMW(DWORD aLineNo);
// aOffset = SU_LINEx + offset
// aCtrl   = SU_xxFont + Option + Control
//extern void DispGotoMW(DWORD aOffset, DWORD aFont);
//extern void DispLineMW(const BYTE *aMsg, DWORD aOffset, DWORD aCtrl);
#define DispClrLineMW(x)              lib_app_call(MIDWARE_ID, DISP_CLRLINEMW, (DWORD)(x), 0, 0)
#define DispClrBelowMW(x)             lib_app_call(MIDWARE_ID, DISP_CLRBELOWMW, (DWORD)(x), 0, 0)
#define DispGotoMW(x, y)              lib_app_call(MIDWARE_ID, DISP_GOTOMW, (DWORD)x, (DWORD)y, 0)
#define DispLineMW(x, y, z)           {                                     \
  DWORD len = strlen((const char *)x);                                      \
  BYTE *ptr = os_malloc(len+1);                                             \
  if (ptr != NULL) {                                                        \
    memcpy(ptr, x, len);                                                    \
    ptr[len] = 0;                                                           \
    lib_app_call(MIDWARE_ID, DISP_LINEMW, (DWORD)ptr, (DWORD)y, (DWORD)z);  \
    os_free(ptr);                                                           \
  }                                                                         \
}

#define MW_NORMONO_LINESIZE          (16)
#define MW_MAXMONO_LINESIZE          (21)
#define MW_NORCOLOR_LINESIZE         (20)
#define MW_MAXCOLOR_LINESIZE         (26)

#define MW_DISP_NORMAL_MONO        0
#define MW_DISP_COLOR              1
#define MW_DISP_MONO               2

//extern DWORD GetLcdType(void);
#define GetLcdTypeMW()              lib_app_call(MIDWARE_ID, GET_LCD_TYPEMW, 0, 0, 0)



//-----------------------------------------------------------------------------
#define OSCallMW(x,y,z)             lib_app_call(MIDWARE_ID, OS_CALL_MW, x,y,(DWORD)z)
#define BSCallMW(x,y,z)             lib_app_call(MIDWARE_ID, BS_CALL_MW, x,y,(DWORD)z)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMS Functions
//-----------------------------------------------------------------------------
// SMS Coding aDcs
#define MWSMS_7BITS       0x00
#define MWSMS_8BITS       0x04
#define MWSMS_UCS2        0x08

struct MWSMS_INFO {
  BYTE   bTxDcs;
  BYTE   *pbTel;
  BYTE   *pbRxDcs;
  BYTE   *pbRxDateTime;
};

extern int SMSEnableMW(BOOLEAN aCtrl, DWORD aTimeout);
#define SMSEnableMW(x,y)        (int) lib_app_call(MIDWARE_ID, SMS_ENB_MW, x,y,0)

extern int SMSRxMW(struct MWSMS_INFO *aSmsInfo, BYTE *aRxBuf, DWORD aMaxLen);
#define SMSRxMW(x,y,z)          (int) lib_app_call(MIDWARE_ID, SMS_RX_MW, (DWORD)x, (DWORD)y, (DWORD)z)

extern int SMSTxMW(struct MWSMS_INFO *aSmsInfo, BYTE *aMsg, DWORD aLen);
#define SMSTxMW(x,y,z)          (int) lib_app_call(MIDWARE_ID, SMS_TX_MW, (DWORD)x, (DWORD)y, (DWORD)z)

extern int SMSUnReadCntMW(void);
#define SMSUnReadCntMW()        (int) lib_app_call(MIDWARE_ID, SMS_COUNT_MW, 0, 0, 0)

extern int SMSClearAllMW(void);
#define SMSClearAllMW()         (int) lib_app_call(MIDWARE_ID, SMS_CLEAR_ALL_MW,0, 0, 0)


//-----------------------------------------------------------------------------
// Extra Control for TCP Connection
// - tcp connect2/ftp connect2/http connect2
// Refer to basecall.h
//-----------------------------------------------------------------------------
#define TcpXtraCtrlAllocMW()    base_func0(BS_FN_TCP_EXTRA_CONTROL_ALLOC)
#define TcpXtraCtrlFreeMW(x)    base_func1(BS_FN_TCP_EXTRA_CONTROL_FREE,  (DWORD)(x))
#define TcpXtraCtrlResetMW(x)   base_func1(BS_FN_TCP_EXTRA_CONTROL_RESET, (DWORD)(x))
#define TcpXtraCtrlSetMW(x,y,z) base_func3(BS_FN_TCP_EXTRA_CONTROL_SET,   (DWORD)(x), (DWORD)(y), (DWORD)(z))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif //_INC_MIDWARE_H_



