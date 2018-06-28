#ifndef UTIL_H
#define UTIL_H

#include "common.h"

extern __pcs DWORD pack_byte(DWORD value);
extern __pcs void pack_nbyte(DWORD value, DWORD len);
extern __pcs void pack_mem(BYTE *source, DWORD len);
extern __pcs void pack_null(DWORD len);
extern __pcs void pack_space(DWORD len);
extern __pcs void pack_zero(DWORD len);
extern __pcs void pack_str(char *string);
extern __pcs DWORD get_byte(void);
extern __pcs DWORD get_word(void);
extern __pcs DWORD get_dword(void);
extern __pcs DWORD get_dword_le(void);
extern __pcs DWORD peek_byte(void);
extern __pcs DWORD inc_pptr(DWORD increment);
extern __pcs DWORD dec_pptr(DWORD decrement);
extern __pcs BYTE *set_pptr(BYTE *ptr,DWORD len);
extern __pcs BYTE *get_pptr(void);
extern __pcs DWORD get_distance(void);
extern __pcs void pack_hex(DWORD value);
extern __pcs DWORD pack_word(DWORD value);
extern __pcs DWORD pack_dword(DWORD value);
extern __pcs DWORD pack_word_le(DWORD value);
extern __pcs DWORD pack_dword_le(DWORD value);
extern __pcs DWORD buffer_overflow(DWORD len);
extern __pcs void pack_lf();
extern __pcs void pack_lfs(DWORD len);
extern __pcs void pack_ff();
extern __pcs DWORD get_mem(BYTE *dest, DWORD len);
extern __pcs void split_data(BYTE *source, DWORD len);
extern __pcs void bindec_data(DWORD value, DWORD len);

extern __pcs DWORD fndb(BYTE *source, DWORD target, DWORD len);
extern __pcs BYTE *fndbptr(BYTE *source, DWORD target, DWORD len);
extern __pcs DWORD skpb(BYTE *source, DWORD target, DWORD len);
extern __pcs BYTE *skpbptr(BYTE *source, DWORD target, DWORD len);
extern __pcs void compress(BYTE *dest, BYTE *source, DWORD pair);
extern __pcs void split(BYTE *dest, BYTE *source, DWORD pair);
extern __pcs void justify_right(BYTE *source, DWORD filler, DWORD len);
extern __pcs DWORD chk_digit(BYTE *ptr, DWORD len);
extern __pcs BOOLEAN chk_digit_ok(BYTE *ptr, DWORD len);
extern __pcs BOOLEAN date_ok(BYTE *dtg);
extern __pcs BOOLEAN leap_year(DWORD year);
extern __pcs BOOLEAN dtg2asc(BYTE *ptr, BYTE *dtg, BOOLEAN century);
extern __pcs DWORD days_of_month(DWORD month);

extern __pcs BOOLEAN parity(DWORD data_byte);
extern __pcs DWORD bit_flip(DWORD data_byte);
extern __pcs DWORD hex_value(DWORD value);
extern __pcs DWORD hex_digit(DWORD value);
extern __pcs DWORD dig2val(DWORD value);
extern __pcs DWORD asc2val(BYTE  *ptr);
extern __pcs BYTE *scanbyte(BYTE *source, DWORD *result);
extern __pcs BYTE  *amount_conv(BYTE  *output, BYTE  *source, DWORD decimal);
extern __pcs BOOLEAN isbdigit(BYTE *ptr, DWORD len);
extern __pcs DWORD cal_crc(BYTE *msg, DWORD len);
extern __pcs DWORD cal_crcx(BYTE *msg, DWORD len,DWORD result);
extern __pcs DWORD vw_crc16(BYTE * aData, DWORD aSize);
extern __pcs DWORD cal_crc_normal(BYTE *msg, DWORD len);
extern __pcs DWORD cal_crcx_normal(BYTE *msg, DWORD len,DWORD result);
extern __pcs DWORD checksum(BYTE * d_ptr, DWORD len);
extern __pcs DWORD lrc(BYTE * d_ptr, DWORD len);
extern __pcs BYTE cal_crc7(BYTE  *msg, DWORD len);
extern __pcs DWORD cal_crc32x(BYTE *msg,int len,DWORD result);
extern __pcs DWORD cal_crc32(BYTE *msg, int len);

extern __pcs void split_30(BYTE *dest,BYTE *src,DWORD len);
extern __pcs void compress_30(BYTE *dest, BYTE *src, DWORD len);
extern __pcs DWORD dec2bin (BYTE *src,DWORD len);
extern __pcs DWORD bcd2val(DWORD bcd);
extern __pcs DWORD bcd2bin (DWORD val);
extern __pcs void bcdinc(BYTE *dest,DWORD len);
extern __pcs DWORD bcdadd(BYTE *dest,BYTE *src,DWORD len);
extern __pcs DWORD bcdsub(BYTE *dest,BYTE *src,DWORD len);
extern __pcs void ascinc(BYTE  *src,DWORD len);
extern __pcs DWORD ascadd(BYTE  *dest, BYTE  *src,DWORD len);
extern __pcs DWORD ascsub(BYTE *dest,BYTE *src,DWORD len);
extern __pcs void lbin2bcd(BYTE * dest, unsigned long val);
extern __pcs void lbin2asc(BYTE *dest,unsigned long val);
extern __pcs DWORD bin2bcd(DWORD val);
extern __pcs void bin2dec(DWORD val,BYTE *dest,DWORD len);
extern __pcs void memxor (BYTE *dest,BYTE *src, DWORD len);
extern __pcs void memor (BYTE *dest,BYTE *src, DWORD len);
extern __pcs void dbin2bcd(BYTE  * dest, unsigned long long val);
extern __pcs void dbin2asc(BYTE * dest, unsigned long long val);

extern __pcs DWORD decbin4b (BYTE * src,DWORD len);
extern __pcs unsigned long long decbin8b (BYTE * src,DWORD len);
extern __pcs DWORD val2bcd(DWORD val);
extern __pcs DWORD swap_w(DWORD w);
extern __pcs DWORD swap_l(DWORD l);

extern __pcs void lbcd2lbin(BYTE *pbcd, BYTE bcd_size, BYTE *pbin);
extern const BYTE month[12][3];

extern __pcs DWORD RTC2UNIX(void * ptr);
extern __pcs void UNIX2RTC(void * ptr,DWORD unix_time);

extern __pcs BYTE cmux_fcs_cal(BYTE *a_msg, DWORD a_len);
extern __pcs BOOLEAN cmux_fcs_check(BYTE *a_msg, DWORD a_len);
extern __pcs DWORD cmux_fcs_byte(DWORD a_data,DWORD a_result);


#endif
