/*
************************************
*       Module : basecall.h        *
*       Name   : TF                *
*       Date   : 11-11-2016        *
************************************
*/
/*
System pack V1.8 17-06-2014
===========================
1. Add 9600bps with V.42 in bs_line speed

System pack V1.5 09-07-2013
===========================
1. increase K_MaxClientCertSize to 6K

System pack V1.4 31-01-2013
===========================
1. Add no verify server cert CA Key option

System pack V1.3 09-01-2013
===========================
1. Increase HTTP and TCP max tx buffer size
2. Http support Client cert auth

System pack V1.1 25-06-2012
===========================
1. In T_TCP_PORT's b_option, add wireless mode support instead of just PPP mode

*/

#ifndef _CALLBASE_H_
#define _CALLBASE_H_

#include "common.h"
extern __pcs DWORD base_func0(DWORD a_func_no);
extern __pcs DWORD base_func1(DWORD a_func_no,DWORD a_param1);
extern __pcs DWORD base_func2(DWORD a_func_no,DWORD a_param1,DWORD a_param2);
extern __pcs DWORD base_func3(DWORD a_func_no,DWORD a_param1,DWORD a_param2,DWORD a_param3);

                /****** IP define ******/

#define K_IpALen                      4       /* IP address length in bytes */
typedef unsigned long IPaddr;                 /* internet address */

                /****** TCP func define ******/
// b_option
#define K_TcpOpenMask                 0x77
#define K_TcpOpenPppMode              0x01
#define K_TcpOpenNoCloseWait          0x02
#define K_TcpOpenSslMode              0x04
#define K_FtpOpenSslCmdMode           0x08  // for ftp func only
#define K_GprsOpenLargeMtu            0x10  // for gprs only
#define K_TcpOpenWl0Mode              K_TcpOpenPppMode
#define K_TcpOpenWl1Mode              0x20
#define K_TcpOpenWl2Mode              0x40
// K_TcpOpenWl2Mode|K_TcpOpenWl1Mode|K_TcpOpenWl0Mode form the new wireless mode
#define K_TcpWlPppMode                0x01

#define K_EOption                     0x80

// b_eoption
#define K_TLS_Func_Bitmask            0x3F
#define K_SslStunnelMode              0x01 /* SSL: ??? */
#define K_NoVerifyServerCert          0x02 /* SSL: skip Server cert verification */
#define K_TLS_Enable_SSL3             0x04 /* SSL: enable SSL/3.0 protocol */
#define K_TLS_Enable_TLS1_0           0x08 /* SSL: enable TLS/1.0 protocol */
#define K_TLS_Enable_TLS1_1           0x10 /* SSL: enable TLS/1.1 protocol */
#define K_TLS_Enable_TLS1_2           0x20 /* SSL: enable TLS/1.2 protocol */

#define getTCP_NIF(tcp_port)  ((((tcp_port)->b_option & 0x60) >> 4) | ((tcp_port)->b_option & 1))
#define setTCP_NIF(tcp_port,n)  ((tcp_port)->b_option = ((tcp_port)->b_option & ~0x61) | (((n) & 6) << 4) | ((n) & 1))

/*
  b_sslidx
    ssl key index
  b_option
    bit0 = PPP channel (old) or wireless channel bit 0 (new)
    bit1 = set => no wait for tcp complete before allow re-connection
    bit2 = ssl mode
    bit3 = ssl used in ftp cmd
    bit4 = using large mtu in gprs
    bit5 = wireless channel bit 1
    bit6 = wireless channel bit 2
    bit7 = set => b_eoption is valid
  b_eoption
    bit0 = SSL Stunnel Mode to allow accept clear random packet from Stunnel
    bit1 = set => no check SSL certificate

*/

#define K_MaxTcp            4
#define K_TcpPortMax        128

// tcp status select to tcp status
#define K_TcpStatus                   0
// tcp status to app
// bit 0-7
#define K_TcpIdleState                0
#define K_TcpConnectingState          1
#define K_TcpConnectedState           2
#define K_TcpClosingState             3
#define K_TcpClosedState              4
#define K_SslConnectedState           0x8000
#define K_TcpAlertState               0x4000

#define K_TcpTxBufSize                K_4BufSize

// tcp status select for tcp tx buffer left
#define K_TcpTxBufLeft                1

// tcp status select for tcp rx buffer use
#define K_TcpRxBufUsed                2

// tcp status select to tcb error status
#define K_TcpErrorStatus              3

// tcp status select to clear tx buffer
#define K_TcpTxBufClear               4

typedef struct {
  IPaddr  d_ip;   // ip stored in big endian
  WORD    w_port; // port stored in little endian or follow cpu
  BYTE    b_sslidx;
  BYTE    b_option;
  BYTE    b_certidx;
  BYTE    b_keyidx;
  BYTE    b_eoption;
  BYTE    s_rfu[1];  // should fill 0's
} T_TCP_PORT;

#define K_TcpMaxDataLen               (K_2BufSize*3+K_TBufSize)
#define K_TcpMaxBuf                   (K_TcpMaxDataLen/K_2BufSize+1)

                /****** UDP func define ******/

// udp status select to udp status
#define K_UdpStatus                   0

// UDP Status select for UDP Rx buffer use
#define K_UdpRxBufUsed                1

// udp status to app
// bit 0-7
#define K_UdpIdleState                0
#define K_UdpConnectedState           1

typedef struct {
  IPaddr  d_ip;   // ip stored in big endian
  WORD    w_port; // port stored in little endian or follow cpu
  BYTE    b_mode;
} T_UDP_PORT;

#define K_UdpOpenServerMode           0x01
#define K_UdpOpenPppMode              0x02  // 0x20   Overlap with NIF device code. i.e. K_NIF_XXXX. e.g. K_NIF_WIFI, K_NIF_WIMAX. Harris 20161124.
// NIF number bits
#define K_UDPOPEN_NIF0    0x20
#define K_UDPOPEN_NIF1    0x40
#define K_UDPOPEN_NIF2    0x80
#define getUDP_NIF(udp_port)  ((udp_port)->b_mode >> 5)
#define setUDP_NIF(udp_port,n)  ((udp_port)->b_mode = ((udp_port)->b_mode & 0x1f) | ((n) << 5))

// udp app interface
#if (__IAR_SYSTEMS_ICC__)
#pragma pack(1)
#endif
typedef struct  {                 /* datagram (not UDP) */
  IPaddr  d_xg_fip;               /* foreign host IP address */
  WORD    w_xg_fport;             /* foreign UDP port number */
  BYTE    s_xg_data[1];           /* K_UMaxLen maximum data to/from UDP */
}
#if (!__IAR_SYSTEMS_ICC__)
__attribute__((packed))
#endif
T_XGRAM;
#if (__IAR_SYSTEMS_ICC__)
#pragma pack()
#endif

#define K_XgramHLen                   (2+K_IpALen)

                /****** CAKEY func define ******/

#define K_CaKeyUserIdx1               0xff
#define K_CaKeyUserIdx2               0xfe
#define K_CaKeyUserIdx3               0xfd
#define K_CaKeyUserIdx4               0xfc

#define K_MaxClientCertSize     (1024*6)

typedef struct  {
  DWORD d_cert_idx;
  DWORD d_size;
  BYTE  s_cert[K_MaxClientCertSize];
} T_CLIENT_CERT;


                /****** ICMP func define ******/

#define K_IcmpPppMode                 0x01

                /****** NIF func define ******/
#define K_MaxDns                      2

