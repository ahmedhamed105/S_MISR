@echo off
@echo === Set Path ===
@echo set GnuWin32 and Yagarto paths
@set PATHORG=%PATH%
@set path=C:\DevTools\GnuWin32\bin;C:\DevTools\gcc_v49_2014q4\bin;C:\Windows\system32;
@echo -----------------------------------
@echo === Clean ===
@make clean
@echo -----------------------------------
@echo === Build ===
@make all
@echo -----------------------------------
@echo === Finish [Check Any Errors or Warnings] ===
@set PATH=%PATHORG%
@set PATHORG=
pause
