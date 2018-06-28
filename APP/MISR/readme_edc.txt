EDC Overview
============
[coremain.c]
  Coremain()                    - starting point of the whole app, with a while loop to receive card input
  SetupModeProcess()            - when STIS parameters are downloaded, this function will be executed
  TransModeProcess()            - when STIS parameters are downloaded, this function will be executed
                                - this function will wait for ICC insert, magnetic card and manual key entry
  ICCInserted() -> EMVTrans()                - when ICC is inserted, EMVTrans() will be called
  MSRSwiped() -> ValidCard() -> SaleTrans()  - when MSR swiped, check if card range within STIS settings
                                             - SaleTrans() will be called

[emvtrans.c]
  EMVTrans() -> EMVProcess()    - start of a chip transaction. EMV_DllMsg() will be called in loop inside EMVProcess()
                                - InCardTable() will be called to check if PAN falls within the card range set by STIS
                                - PackComm(), APM_ConnectOK() & APM_SendRcvd() will be used for an online txn
                                - wVMOnlineTags (list of tags) is used to pack TLV in Field 55 of ISO message
                                - PackHostMsg() & CheckHostRsp() to format & decode ISO message
                                - PackInputP() will be called to prepare the slip content
                                - SaveRecord() will be called to save txn record to flash (stored by APM)
                                - TransEnd() will call PrintBuf() to print out the slip

[sale.c] & [tranutil.c]
  SaleTrans() -> TransAllowed() - check STIS settings and draw the cardholder confirmation screen
                                - GetAmount() will be called to get amount for this txn
                                - PackComm(), APM_ConnectOK() & APM_SendRcvd() will be used for an online txn
                                - PackHostMsg() & CheckHostRsp() to format & decode ISO message
                                - PackInputP() will be called to prepare the slip content
                                - SaveRecord() will be called to save txn record to flash (stored by APM)
                                - TransEnd() will call PrintBuf() to print out the slip

[input.c]
  InCardTable()                 - APM_GetCardTbl(), APM_GetIssuer() & APM_GetAcqTbl() will be called 
                                - in sequence to search for the matched Acquirer set in STIS

[keytrans.c]
  IccTmkInject()                - Inject TMK through special ICC
  LoadKeys()                    - Load encrypted keys into Keydll
  GetPIN()                      - Get encrypted PIN block by specifying the key index
  ResetKey()                    - Reset keys

[message.c]
  KMessage                      - All messages definition
  KRspText                      - Response Code from host & message to display accordingly

[constant.c]
  KTransBitmap                  - Definition of Msg ID, Bitmap, etc for different transaction types
  KTransHeader                  - Definition of the title bar message for different transaction types 

[chkoptn.c]                     - Functions to check STIS parameters (Acquirer/Issuer/CardRange tables)
