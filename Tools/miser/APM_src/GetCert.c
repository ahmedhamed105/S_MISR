//-----------------------------------------------------------------------------
//  File          : GetCert.c
//  Module        :
//  Description   : Get Cert/key from file.
//  Author        : Kenneth Chan
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
//  10 Jul  2012  Kenneth     Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "midware.h"
#include "apm.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "util.h"
#include "GetCert.h"
#include "x509.h"
#include "pkcs.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant
//-----------------------------------------------------------------------------
const BYTE CAKeyEcr[] = "PARAM0";      // define CA parameter file name for Web-TMS
const BYTE CAKeyHost[] = "PARAM1";      // define CA parameter file name for Web-TMS
const BYTE ClientCert[] = "PARAM2";    // define client cert parameter file name
const BYTE ClientPriKey[] = "PARAM3";  // define client private key parameter file name
const BYTE CAKeyTMS[] = "PARAM9";      // define CA parameter file name for Web-TMS

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

//*****************************************************************************
//  Function        : Str2hex
//  Description     : Convert ASCII into DWORD
//  Input           : source: ASCII string
//                    len: length of string
//  Return          : DWORD value
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD Str2hex(BYTE * source, DWORD len)
{
  int i = 0;
  DWORD val=0;
  BYTE tmp[8];
  for (i=0; i<len; i++)
  {
    tmp[i] = hex_value(source[i]);
    val |= tmp[i];
    if (i < len-1)
      val <<= 4;
  }
  return val;
}