typedef struct {
  IPaddr  d_ip;             // stored in big endian
  IPaddr  d_netmask;        // stored in big endian
  IPaddr  d_gateway;        // stored in big endian
  IPaddr  s_dns[K_MaxDns];  // stored in big endian
} T_NIF_INFO;

typedef struct {
  IPaddr  d_ip;             // stored in big endian
  IPaddr  d_netmask;        // stored in big endian
  IPaddr  d_gateway;        // stored in big endian
  IPaddr  s_dns[K_MaxDns];  // stored in big endian
  BYTE    MACAddress[6];
  BYTE    DHCP_Enabled;
  BYTE    nif_type;     // net interface type, defined in system.h
  unsigned int dev_id;
  unsigned int bus_id;
  char name[64];
} T_NIF_INFO2;

#define K_NIF_ETHERNET                0
#define K_NIF_PPP                     1
#define K_NIF_MOBILE                  2
#define K_NIF_WIFI                    3
#define K_NIF_BLUETOOTH               4
#define K_NIF_WIMAX                   5
#define K_NIF_EXTERNAL_DEV            6

                /****** Phone func define ******/

// phone detection status
#define   K_PhoneOnhook               0
#define   K_PhoneOffhook              1
#define   K_PhoneNoline               2
#define   K_PhoneDetectNotEnable      3

                /****** Call ID func define ******/
// mode
// for conexant
#define K_CidModeDisable              0
#define K_CidModeEnable               1
// for silab
#define K_CidModeNone                 0
#define K_CidModeUsa                  1
#define K_CidModeUk                   2
#define K_CidModeJapan                3

#define K_TelLenMax                   (34+32)

                /****** line dial func define ******/

typedef struct {
  BYTE b_protocol;               /* Default SDLC */
                                 /* 0 - Visa1 */
                                 /* 1 - SDLC  */
                                 /* 2 - Async */
  BYTE b_ccitt;                  /* 0 - Bell */
                                 /* 1 - CCITT */
                                 /* 2 - Hypercom Special 1200 */
  BYTE b_speed;                  /* 0 - 300bps */
                                 /* 1 - 1200bps fast*/
                                 /* 2 - 2400bps */
                                 /* 3 - 9600bps fast*/
                                 /* 4 - auto */
                                 /* 5 - auto with V.42 & MNP enable */
                                 /* 6 - auto with V.42 & MNP enable and using circular */
                                 /*     buffer supporting Unionpay STX message format in RX */
                                 /* 7 - 14400bps */
                                 /* 8 - 33600bps */
                                 /* 9 - 56000bps */
                                 /* 10- 9600bps  */
                                 /* bit 7 set for support Unionpay STX message format */
  BYTE b_inactivity_tval;        /* in unit of 2.5 sec */
  BYTE b_redial_pause_tval;      /* Pause time between redial in 100 msec*/
  BYTE b_ptel_len;               /* Primary telno length */
  BYTE s_ptel[K_TelLenMax];      /* Prime telno */
  BYTE b_pconn_limit;            /* # of connection attempt */
  BYTE b_pconn_tval;             /* Time allowed to wait CD in 2 sec unit */
  BYTE b_stel_len;               /* Secondary telno length */
  BYTE s_stel[K_TelLenMax];      /* Secondary telno */
  BYTE b_sconn_limit;            /* # of connection attempt */
  BYTE b_sconn_tval;             /* Time allowed to wait CD in 2 sec unit */
  BYTE b_cd_on_qualify_tval;     /* cd on qualify time in 0.25 sec,default 3 */
  BYTE b_cd_off_qualify_tval;    /* cd off qualify time in 0.25 sec,default 4 */
  BYTE b_message_qualify_tval;   /* Message qualify time in 10msec, default 10 */
  BYTE b_wait_online_tval;       /* wait for link online in sec, default 15 */
  BYTE b_tone_duration;          /* tone duration in 10msec */
  BYTE b_holdline_tval;          /* hold line time in second */
  BYTE b_detect_mode;            /* bit 0 - 0 = busy tone detection
                                            1 = disable busy tone detection
                                    bit 1 - 0 = check line before dial
                                            1 = no check line
                                    bit 2 - 0 = blind dialing disable
                                            1 = blind dialing enable
                                    bit 3 - 0 = normal dial tone duration
                                            1 = short dial tone duration
                                    bit 4 - 0 = off hook with loop current test
                                            1 = disable loop current test, not use
                                    bit 5 - 0 = enable Line in use detection
                                            1 = disable Line in use detection
                                    bit 6 - 0 = enable Extension pickup detection
                                            1 = disable Extension pickup detection
                                    bit 7 - 0 = enable Remote hangup detection
                                            1 = disable Remote hangup detection
                                 */
} T_LINE_CONFIG;

// b_ccitt
#define K_LineBell                    0x00
#define K_LineCcitt                   0x01
#define K_LineHypercomSpecial         0x02

// b_detect_mode
#define K_LineDisBusyToneDetect       0x01
#define K_LineNoCheckLine             0x02
#define K_LineBlindDial               0x04
#define K_LineShortDialTone           0x08
#define K_LineNoLoopCurrentDetect     0x10
#define K_LineDisLineInUseDetect      0x20
#define K_LineDisExtPickupDetect      0x40
#define K_LineDisRemoteHangupDetect   0x80
// bs_line_status(DWORD a_service)
#define K_LineStatus                  0
  // line status = dialing state + sticky status
  // bit 0-7 - dialing state
  #define K_LineIdle                  0
  #define K_LineDialingP1             1
  #define K_LineDialingP2             2
  #define K_LineConnectingP1          3
  #define K_LineConnectingP2          4
  #define K_LineHostConnected         5
  #define K_LineCdLost                0x80
  #define K_LineHostOffline           0x81
  #define K_LineInactivityTimeout     0x82
  #define K_LineConnectionFail        0x83
  #define K_LineConnectTimeout        0x84

  // sticky status, cleared after being read
  #define K_LineOccupied              0x00000100
  #define K_LineNoLine                0x00000200
  #define K_LineNoDialTone            0x00000400
  #define K_LineWrongNo               0x00000800
  #define K_LineBusy                  0x00001000
  #define K_LineNoCd                  0x00002000
  #define K_LineConnectFail           0x00004000
  #define K_LineMdmBusy               0x00008000
  #define K_LineMsgTxed               0x00010000
  #define K_LineWaitDisc              0x00020000
  #define K_LineWaitDial              0x00040000
  #define K_LineReady                 0x00080000  // line not in mdm reset or disconnect

#define K_LineOwner                  1

// line disc holdline
#define K_LineHoldLine                1
#define K_LineNoHoldLine              0

                /****** ppp func define ******/

#define K_PppDeviceLine               1
#define K_PppDeviceAux1               2
#define K_PppDeviceAuxdbg             3
#define K_PppDeviceGprs               4
#define K_PppDeviceOpenNeg            0x80
#define K_PppDeviceFixIp              0x40

// PPP mode
/* physical::type values (OR'd in bundle::phys_type) */
#define K_PppModeNone                0
#define K_PppModeInteractive          1   /* Manual link */
#define K_PppModeNormal               2
#define K_PppModeDirect               4   /* already established */
#define K_PppModeDDial                8   /* Dial immediately, stay connected */
#define K_PppModeAll                  15

// ppp phase
#define K_PppIdle                     -1  /* idle not open and own by any owner */
#define K_PppPhaseDead                0   /* Link is dead */
#define K_PppPhaseEstablish           1   /* Establishing link */
#define K_PppPhaseAuthenticate        2   /* Being authenticated */
#define K_PppPhaseNetwork             3   /* We're alive ! */
#define K_PppPhaseTerminate           4   /* Terminating link */
#define K_PppPhaseReady               5   /* ppp is in a usable state */

