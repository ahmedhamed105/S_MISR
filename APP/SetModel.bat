@echo off
@REM *** Note ***
@REM *** SetModel should be called by dos prompt that created by CdEnv.bat
rem SETLOCAL EnableDelayedExpansion
@echo Usage: SetModel MODEL_NAME
@REM *** Set Parameters ***
@IF NOT "%1" == "" (
  @set MODEL_NAME=%1
  CALL :UpCase MODEL_NAME
) ELSE (
  goto END
)

@REM *** Environments ***
@IF %MODEL_NAME% == CREON (
  goto CREON
) ELSE IF %MODEL_NAME% == SP530 (
  goto SP530
) ELSE IF %MODEL_NAME% == SP530GCC ( 
  @set MODEL_NAME=SP530
  goto GCC
) ELSE IF %MODEL_NAME% == T1000GCC ( 
  goto GCC
) ELSE IF %MODEL_NAME% == A5T1000 ( 
  goto GCC
) ELSE (
  goto YAGARTO
)

:CREON
  @set COMPILER_DIR=%CREON_COMPILER_DIR%
  @set TOOLS_DIR=%CREON_TOOLS_DIR%
  @set PATH=%CREON_PATH%
  @set CC=
  goto END

:SP530
  @set COMPILER_DIR=%SP530_COMPILER_DIR%
  @set TOOLS_DIR=%SP530_TOOLS_DIR%
  @set PATH=%SP530_PATH%
  @set CC=iar
  goto END

:YAGARTO
  @set COMPILER_DIR=%ARM9_COMPILER_DIR%
  @set TOOLS_DIR=%ARM9_TOOLS_DIR%
  @set PATH=%ARM9_PATH%
  @set CC=
  goto END

:YAGARTO2012
  @set COMPILER_DIR=%ARMX_COMPILER_DIR%
  @set TOOLS_DIR=%ARMX_TOOLS_DIR%
  @set PATH=%ARMX_PATH%
  @set CC=gcc
  goto END

:GCC
  @set COMPILER_DIR=%GCC_COMPILER_DIR%
  @set TOOLS_DIR=%GCC_TOOLS_DIR%
  @set PATH=%GCC_PATH%
  @set CC=gcc
  goto END

:END
@echo ---------------------------------------
@echo COMPILER_DIR: %COMPILER_DIR%
@echo TOOLS_DIR: %TOOLS_DIR%
@echo PATH: %PATH%
@echo ---------------------------------------
@echo  [make clean], [make all], [make load]
@echo  MODEL_NAME: [%MODEL_NAME%]            
@echo  CC: [%CC%]            
@echo ---------------------------------------
rem ENDLOCAL
GOTO:EOF

:UpCase
:: Subroutine to convert a variable VALUE to all UPPER CASE.
:: The argument for this subroutine is the variable NAME.
FOR %%i IN ("a=A" "b=B" "c=C" "d=D" "e=E" "f=F" "g=G" "h=H" "i=I" "j=J" "k=K" "l=L" "m=M" "n=N" "o=O" "p=P" "q=Q" "r=R" "s=S" "t=T" "u=U" "v=V" "w=W" "x=X" "y=Y" "z=Z") DO CALL SET "%1=%%%1:%%~i%%"
GOTO:EOF