//*****************************************************************************
//  Function        : GetKey
//  Description     : Get key from file
//  Input           : aptr:       CA key pointer
//                    aCAKeyID:   CA key ID
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetKey(struct MW_KEY *aptr, BYTE aCAKeyID)
{
  T_Dir dirLink;
  T_DirEntry *entry;
  struct FILELIST list[10];
  char dirname[] = "/";
  
  int result;
  BYTE *content;
  BYTE filename[20];        // stored filename to be read locally
  int pfile_num = 0;
  int filesize = 0;
  int readlen = 0;
  int read_filehdr = -1;
  T_X509_CERT_INFO cert;
  BYTE buf[32];
  
  if (aCAKeyID >= MAX_CA)
    return FALSE;
  
  switch (aCAKeyID)
  {
    case CA_ECR:
      strcpy(filename, CAKeyEcr);
      break;
    case CA_HOST:
      strcpy(filename, CAKeyHost);
      break;
    case CA_TMS:
      strcpy(filename, CAKeyTMS);
      break;
  }
  
  if (os_file_init_search(dirname, &dirLink) < 0)        // read number of files
    return FALSE;

  do {
    entry = (T_DirEntry *)os_file_readdir(&dirLink);    // read files one by one
    if (entry != NULL) {
      if (!memcmp((char const *)entry->s_name, filename, 6)) {  // read para files only
        memcpy(list[pfile_num].filename, entry->s_name, sizeof(list[pfile_num].filename));
        list[pfile_num].len = entry->d_filelen;
        pfile_num++;
        break;
      }
    }
  } while (entry != NULL);

  memset(filename, 0, sizeof(filename));
  if (pfile_num) {
    pfile_num--;
    memcpy(filename, dirname, 1);
    strcat(filename, list[pfile_num].filename);
    if ((read_filehdr = os_file_open(filename, K_O_RDONLY)) < 0) {
      LongBeep();
      SprintfMW(buf, "Open %s fail!", list[pfile_num].filename);
      DispLineMW(buf, MW_LINE2, MW_LEFT|MW_SMFONT|MW_CLRDISP);
      os_file_close(read_filehdr);
      APM_WaitKey(300, 0);
      return FALSE;
    }
    else {
      /* getting file content */
      filesize = os_file_length(read_filehdr);
      content = (BYTE*) MallocMW(filesize);
      if (content == NULL) {
        os_file_close(read_filehdr);
        return FALSE;
      }
      readlen = os_file_read(read_filehdr, content, filesize);
      os_file_close(read_filehdr);
    }
    
    if (readlen != filesize) {
      FreeMW(content);
      return FALSE;
    }
    
    //21-04-17 JC ++
    //make sure parameter file in cert format before loading
    if (memcmp(content, "\x30\x82", 2)) {
      FreeMW(content);
      return FALSE;
    }
    //21-04-17 JC --

    memset(aptr, 0, sizeof(struct MW_KEY));
    aptr->d_key_idx = 0xFF;              // extract cert index
    result = x509_parse_cert(content, filesize, &cert);
    if (result != K_X509Ok) {
      LongBeep();
      //SprintfMW(buf, "X509 Key Parse Err: %d", result);
      //DispLineMW(buf, MW_LINE2, MW_LEFT|MW_SMFONT|MW_CLRDISP);
      FreeMW(content);
      return FALSE;
    }
    memcpy((T_KEY *)aptr, &cert.s_public_key, sizeof(T_KEY));
    FreeMW(content);
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//  Function        : GetClientCert
//  Description     : Get ClientCert from file
//  Input           : aptr:  Client Cert pointer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetClientCert(T_CLIENT_CERT *aptr)
{
  T_Dir dirLink;
  T_DirEntry *entry;
  struct FILELIST list[10];
  BYTE dirname[] = "/";
  BYTE buf[32];
  
  int i;
  BYTE *content;
  BYTE filename[20];        // stored filename to be read locally
  int pfile_num = 0;
  int filesize = 0;
  int readlen = 0;
  int read_filehdr = -1;
  
  memset(filename, 0, sizeof(filename));
  
  if (os_file_init_search(dirname, &dirLink) < 0)        // read number of files
    return FALSE;

  do {
    entry = (T_DirEntry *)os_file_readdir(&dirLink);    // read files one by one
    if (entry != NULL) {
      if (!memcmp((char const *)entry->s_name, ClientCert, 6)) {  // read para files only
        memcpy(list[pfile_num].filename, entry->s_name, sizeof(list[pfile_num].filename));
        list[pfile_num].len = entry->d_filelen;
        pfile_num++;
        break;
      }
    }
  } while (entry != NULL);

  if (pfile_num)
  {
    pfile_num--;
    memcpy(filename, dirname, 1);
    strcat(filename, list[pfile_num].filename);
    if ((read_filehdr = os_file_open(filename, K_O_RDONLY)) < 0)
    {
      SprintfMW(buf, "%s open fail!", ClientCert);
      DispLineMW(buf, MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
      os_file_close(read_filehdr);
      WaitKey(KBD_TIMEOUT);
      return FALSE;
    }
    else
    {
      /* getting file content */
      filesize = os_file_length(read_filehdr);
      content = (BYTE*) MallocMW(filesize);
      if (content == NULL) {
        os_file_close(read_filehdr);
        return FALSE;
      }
      readlen = os_file_read(read_filehdr, content, filesize);
      os_file_close(read_filehdr);
    }
    
    if (readlen != filesize) {
      FreeMW(content);
      return FALSE;
    }
    
    //21-04-17 JC ++
    //make sure parameter file in cert format before loading
    if (memcmp(content, "\x30\x82", 2)) {
      FreeMW(content);
      return FALSE;
    }
    
    //prevent client cert buffer overflow
    if (filesize > K_MaxClientCertSize) {
      FreeMW(content);
      return FALSE;
    }
    //21-04-17 JC --
    
    /* getting data */
    aptr->d_cert_idx = 0xFF;             // extract cert index
    aptr->d_size = filesize;             // extract cert size
    for (i=0; i<filesize; i++)           // extract cert content
    {
      aptr->s_cert[i] = content[i];
    }
    
    FreeMW(content);
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//  Function        : GetClientPriKey
//  Description     : Get Client Private Key from file
//  Input           : aptr:  Private Key pointer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetClientPriKey(T_RSA_PRIVATE_KEY *aptr)
{
  T_Dir dirLink;
  T_DirEntry *entry;
  struct FILELIST list[10];
  BYTE dirname[] = "/";
  BYTE buf[32];
  
  T_KEY pubKey;
  int result;
  BYTE *content;
  BYTE filename[20];        // stored filename to be read locally
  int pfile_num = 0;
  int filesize = 0;
  int readlen = 0;
  int read_filehdr = -1;
  
  memset(filename, 0, sizeof(filename));
  
  if (os_file_init_search(dirname, &dirLink) < 0)        // read number of files
    return FALSE;

  do {
    entry = (T_DirEntry *)os_file_readdir(&dirLink);    // read files one by one
    if (entry != NULL) {
      if (!memcmp((char const *)entry->s_name, ClientPriKey, 6)) {  // read para files only
        memcpy(list[pfile_num].filename, entry->s_name, sizeof(list[pfile_num].filename));
        list[pfile_num].len = entry->d_filelen;
        pfile_num++;
        break;
      }
    }
  } while (entry != NULL);

  if (pfile_num)
  {
    pfile_num--;
    memcpy(filename, dirname, 1);
    strcat(filename, list[pfile_num].filename);
    if ((read_filehdr = os_file_open(filename, K_O_RDONLY)) < 0)
    {
      SprintfMW(buf, "%s open fail!", ClientPriKey);
      DispLineMW(buf, MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
      os_file_close(read_filehdr);
      WaitKey(KBD_TIMEOUT);
      return FALSE;
    }
    else
    {
      /* getting file content */
      filesize = os_file_length(read_filehdr);
      content = (BYTE*) MallocMW(filesize);
      if (content == NULL) {
        os_file_close(read_filehdr);
        return FALSE;
      }
      readlen = os_file_read(read_filehdr, content, filesize);
      os_file_close(read_filehdr);
    }
    
    if (readlen != filesize) {
      FreeMW(content);
      return FALSE;
    }
    
    //21-04-17 JC ++
    //make sure parameter file in cert format before loading
    if (memcmp(content, "\x30\x82", 2)) {
      FreeMW(content);
      return FALSE;
    }
    //21-04-17 JC --
    
    /* getting data */
    aptr->d_key_idx = 0xFF;              // extract cert index
    result = x509ParseKey(content, filesize, &pubKey, aptr);
    if (result != PKCS_OK)
    {
      //sprintf(buf, "PKCS Parsing Err: %d", result);
      //DispLineMW(buf, MW_LINE3, MW_CENTER|MW_CLREOL|MW_SPFONT);
      FreeMW(content);
      return FALSE;
    }
    
    FreeMW(content);
    return TRUE;
  }
  return FALSE;
}
