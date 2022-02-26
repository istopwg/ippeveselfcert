@echo off
::
:: IPP Everywhere Printer Self-Certification Manual 1.1: Section 6: IPP Tests.
::
:: Copyright 2014-2020 by the IEEE-ISTO Printer Working Group.
::
:: Licensed under Apache License v2.0.  See the file "LICENSE" for more
:: information.
::
:: Usage:
::
::   ipp-tests.bat 'Printer Name'
::

set name=%1
set name=%name:~1,-1%
ippfind "%name%._ipp._tcp.local." -x ipptool -P "%USERPROFILE%\Desktop\%name% IPP Results.plist" -I {} ipp-tests.test ";"
