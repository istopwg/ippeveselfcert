@echo off
::
:: IPP Everywhere Printer Self-Certification Manual 1.0: Section 6: IPP Tests.
::
:: Copyright 2014-2016 by The Printer Working Group.
::
:: This program may be copied and furnished to others, and derivative works
:: that comment on, or otherwise explain it or assist in its implementation may
:: be prepared, copied, published and distributed, in whole or in part, without
:: restriction of any kind, provided that the above copyright notice and this
:: paragraph are included on all such copies and derivative works.
::
:: The IEEE-ISTO and the Printer Working Group DISCLAIM ANY AND ALL WARRANTIES,
:: WHETHER EXPRESS OR IMPLIED INCLUDING (WITHOUT LIMITATION) ANY IMPLIED
:: WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
::
:: Usage:
::
::   ipp-tests.bat 'Printer Name'
::

set name=%1
set name=%name:~1,-1%
ippfind "%name%._ipp._tcp.local." -x ipptool -P "\"%USERPROFILE%\\Desktop\\%name% IPP Results.plist\"" -I "{}" ipp-tests.test ";"
