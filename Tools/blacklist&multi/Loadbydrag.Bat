@echo off                                            
@echo NatTechG > UNILOAD.INF                          
REM Normal COM
REM conloader %1 -s115200 -X -3 -k78711F48352D7CCE3C6D4889FAFFB2D2
REM USB COM
..\conloader %1 -s115200 -X -1 -k78711F48352D7CCE3C6D4889FAFFB2D2
@del UNILOAD.INF                                     
@echo on                                                