/*
Expect-Send struct
It is possible to specify some special `values' in the expect-send script as follows:
  \c  When used as the last character in a `send' string, this indicates that a newline
      should not be appended.
  \d  When the chat script encounters this sequence, it delays two seconds.
  \p  When the chat script encounters this sequence, it delays for one quarter of a second.
  \T  This is replaced by the current phone number (see ``set phone'' below).
  \P  This is replaced by the current authkey value (see ``set authkey'' above).
  \U  This is replaced by the current authname value (see ``set authname'' above).
  \j<r1[r2][r3]...>  Reletive jump to other expect_send pair.
      if jump to the location grater than total expect-send, script done.
      if jump to location less than 0, script failed.
*/
// script contains "expect-send" pair strings for modem communication
typedef struct {
  char ** p_expect;     // expect receive strings, the last one must NULL
  WORD  w_min_len;      // min. expect len
  WORD  w_max_len;      // max. expect len
  signed char * p_send; // reply string
  char * p_abort;       // expect receive abort string
  short w_timeout;      // timeout for expect
  BYTE  b_retry;        // no. of retry
  char  b_flags;        // flags
} T_EXPECT_SEND;

#define K_DefaultScript               0xff    // use default script when no of pair = 0xff
#define K_GprsFixSpeed                115200

                /****** ftp func define ******/

// ftp status select ftp status
#define K_FtpStatus                   0
// bit 7  = indicates ftp cmd Q is not empty
// bit0-6 = ftp status
#define K_FtpIdle                     0
#define K_FtpConnecting               1
#define K_FtpConnected                2
#define K_FtpOpening                  3
#define K_FtpOpened                   4
#define K_FtpDataConnecting           5
#define K_FtpDataConnected            6
#define K_FtpClosing                  7
// get back ftp control channel tcp status
#define K_FtpCtrlChnTcpStatus         1
// get back ftp data channel tcp status
#define K_FtpDataChnTcpStatus         2
// get back ftp control channel tcp tx buffer left
#define K_FtpCtrlChnTcpTxBufLeft      3
// get back ftp data channel tcp tx buffer left
#define K_FtpDataChnTcpTxBufLeft      4
// get back ftp control channel tcp rx buffer used
#define K_FtpCtrlChnTcpRxBufUsed      5
// get back ftp data channel tcp rx buffer used
#define K_FtpDataChnTcpRxBufUsed      6


                /****** udp & tcp open status define ******/
#define K_NoProcess                   0xff


                /****** DNS define ******/
#define K_DnsDefaultPort              53
#define K_DnsMaxLen                   300

typedef struct {
  BYTE    s_name[K_DnsMaxLen];
  IPaddr  d_ip;       // DNS server IP in big endian
  WORD    w_port;     // DNS server port in little endian
  WORD    w_timeout;  // response timeout in unit 10msec, 0 = default 1000 for 10sec
} T_DNS_REQ;

                /****** HTTP define ******/
#define K_HttpDefaultPort             80
#define K_HttpsDefaultPort            443

#define K_HttpVersion1_0              0
#define K_HttpVersion1_1              1

#define K_HttpGet                     0
#define K_HttpPost                    1

#define K_HttpMaxHostLen              K_DnsMaxLen
#define K_HttpMaxTypeLen              200
#define K_HttpMaxDataLen              (K_2BufSize*3)

#define K_HttpResponseHeaderMax       (K_2BufSize-4)

typedef struct {
  IPaddr  d_dns_ip;
  WORD    w_dns_port;
  WORD    w_host_port;
  BYTE    s_host[K_HttpMaxHostLen];
  BYTE    b_sslidx;   // same as tcp
  BYTE    b_option;   // same as tcp
  BYTE    b_version;  // http version
  BYTE    b_certidx;  // client cert idx
  BYTE    b_keyidx;   // client cert private key idx
  BYTE    b_eoption;  // same as tcp
  BYTE    b_rfu[1];
} T_HTTP_INFO;
#define getHTTP_NIF getTCP_NIF
#define setHTTP_NIF setTCP_NIF

typedef struct {
  WORD    w_req_len;
  char    *p_uri;
  BYTE    b_method;
  BYTE    b_close;    // set pack close http connection at the end
  WORD    w_max_rlen; // max rx buffer length
  WORD    w_timeout;  // response timeout in unit 10msec, 0 = default 1000 for 10sec
  BYTE    s_type[K_HttpMaxTypeLen];
} T_HTTP_REQ;

typedef struct {
  WORD    w_req_len;
  BYTE    b_method;
  BYTE    b_close;    // set pack close http connection at the end
  WORD    w_max_rlen; // max rx buffer length
  WORD    w_timeout;  // response timeout in unit 10msec, 0 = default 1000 for 10sec
  char    *p_uri;
  char    *p_cust_hdr;
  BYTE    s_type[K_HttpMaxTypeLen];
} T_HTTP_REQ2;

// http status select http status
#define K_HttpStatus                  0
// bit0-7 = http status
#define K_HttpIdle                    0
#define K_HttpOpened                  1
#define K_HttpDnsResolve              2
#define K_HttpConnecting              3
#define K_HttpConnected               4
#define K_HttpRequested               5
#define K_HttpDisconnecting           6


