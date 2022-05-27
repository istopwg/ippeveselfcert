@echo off
::
:: IPP Everywhere Printer Self-Certification Manual 1.1: Section 7: Document Data Tests.
::
:: Copyright 2014-2022 by the IEEE-ISTO Printer Working Group.
::
:: Licensed under Apache License v2.0.  See the file "LICENSE" for more
:: information.
::
:: Usage:
::
::   document-tests.bat 'Printer Name'
::

set name=%1
set name=%name:~1,-1%
ippfind "%name%._ipp._tcp.local." -x ipptool -P "%USERPROFILE%\Desktop\%name% Document Results.plist" -I -T 120 {} document-tests.test ";"
