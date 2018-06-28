@echo off                                            
@echo SptWWEgy > UNILOAD.INF                          
REM Normal COM
REM conloader %1 -s115200 -X -3 -kF25B15AAF4B48EAF5A9B8E7BC1B81A0A
REM USB COM
conloader %1 -s115200 -X -1 -kF25B15AAF4B48EAF5A9B8E7BC1B81A0A
@del UNILOAD.INF                                     
@echo on                                                