enum {
  BS_FN_GET_VERSION,
  BS_FN_TCP_OPEN,
  BS_FN_TCP_CLOSE,
  BS_FN_TCP_CONNECT,
  BS_FN_TCP_DISCONNECT,
  BS_FN_TCP_SEND,
  BS_FN_TCP_READ,
  BS_FN_TCP_STATUS,
  BS_FN_UDP_OPEN,
  BS_FN_UDP_CLOSE,
  BS_FN_UDP_CONNECT,
  BS_FN_UDP_DISCONNECT,
  BS_FN_UDP_SEND,
  BS_FN_UDP_READ,
  BS_FN_UDP_STATUS,
  BS_FN_CAKEY_INJECT,
  BS_FN_ICMP_CONFIG,
  BS_FN_ICMP_SEND,
  BS_FN_ICMP_READ,
  BS_FN_NET_NIF_INFO,
  BS_FN_PHONE_DETECT,
  BS_FN_PHONE_LINE,
  BS_FN_PHONE_DIAL,
  BS_FN_CID_DETECT,
  BS_FN_CID_READ,
  BS_FN_LINE_OPEN,
  BS_FN_LINE_CLOSE,
  BS_FN_LINE_DIAL,
  BS_FN_LINE_DISC,
  BS_FN_LINE_SEND,
  BS_FN_LINE_READ,
  BS_FN_LINE_STATUS,
  BS_FN_PPP_OPEN,
  BS_FN_PPP_CLOSE,
  BS_FN_PPP_SET_DEVICE,
  BS_FN_PPP_SET_MTU,
  BS_FN_PPP_SET_MODE,
  BS_FN_PPP_GET_SPEED,
  BS_FN_PPP_SET_SPEED,
  BS_FN_PPP_GET_PHASE,
  BS_FN_PPP_SET_LOGIN_ID,
  BS_FN_PPP_SET_DIAL_SC,
  BS_FN_PPP_SET_ID_STR,
  BS_FN_PPP_SET_LOGIN_SCRIPT,
  BS_FN_PPP_SET_PHONES,
  BS_FN_PPP_SET_DIAL_PARAM,
  BS_FN_PPP_SET_REDIAL,
  BS_FN_PPP_SET_RECONNECT,
  BS_FN_PPP_LOGIN_READ,
  BS_FN_PPP_LOGIN_WRITE,
  BS_FN_PPP_LOGIN_FINISH,
  BS_FN_PPP_SET_TIMEOUT,
  BS_FN_PPP_UPTIME,
  BS_FN_PPP_SET_ALIVE_TIME,
  BS_FN_PPP_GET_STATUS,
  BS_FN_FTP_OPEN,
  BS_FN_FTP_CLOSE,
  BS_FN_FTP_CONNECT,
  BS_FN_FTP_DISCONNECT,
  BS_FN_FTP_DATA_SEND,
  BS_FN_FTP_DATA_READ,
  BS_FN_FTP_CMD_SEND,
  BS_FN_FTP_CMD_READ,
  BS_FN_FTP_STATUS,
  BS_FN_LAN_OPEN,
  BS_FN_LAN_CLOSE,
  BS_FN_LAN_STATUS,
  BS_FN_DNS_OPEN,
  BS_FN_DNS_CLOSE,
  BS_FN_DNS_REQUEST,
  BS_FN_DNS_RESPONSE,
  BS_FN_HTTP_OPEN,
  BS_FN_HTTP_CLOSE,
  BS_FN_HTTP_CONNECT,
  BS_FN_HTTP_DISCONNECT,
  BS_FN_HTTP_REQUEST,
  BS_FN_HTTP_RESPONSE,
  BS_FN_HTTP_STATUS,
  BS_FN_CLIENTCERT_INJECT,
  BS_FN_PRIVATEKEY_INJECT,
  BS_FN_NET_NIF_INFO2,
  BS_FN_NET_ENABLE,
  BS_FN_NET_DISABLE,
  BS_FN_NET_STATUS,
  BS_FN_NET_ENQUEUE,
  BS_FN_NET_DEQUEUE,
  BS_FN_PPP_GET_NIF,
  BS_FN_HTTP_REQUEST2,
  BS_FN_NET_IOCTL,
  BS_FN_TCP_CONNECT2,
  BS_FN_TCP_EXTRA_CONTROL_ALLOC,
  BS_FN_TCP_EXTRA_CONTROL_FREE,
  BS_FN_TCP_EXTRA_CONTROL_RESET,
  BS_FN_TCP_EXTRA_CONTROL_SET,
  BS_FN_HTTP_CONNECT2,
  BS_FN_FTP_CONNECT2,
  BS_FN_TCP_SET_LINE_WINDOW_SIZE,
  BS_FN_TCP_SAVE_TLS_SESSION_STATE,
  BS_FN_NET_SET_NIF_IP,
  BS_FN_TCP_WIFI_SOCKET_RECEIVE,
  BS_FN_UDP_WIFI_SOCKET_RECEIVE,
  BS_FN_UDP_WIFI_SOCKET_UPDATE_STATE,
  BS_FN_HTTP_RESPONSE_HEADER,
#if 0
// testing
  BS_FN_HMAC_SHA1_INIT,
  BS_FN_HMAC_SHA1_PROCESS,
  BS_FN_HMAC_SHA1_FINISH,
  BS_FN_HMAC_SHA256_INIT,
  BS_FN_HMAC_SHA256_PROCESS,
  BS_FN_HMAC_SHA256_FINISH,
  BS_FN_HMAC_SHA384_INIT,
  BS_FN_HMAC_SHA384_PROCESS,
  BS_FN_HMAC_SHA384_FINISH,
  BS_FN_HMAC_SHA512_INIT,
  BS_FN_HMAC_SHA512_PROCESS,
  BS_FN_HMAC_SHA512_FINISH,
#endif
#if 0
// testing
  BS_FN_GCM_AES_INIT,
  BS_FN_GCM_AES_ENCRYPT,
  BS_FN_GCM_AES_DECRYPT,
  BS_FN_GCM_AES_CLEANUP,
#endif
  BS_FN_MAX,
};
                /****** base func routine ******/

// void bs_get_version(BYTE * a_ptr);
#define bs_get_version(x)             base_func1(BS_FN_GET_VERSION,(DWORD)(x))

// DWORD base_tcp_open(void);
#define bs_tcp_open()                 base_func0(BS_FN_TCP_OPEN)

// DWORD base_tcp_close(DWORD a_handle);
#define bs_tcp_close(x)               base_func1(BS_FN_TCP_CLOSE,(DWORD)(x))

// DWORD base_tcp_connect(DWORD a_handle,T_TCP_PORT * a_tcp);
#define bs_tcp_connect(x,y)           base_func2(BS_FN_TCP_CONNECT,(DWORD)(x),(DWORD)(y))

// DWORD bs_tcp_connect2(DWORD a_handle, T_TCP_PORT *a_tcp, DWORD a_exctrl)
#define bs_tcp_connect2(x,y,z)        base_func3(BS_FN_TCP_CONNECT2,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD base_tcp_disconnect(DWORD a_handle);
#define bs_tcp_disconnect(x)          base_func1(BS_FN_TCP_DISCONNECT,(DWORD)(x))

// DWORD base_tcp_send(DWORD a_handle,BYTE * a_buf,DWORD a_len);
#define bs_tcp_send(x,y,z)            base_func3(BS_FN_TCP_SEND,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD base_tcp_read(DWORD a_handle,BYTE * a_buf);
#define bs_tcp_read(x,y)              base_func2(BS_FN_TCP_READ,(DWORD)(x),(DWORD)(y))

// DWORD base_tcp_status(DWORD a_handle,DWOD a_select,T_TCP_PORT * a_tcp);
#define bs_tcp_status(x,y,z)          base_func3(BS_FN_TCP_STATUS,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD base_tcp_set_line_window_size(WORD a_size)
#define bs_tcp_set_line_window_size(x) base_func1(BS_FN_TCP_SET_LINE_WINDOW_SIZE,(DWORD)(x))

// DWORD base_udp_open(void);
#define bs_udp_open()                 base_func0(BS_FN_UDP_OPEN)

// DWORD base_udp_close(DWORD a_handle);
#define bs_udp_close(x)               base_func1(BS_FN_UDP_CLOSE,(DWORD)(x))

// DWORD base_udp_connect(DWORD a_handle,T_UDP_PORT * a_udp);
#define bs_udp_connect(x,y)           base_func2(BS_FN_UDP_CONNECT,(DWORD)(x),(DWORD)(y))

// DWORD base_udp_disconnect(DWORD a_handle);
#define bs_udp_disconnect(x)          base_func1(BS_FN_UDP_DISCONNECT,(DWORD)(x))

// DWORD base_udp_send(DWORD a_handle,BYTE * a_buf,DWORD a_len);
#define bs_udp_send(x,y,z)            base_func3(BS_FN_UDP_SEND,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD base_udp_read(DWORD a_handle,BYTE * a_buf);
#define bs_udp_read(x,y)              base_func2(BS_FN_UDP_READ,(DWORD)(x),(DWORD)(y))

// DWORD base_udp_status(DWORD a_handle,DWORD a_select);
#define bs_udp_status(x,y)            base_func2(BS_FN_UDP_STATUS,(DWORD)(x),(DWORD)(y))

// BOOLEAN base_cakey_inject(T_KEY * a_key);
#define bs_cakey_inject(x)            base_func1(BS_FN_CAKEY_INJECT,(DWORD)(x))

// DWORD base_icmp_config(DWORD a_ip,DWORD a_control)
// a_control
// bit 0 => PPP mode
#define bs_icmp_config(x,y)           base_func2(BS_FN_ICMP_CONFIG,(DWORD)(x),(DWORD)(y))

