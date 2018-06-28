@echo off
@REM echo Usage: CdEnv MODEL_NAME
@REM *** Set Parameters ***
@set MODEL_NAME=A5T1000
@IF NOT "%1" == "" (
  @set MODEL_NAME=%1
  CALL :UpCase MODEL_NAME
)
@REM *** Environment ***
@set ORG_PATH=%PATH%
@set SET_MODEL=%CD%\SetModel.bat
@set GCC_COMPILER_DIR=C:\DevTools\gcc_v49_2014q4
@set GCC_TOOLS_DIR=../../Tools
@set GCC_PATH=%GCC_TOOLS_DIR%;C:\DevTools\GnuWin32\bin;%GCC_COMPILER_DIR%\bin;C:\Windows\system32
@call %SET_MODEL% %MODEL_NAME%
@cmd
@set PATH=%ORG_PATH%
@set ORG_PATH=
GOTO:EOF

:UpCase
:: Subroutine to convert a variable VALUE to all UPPER CASE.
:: The argument for this subroutine is the variable NAME.
FOR %%i IN ("a=A" "b=B" "c=C" "d=D" "e=E" "f=F" "g=G" "h=H" "i=I" "j=J" "k=K" "l=L" "m=M" "n=N" "o=O" "p=P" "q=Q" "r=R" "s=S" "t=T" "u=U" "v=V" "w=W" "x=X" "y=Y" "z=Z") DO CALL SET "%1=%%%1:%%~i%%"
GOTO:EOF
