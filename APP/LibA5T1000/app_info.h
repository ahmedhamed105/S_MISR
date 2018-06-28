                /****** file type define ******/
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

#define K_Version               1
#define K_SubVersion            0
#define K_AppID                 0
#define K_Type                  K_SysSign