// DWORD base_icmp_send(DWORD a_ip,BYTE * a_buf,DWORD a_len)
#define bs_icmp_send(x,y,z)           base_func3(BS_FN_ICMP_SEND,(DWORD)(x),(DWORD)(y),(DWORD)(z))

// DWORD base_icmp_read(BYTE * a_buf)
#define bs_icmp_read(x)               base_func1(BS_FN_ICMP_READ,(DWORD)(x))

// DWORD base_net_nif_info(DWORD a_nif, T_NIF_INFO * a_pniff)
#define bs_net_nif_info(x,y)          base_func2(BS_FN_NET_NIF_INFO,(DWORD)(x),(DWORD)(y))

// int base_net_nif_info2(DWORD a_nif, T_NIF_INFO2 * a_pniff)
#define bs_net_nif_info2(x,y)   (int)base_func2(BS_FN_NET_NIF_INFO2,(DWORD)(x),(DWORD)(y))

// int base_net_enable(DWORD nif) // return nif number, or error code
#define bs_net_enable(x)            (int)base_func1(BS_FN_NET_ENABLE,(DWORD)(x))

// int base_net_disable(DWORD nif)  // return nif number disabled, or error code
#define bs_net_disable(x)           (int)base_func1(BS_FN_NET_DISABLE,(DWORD)(x))

// int base_net_status(DWORD nif) // return net status (same as bs_lan_status) or error code
#define bs_net_status(x)            (int)base_func1(BS_FN_NET_STATUS,(DWORD)(x))

// int base_net_enqueue(T_NETIF *pnif, T_BUF *) // enqueue data from driver to bottom of protocol stack
#define bs_net_enqueue(x,y)         (int)base_func2(BS_FN_NET_ENQUEUE,(DWORD)(x), (DWORD)(y))

// T_BUF *base_net_dequeue(T_NETIF *pnif) // dequeue data from bottom of protocol stack to driver
#define bs_net_dequeue(x)         (T_BUF *)base_func1(BS_FN_NET_DEQUEUE,(DWORD)(x))

// int base_net_ioctl(DWORD nif, unsigned int cmd, void *param)
#define bs_net_ioctl(x,y,z) (int)base_func3(BS_FN_NET_IOCTL, (DWORD)(x), (DWORD)(y), (DWORD)(z))

// DWORD base_phone_detect(DWORD a_state);
#define bs_phone_detect(x)        base_func1(BS_FN_PHONE_DETECT,(DWORD)(x))

//  DWORD base_phone_line(void);
#define bs_phone_line()           base_func0(BS_FN_PHONE_LINE)

//  DWORD base_phone_dial(char * a_dial_str);
#define bs_phone_dial(x)          base_func1(BS_FN_PHONE_DIAL,(DWORD)(x))

//  DWORD base_cid_detect(DWORD a_mode);
#define bs_cid_detect(x)          base_func1(BS_FN_CID_DETECT,(DWORD)(x))

//  DWORD base_cid_read(BYTE * a_buf); min 64-byte buffer must be reserved
#define bs_cid_read(x)            base_func1(BS_FN_CID_READ,(DWORD)(x))

//  DWORD base_line_open(void);
#define bs_line_open()                base_func0(BS_FN_LINE_OPEN)

//  void base_line_close(void);
#define bs_line_close()               base_func0(BS_FN_LINE_CLOSE)

//  DWORD base_line_dial(T_LINE_CONFIG * a_pconfig);
#define bs_line_dial(x)               base_func1(BS_FN_LINE_DIAL,(DWORD)(x))

//  DWORD base_line_disc(DWORD a_holdline);
#define bs_line_disc(x)               base_func1(BS_FN_LINE_DISC,(DWORD)(x))

//  DWORD base_line_send(BYTE * a_buf,DWORD a_len);
#define bs_line_send(x,y)             base_func2(BS_FN_LINE_SEND,(DWORD)(x),(DWORD)(y))

//  DWORD base_line_read(BYTE * a_buf);
#define bs_line_read(x)               base_func1(BS_FN_LINE_READ,(DWORD)(x))

//  DWORD base_line_status(DWORD a_service);
#define bs_line_status(x)             base_func1(BS_FN_LINE_STATUS,(DWORD)(x))

//  DWORD base_ppp_open(void);
#define bs_ppp_open()                 base_func0(BS_FN_PPP_OPEN)

//  void base_ppp_close(void);
#define bs_ppp_close()                base_func0(BS_FN_PPP_CLOSE)

//  DWORD base_ppp_set_device(DWORD a_device,DWORD a_speed,DWORD a_id);
#define bs_ppp_set_device(x,y,z)      base_func3(BS_FN_PPP_SET_DEVICE,(DWORD)(x),(DWORD)(y),(DWORD)(z))

//  DWORD base_ppp_set_mtu(DWORD a_mtu);
#define bs_ppp_set_mtu(x)             base_func1(BS_FN_PPP_SET_MTU,(DWORD)(x))

//  DWORD base_ppp_set_mode(DWORD a_mode);
#define bs_ppp_set_mode(x)            base_func1(BS_FN_PPP_SET_MODE,(DWORD)(x))

//  DWORD base_ppp_get_speed(void);
#define bs_ppp_get_speed()            base_func0(BS_FN_PPP_GET_SPEED)

//  void base_ppp_set_speed(DWORD a_sp);
#define bs_ppp_set_speed(x)           base_func1(BS_FN_PPP_SET_SPEED,(DWORD)(x))

//  DWORD base_ppp_get_phase(void);
#define bs_ppp_get_phase()            base_func0(BS_FN_PPP_GET_PHASE)

//  DWORD base_ppp_set_login_id(char * a_u,char * a_p);
#define bs_ppp_set_login_id(x,y)      base_func2(BS_FN_PPP_SET_LOGIN_ID,(DWORD)(x),(DWORD)(y))

//  DWORD base_ppp_set_dialup_script(T_EXPECT_SEND * a_es,DWORD a_n);
#define bs_ppp_set_dialup_script(x,y) base_func2(BS_FN_PPP_SET_DIAL_SC,(DWORD)(x),(DWORD)(y))

//  DWORD base_ppp_set_id_str(char * a_id);
#define bs_ppp_set_id_str(x)          base_func1(BS_FN_PPP_SET_ID_STR,(DWORD)(x))

//  DWORD base_ppp_set_login_script(T_EXPECT_SEND * a_es,DWORD a_n);
#define bs_ppp_set_login_script(x,y)  base_func2(BS_FN_PPP_SET_LOGIN_SCRIPT,(DWORD)(x),(DWORD)(y))

//  DWORD base_ppp_set_phones(char * a_p1,char * a_p2); not support
#define bs_ppp_set_phones(x,y)        base_func2(BS_FN_PPP_SET_PHONES,(DWORD)(x),(DWORD)(y))

//  DWORD base_ppp_set_dial_param(T_LINE_CONFIG * a_dp); not support
#define bs_ppp_set_dial_param(x)      base_func1(BS_FN_PPP_SET_DIAL_PARAM,(DWORD)(x))

//  DWORD base_ppp_set_redial(DWORD * a_redial);
#define bs_ppp_set_redial(x)          base_func1(BS_FN_PPP_SET_REDIAL,(DWORD)(x))

//  DWORD base_ppp_set_reconnect(DWORD a_timewait,DWORD a_retry);
#define bs_ppp_set_reconnect(x,y)     base_func2(BS_FN_PPP_SET_RECONNECT,(DWORD)(x),(DWORD)(y))

//  DWORD base_ppp_login_read(char * a_buf,DWORD a_buf_size);
#define bs_ppp_login_read(x,y)        base_func2(BS_FN_PPP_LOGIN_READ,(DWORD)(x),(DWORD)(y))

