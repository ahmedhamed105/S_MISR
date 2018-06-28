@echo off                                            
@echo <AGENTID> > UNILOAD.INF                          
REM Normal COM
REM @%2\conloader %1 -s115200 -<COMPORT> -k<AGENTKEY>
REM USB COM
@%2\conloader %1 -s115200 -X -<COMPORT> -k<AGENTKEY>
@del UNILOAD.INF                                     
@echo on                                                