//  DWORD base_ppp_login_write(char * a_buf,DWORD a_buf_size);
#define bs_ppp_login_write(x,y)       base_func2(BS_FN_PPP_LOGIN_WRITE,(DWORD)(x),(DWORD)(y))

//  DWORD base_ppp_login_finish(DWORD a_success);
#define bs_ppp_login_finish(x)        base_func1(BS_FN_PPP_LOGIN_FINISH,(DWORD)(x))

//  DWORD base_ppp_set_timeout(DWORD a_timeout,DWORD a_min_up);
#define bs_ppp_set_timeout(x,y)       base_func2(BS_FN_PPP_SET_TIMEOUT,(DWORD)(x),(DWORD)(y))

//  DWORD base_ppp_uptime(void);
#define bs_ppp_uptime()               base_func0(BS_FN_PPP_UPTIME)

//  void base_ppp_set_alive_time(DWORD a_time);
#define bs_ppp_set_alive_time(x)      base_func1(BS_FN_PPP_SET_ALIVE_TIME,(DWORD)(x))

// bit 0-6 = device set
// bit 7 = 0 => PPP closed
//         1 => PPP opened
//  DWORD base_ppp_get_status(void);
#define bs_ppp_get_status()           base_func0(BS_FN_PPP_GET_STATUS)

// DWORD base_ppp_get_nif(void);
#define bs_ppp_get_nif()              base_func0(BS_FN_PPP_GET_NIF)

//  DWORD base_ftp_open(void);
#define bs_ftp_open()                 base_func0(BS_FN_FTP_OPEN)

//  DWORD base_ftp_close(void);
#define bs_ftp_close()                base_func0(BS_FN_FTP_CLOSE)

#define K_FtpDataChannel              0
#define K_FtpCmdChannel               1
//  DWORD base_ftp_connect(T_TCP_PORT * a_tcp);
#define bs_ftp_connect(x)             base_func1(BS_FN_FTP_CONNECT,(DWORD)(x))

//  DWORD base_ftp_connect2(T_TCP_PORT * a_tcp, DWORD a_exctrl);
#define bs_ftp_connect2(x,y)          base_func2(BS_FN_FTP_CONNECT2,(DWORD)(x),(DWORD)(y))

//  DWORD base_ftp_disconnect(DWORD a_which);
#define bs_ftp_disconnect(x)          base_func1(BS_FN_FTP_DISCONNECT,(DWORD)(x))

//  DWORD base_ftp_data_send(BYTE * a_buf,DWORD a_len);
#define bs_ftp_data_send(x,y)         base_func2(BS_FN_FTP_DATA_SEND,(DWORD)(x),(DWORD)(y))

//  DWORD base_ftp_data_read(BYTE * a_buf);
#define bs_ftp_data_read(x)           base_func1(BS_FN_FTP_DATA_READ,(DWORD)(x))

//  DWORD base_ftp_cmd_send(BYTE * a_str);
#define bs_ftp_cmd_send(x)            base_func1(BS_FN_FTP_CMD_SEND,(DWORD)(x))

//  DWORD base_ftp_cmd_read(BYTE * a_buf);
#define bs_ftp_cmd_read(x)            base_func1(BS_FN_FTP_CMD_READ,(DWORD)(x))

//  DWORD base_ftp_status(DWORD a_select);
#define bs_ftp_status(x)              base_func1(BS_FN_FTP_STATUS,(DWORD)(x))

//  DWORD base_lan_open(void);
#define bs_lan_open()                 base_func0(BS_FN_LAN_OPEN)

//  void base_lan_close(void);
#define bs_lan_close()                base_func0(BS_FN_LAN_CLOSE)

#define K_BsLanStatus                 0
#define K_LanPlugIn                   0x01
#define K_LanEnabled                  0x02
#define K_LanInstalled                0x04
#define K_LanSpeed100M                0x08  // 802.3 100Base-TX Ethernet speed
//  DWORD base_lan_status(DWORD a_select);
#define bs_lan_status(x)              base_func1(BS_FN_LAN_STATUS,(DWORD)(x))

//  DWORD base_dns_open(DWORD a_nif);
#define bs_dns_open(x)                base_func1(BS_FN_DNS_OPEN,(DWORD)(x))

//  DWORD base_dns_close(void);
#define bs_dns_close()                base_func0(BS_FN_DNS_CLOSE)

//  DWORD base_dns_request(T_DNS_REQ * a_req);
#define bs_dns_request(x)             base_func1(BS_FN_DNS_REQUEST,(DWORD)(x))

//  DWORD base_dns_response(void);
#define bs_dns_response()             base_func0(BS_FN_DNS_RESPONSE)
// 0.0.0.0 = no response
// 255.255.255.255 = response timeout
// other = response

//  DWORD base_http_open(void);
#define bs_http_open()                base_func0(BS_FN_HTTP_OPEN)

//  DWORD base_http_close(void);
#define bs_http_close()               base_func0(BS_FN_HTTP_CLOSE)

//  DWORD base_http_connect(T_HTTP_INFO *a_http_info);
#define bs_http_connect(x)            base_func1(BS_FN_HTTP_CONNECT,(DWORD)(x))

//  DWORD base_http_connect2(T_HTTP_INFO *a_http_info, DWORD a_tcp_exctrl);
#define bs_http_connect2(x,y)         base_func2(BS_FN_HTTP_CONNECT2,(DWORD)(x),(DWORD)(y))

//  DWORD base_http_disconnect(void);
#define bs_http_disconnect()          base_func0(BS_FN_HTTP_DISCONNECT)

//  DWORD base_http_request(T_HTTP_REQ *a_request,BYTE *a_data,BYTE *a_response);
#define bs_http_request(x,y,z)          base_func3(BS_FN_HTTP_REQUEST,(DWORD)(x),(DWORD)(y),(DWORD)(z))

//  DWORD base_http_request2(T_HTTP_REQ2 *a_request,BYTE *a_data,BYTE *a_response);
#define bs_http_request2(x,y,z)         base_func3(BS_FN_HTTP_REQUEST2,(DWORD)(x),(DWORD)(y),(DWORD)(z))

//  DWORD base_http_response(void);
#define bs_http_response()            base_func0(BS_FN_HTTP_RESPONSE)
// 0 = no response
// -1 = timeout
// -2 = not found
// other = response length

//  DWORD base_http_status(DWORD a_select);
#define bs_http_status(x)             base_func1(BS_FN_HTTP_STATUS,(DWORD)(x))

//  DWORD base_http_response_header(BYTE *a_header)
//    retrieve the HTTP response header of last completed HTTP request.
//    0 if error or no response header record;
//    otherwise, return the no. of bytes copied to destination address (a_header)
#define bs_http_response_header(x)    base_func1(BS_FN_HTTP_RESPONSE_HEADER,(DWORD)(x))

//  BOOLEAN base_clientcert_inject(T_CLIENT_CERT* a_clientcert);
#define bs_clientcert_inject(x)       base_func1(BS_FN_CLIENTCERT_INJECT, (DWORD)(x))

//  BOOLEAN base_privatekey_inject(T_RSA_PRIVATE_KEY a_privatekey);
#define bs_privatekey_inject(x)       base_func1(BS_FN_PRIVATEKEY_INJECT, (DWORD)(x))

// TCP extra feature control ID
typedef enum {
  E_TcpExCtrl_MIN = 0x0000,
  E_TcpExCtrl_SNI,              // control value is a (const char *)
  E_TcpExCtrl_X509_Verify,      // control value see T_TCP_EXCTRL_X509_VERIFY_MODE
  E_TcpExCtrl_Set_TLS_Session,  // set SSL/TLS session state
  E_TcpExCtrl_MAX
} T_TCP_EXCTRL_ID;

// On T1000, default SSL/TLS X.509 certificates verification mode is
// E_TcpExCtrlVal_X509_Verify_1Level.
#define K_TcpExCtrl_DefaultX509VerifyMode  E_TcpExCtrlVal_X509_Verify_1Level
// TCP extra feature control value for ID E_TcpExCtrl_X509_Verify
// This control the way of verifying certificates chain provided by
// remote server peer during a SSL/TLS handshake session.
typedef enum {
  E_TcpExCtrlVal_X509_Verify_MIN = 0, /* place-holder - MIN. value */
  E_TcpExCtrlVal_X509_Verify_1Level , /* parse 1st cert. and verify */
  E_TcpExCtrlVal_X509_Verify_None,    /* parse 1st cert. and don't verify */
  E_TcpExCtrlVal_X509_Verify_Chained, /* chained parsing and verification */
  E_TcpExCtrlVal_X509_Verify_ChainedSelfSigned,
                                      /* same as chained mode, and recognise
                                       * self-signed CA cert being a valid
                                       * root CA certificate. */
  E_TcpExCtrlVal_X509_Verify_ChainedSelfSigned_1Level,
                                      /* special debug mode, similar to
                                       * ChainedSelfSigned.  In additional,
                                       * server's cert will be accepted
                                       * even though it is a self-signed
                                       * root CA certificate. */
  E_TcpExCtrlVal_X509_Verify_MAX      /* place-holder - MAX. value */
} T_TCP_EXCTRL_X509_VERIFY_MODE;

// DWORD bs_tcp_extra_control_alloc(void)
//   Allocate a handle for TCP extra control
//
//   Input -
//     None
//
//   Output or Return Value -
//     A valid TCP extra control handle
#define bs_tcp_extra_control_alloc()    base_func0(BS_FN_TCP_EXTRA_CONTROL_ALLOC)

// BOOLEAN bs_tcp_extra_control_free(DWORD a_extra_control)
//   Release previously allocated TCP extra control back to system.
//
//   Input -
//     a_extra_control    TCP extra control handle ID
#define bs_tcp_extra_control_free(x)    base_func1(BS_FN_TCP_EXTRA_CONTROL_FREE, (DWORD)(x))

// BOOLEAN bs_tcp_extra_control_reset(DWORD a_extra_control)
//   Reset or initailise allocated TCP extra control handle back to
//   system default settings.
//
//   Input -
//     a_extra_control    TCP extra control handle ID
//
//   Output or Return Value -
//     None
#define bs_tcp_extra_control_reset(x)   base_func1(BS_FN_TCP_EXTRA_CONTROL_RESET, (DWORD)(x))

// DWORD bs_tcp_extra_control_set(DWORD a_extra_control, DWORD id, DWORD value)
//   Modify controls embedded in TCP extra control handle.  Unless you are
//   instructed by R&D, please use wrapped up MACROS instead.
//
//   Input -
//     a_extra_control    TCP extra control handle ID
//     id                 control ID
//     value              argument to associated to the control ID
//
//   Output -
//     Various, depending on the control ID concerned.
#define bs_tcp_extra_control_set(x,y,z) base_func3(BS_FN_TCP_EXTRA_CONTROL_SET, (DWORD)(x), (DWORD)(y), (DWORD)(z))

// DWORD bs_tcp_exctrl_set_sni(DWORD a_extra_control, const char* a_server_name)
//   Fix the server name indication value during SSL/TLS handshake session.
//
//   Input -
//     a_extra_control    TCP extra control handle
//     a_server_name      Hostname in ASCII STRING format.  According to
//                        TLS extension decscription (RFC6066), supported
//                        SNI hostname SHOULD BE a A-Label without a trailing
//                        dot.  Details can be found in RFC5890.
//
//   Output or Return Value -
//     TRUE for success; FALSE otherwise.
#define bs_tcp_exctrl_set_sni(x,y)         bs_tcp_extra_control_set((x),E_TcpExCtrl_SNI,(y))

// DWORD bs_tcp_exctrl_set_x509_verify(DWORD a_extra_control,
//                                     T_TCP_EXCTRL_X509_VERIFY_MODE a_mode)
//   Fix targeting verficiation mode to certficiates presented by remote
//   server during a SSL/TLS handshake session.
//
//   a. E_TcpExCtrlVal_X509_Verify_1Level
//      parse 1st certificate and verify it signature against prescribed
//      root CA keys installed in or injected into the system.
//
//   b. E_TcpExCtrlVal_X509_Verify_None
//      parse 1st certificate and DO NOT VERIFY its signature.  This mode
//      is merely for debugging purpose and should not be employed in real
//      application.
//
//   c. E_TcpExCtrlVal_X509_Verify_Chained
//      perform chained certificate parsing and verification.  The last
//      CA certificate's signature will verified against prescribed
//      root CA keys installed in or injected into the system.
//
//   d. E_TcpExCtrlVal_X509_Verify_ChainedSelfSigned
//      Same as E_TcpExCtrlVal_X509_Verify_Chained.  However, if the last
//      CA certificate is a self-signed, this certificate will be trusted
//      by system and its signature WILL NOT BE VERIFIED against any root
//      CA keys installed in or injected into the system.
//
//   Input -
//     a_extra_control    TCP extra control handle
//     a_mode             targeting verfication mode.
//
//   Output or Return Value -
//     TRUE for success; FALSE otherwise.
#define bs_tcp_exctrl_set_x509_verify(x,y) bs_tcp_extra_control_set((x),E_TcpExCtrl_X509_Verify,(y))

// RFC5077 (session ticket extension) defines an opaque ticket structure
// which server passes to client.  In practice, this ticket should be small
// in size and we only support ticket no more than 2048 bytes
#define K_SslSessionTicket_MaxLen  (2048)

typedef struct {
  WORD     w_struct_size;          // sizeof(T_TCP_TLS_SESSION_STATE)
  WORD     w_protocol_version;     // SSL/TLS version
  WORD     w_cipher_suite;         // cipher suite ID
  BYTE     b_compress_method;      // compression method
  BYTE     b_session_id_len;       // length of session ID
  WORD     w_session_ticket_len;   // RFC5077 session ticket length
  WORD     w_rfu_00;
  DWORD    w_rfu_01[5];
  BYTE     s_master_secret[48];    // master secret
  BYTE     s_session_id[32];       // session ID
  // RFC5077 session ticket storage.
  BYTE     s_session_ticket[K_SslSessionTicket_MaxLen];
} T_TCP_TLS_SESSION_STATE;

// DWORD bs_tcp_save_tls_session(DWORD a_handle,
//                               T_TCP_TLS_SESSION_STATE *ap_sess_state)
//
// Snapshot the session state of SSL/TLS enabled TCP connection.  The
// TCP handle must be in SSL/TLS connected state.
//
// Input -
//   a_handle      - TCP socket handle
//   ap_sess_state - TLS session state variable buffer.
//                   ap_sess_state.w_struct_size must be initialized
//                   probably.  This API will check this size field
//                   to prevent memory overwritten/overflow problem.
//
// Output or Return Value -
//   TRUE if success and session state variable will be filled in;
//   FALSE otherwise.
#define bs_tcp_save_tls_session_state(x,y)  base_func2(BS_FN_TCP_SAVE_TLS_SESSION_STATE,(DWORD)(x),(DWORD)(y))

// DWORD bs_tcp_exctrl_set_tls_session(DWORD a_extra_control,
//                                     T_TCP_TLS_SESSION_STATE *ap_sess_state)
#define bs_tcp_exctrl_set_tls_session(x,y)      bs_tcp_extra_control_set((x),E_TcpExCtrl_Set_TLS_Session,(y))

// DWORD bs_tcp_exctrl_clr_tls_session(DWORD a_extra_control)
#define bs_tcp_exctrl_clr_tls_session(x)        bs_tcp_exctrl_set_tls_session((x),(void*)NULL)

//DWORD bs_net_set_nif_ip(DWORD nifnum, T_NIF_INFO *nif_info)
#define bs_net_set_nif_ip(x,y)      base_func2(BS_FN_NET_SET_NIF_IP,(DWORD)(x),(DWORD)(y))

//DWORD bs_tcp_wifi_socket_receive(DWORD a_handle, T_BUF *buf)
#define bs_tcp_wifi_socket_receive(x,y)   base_func2(BS_FN_TCP_WIFI_SOCKET_RECEIVE,(DWORD)(x),(DWORD)(y))

//DWORD bs_udp_wifi_socket_receive(DWORD a_handle, T_BUF *buf)
#define bs_udp_wifi_socket_receive(x,y)   base_func2(BS_FN_UDP_WIFI_SOCKET_RECEIVE,(DWORD)(x),(DWORD)(y))

//int base_udp_wifi_socket_state_update(DWORD a_handle, int state)
#define bs_udp_wifi_socket_update_state(x,y)    base_func2(BS_FN_UDP_WIFI_SOCKET_UPDATE_STATE,(DWORD)(x),(DWORD)(y))

#if 0
// testing
//void hmac_sha1_init(T_HMAC_SHA1_CONTEXT *a_ctx,const BYTE *a_key,int a_key_len)
#define bs_hmac_sha1_init(x,y,z)          base_func3(BS_FN_HMAC_SHA1_INIT,(DWORD)(x),(DWORD)(y),(DWORD)z)

//void hmac_sha1_process(T_HMAC_SHA1_CONTEXT *a_ctx,const BYTE *a_txt,int a_txt_len)
#define bs_hmac_sha1_process(x,y,z)       base_func3(BS_FN_HMAC_SHA1_PROCESS,(DWORD)(x),(DWORD)(y),(DWORD)z)

//void hmac_sha1_finish(T_HMAC_SHA1_CONTEXT *a_ctx, BYTE *a_digest)
#define bs_hmac_sha1_finish(x,y)          base_func2(BS_FN_HMAC_SHA1_FINISH,(DWORD)(x),(DWORD)(y))

//void hmac_sha256_init(T_HMAC_SHA256_CONTEXT *a_ctx,const BYTE *a_key,int a_key_len)
#define bs_hmac_sha256_init(x,y,z)        base_func3(BS_FN_HMAC_SHA256_INIT,(DWORD)(x),(DWORD)(y),(DWORD)z)

//void hmac_sha256_process(T_HMAC_SHA256_CONTEXT *a_ctx,const BYTE *a_txt,int a_txt_len)
#define bs_hmac_sha256_process(x,y,z)     base_func3(BS_FN_HMAC_SHA256_PROCESS,(DWORD)(x),(DWORD)(y),(DWORD)z)

//void hmac_sha256_finish(T_HMAC_SHA256_CONTEXT *a_ctx, BYTE *a_digest)
#define bs_hmac_sha256_finish(x,y)        base_func2(BS_FN_HMAC_SHA256_FINISH,(DWORD)(x),(DWORD)(y))

//void hmac_sha384_init(T_HMAC_SHA384_CONTEXT *a_ctx,const BYTE *a_key,int a_key_len)
#define bs_hmac_sha384_init(x,y,z)        base_func3(BS_FN_HMAC_SHA384_INIT,(DWORD)(x),(DWORD)(y),(DWORD)z)

//void hmac_sha384_process(T_HMAC_SHA384_CONTEXT *a_ctx,const BYTE *a_txt,int a_txt_len)
#define bs_hmac_sha384_process(x,y,z)     base_func3(BS_FN_HMAC_SHA384_PROCESS,(DWORD)(x),(DWORD)(y),(DWORD)z)

//void hmac_sha384_finish(T_HMAC_SHA384_CONTEXT *a_ctx, BYTE *a_digest)
#define bs_hmac_sha384_finish(x,y)        base_func2(BS_FN_HMAC_SHA384_FINISH,(DWORD)(x),(DWORD)(y))

//void hmac_sha512_init(T_HMAC_SHA512_CONTEXT *a_ctx,const BYTE *a_key,int a_key_len)
#define bs_hmac_sha512_init(x,y,z)        base_func3(BS_FN_HMAC_SHA512_INIT,(DWORD)(x),(DWORD)(y),(DWORD)z)

//void hmac_sha512_process(T_HMAC_SHA512_CONTEXT *a_ctx,const BYTE *a_txt,int a_txt_len)
#define bs_hmac_sha512_process(x,y,z)     base_func3(BS_FN_HMAC_SHA512_PROCESS,(DWORD)(x),(DWORD)(y),(DWORD)z)

//void hmac_sha512_finish(T_HMAC_SHA512_CONTEXT *a_ctx, BYTE *a_digest)
#define bs_hmac_sha512_finish(x,y)        base_func2(BS_FN_HMAC_SHA512_FINISH,(DWORD)(x),(DWORD)(y))
#endif

#if 0
// testing
typedef struct {
  /* input arguments */
  const BYTE *a_Key;
  DWORD a_Key_len;
  const BYTE *a_P;
  DWORD a_P_len;
  const BYTE *a_IV;
  DWORD a_IV_len;
  const BYTE *a_AAD;
  DWORD a_AAD_len;
  /* output arguments */
  BYTE *a_C;
  DWORD a_C_len;
  BYTE *a_Tag;
  DWORD a_Tag_len;
} T_ENCRYPT;

typedef struct {
  /* input arguments */
  const BYTE *a_Key;
  DWORD a_Key_len;
  const BYTE *a_C;
  DWORD a_C_len;
  const BYTE *a_IV;
  DWORD a_IV_len;
  const BYTE *a_AAD;
  DWORD a_AAD_len;
  const BYTE *a_Tag;
  DWORD a_Tag_len;
  /* output arguments */
  BYTE *a_P;
  DWORD a_P_len;
} T_DECRYPT;


//int gcm_aes_init(T_GCM_MODE_CONTEXT *a_ctx,T_AES_CONTEXT *a_aes_ctx);
#define bs_gcm_aes_init(x,y)       (int)base_func2(BS_FN_GCM_AES_INIT,(DWORD)(x),(DWORD)(y))

// int gcm_aes_encrypt(T_GCM_MODE_CONTEXT *a_ctx,T_ENCRYPT * a_encrypt);
#define bs_gcm_aes_encrypt(x,y)    (int)base_func2(BS_FN_GCM_AES_ENCRYPT,(DWORD)(x),(DWORD)(y))

// int gcm_aes_decrypt(T_GCM_MODE_CONTEXT *a_ctx,T_DECRYPT * a_decrypt);
#define bs_gcm_aes_decrypt(x,y)    (int)base_func2(BS_FN_GCM_AES_DECRYPT,(DWORD)(x),(DWORD)(y))

// int gcm_aes_cleanup(T_GCM_MODE_CONTEXT *a_ctx);
#define bs_gcm_aes_cleanup(x)      (int)base_func2(BS_FN_GCM_AES_INIT,(DWORD)(x))

#endif
#endif
