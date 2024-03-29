@echo off
::
:: IPP Everywhere Printer Self-Certification Manual 1.1: Section 5: DNS-SD Tests.
::
:: Copyright 2014-2022 by the IEEE-ISTO Printer Working Group.
::
:: Licensed under Apache License v2.0.  See the file "LICENSE" for more
:: information.
::
:: Usage:
::
::   dnssd-tests.bat 'Printer Name'
::

set name=%1
set name=%name:~1,-1%
set PLIST=%USERPROFILE%\Desktop\%name% DNS-SD Results.plist
echo Sending output to "%PLIST%"...

:: Write the standard XML plist header...
echo ^<?xml version=^"1.0^" encoding=^"UTF-8^"?^> >"%PLIST%"
echo ^<!DOCTYPE plist PUBLIC ^"-//Apple Computer//DTD PLIST 1.0//EN^" ^"http://www.apple.com/DTDs/PropertyList-1.0.dtd^"^> >>"%PLIST%"
echo ^<plist version=^"1.0^"^> >>"%PLIST%"
echo ^<dict^> >>"%PLIST%"
echo ^<key^>Tests^</key^>^<array^> >>"%PLIST%"

set total=0
set pass=0
set fail=0
set skip=0

:: B-1. IPP Browse test: Printers appear in a search for "_ipp._tcp,_print" services?
set /a total+=1
set <NUL /p="B-1. IPP Browse test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-1. IPP Browse test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ippeveselfcert11.dnssd^</string^> >>"%PLIST%"

set result=FAIL
ippfind _ipp._tcp,_print.local. -T 5 --literal-name "%name%" --quiet && set result=PASS
if "%result%" == "PASS" (
	set /a pass+=1
) else (
	set /a fail+=1
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"
) else (
	echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
)
echo ^</dict^> >>"%PLIST%"

:: B-2. IPP TXT keys test: The IPP TXT record contains all required keys.
set /a total+=1
set <NUL /p="B-2. IPP TXT keys test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-2. IPP TXT keys test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

set result=FAIL
ippfind "%name%._ipp._tcp.local." --txt adminurl --txt pdl --txt rp --txt UUID --quiet && set result=PASS
if "%result%" == "PASS" (
	set /a pass+=1
) else (
	set /a fail+=1
	echo ^<key^>Errors^</key^>^<array^>^<string^> >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo adminurl="{txt_adminurl}" ";" >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo pdl="{txt_pdl}" ";" >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo rp="{txt_rp}" ";" >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo UUID="{txt_uuid}" ";" >>"%PLIST%"
	echo ^</string^>^</array^> >>"%PLIST%"
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"

	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo adminurl="{txt_adminurl}" ";"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo pdl="{txt_pdl}" ";"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo rp="{txt_rp}" ";"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo UUID="{txt_uuid}" ";"
) else (
	echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
)
echo ^</dict^> >>"%PLIST%"

:: B-3. IPP Resolve test: Printer responds to an IPP Get-Printer-Attributes request using the resolved hostname, port, and resource path.
set /a total+=1
set <NUL /p="B-3. IPP Resolve test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-3. IPP Resolve test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

set result=FAIL
(ippfind "%name%._ipp._tcp.local." --ls && set result=PASS) >nul:
if "%result%" == "PASS" (
	set /a pass+=1
) else (
	set /a fail+=1
	echo ^<key^>Errors^</key^>^<array^>^<string^> >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." --ls >>"%PLIST%"
	echo ^</string^>^</array^> >>"%PLIST%"
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"
) else (
	echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
)
echo ^</dict^> >>"%PLIST%"

:: B-4. IPP TXT values test: The IPP TXT record values match the reported IPP attribute values.
set /a total+=1
set <NUL /p="B-4. IPP TXT values test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-4. IPP TXT values test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

set result=FAIL
ippfind "%name%._ipp._tcp.local." --txt-adminurl ^^^(http:^|https:^)// --txt-pdl image/pwg-raster --txt-rp ^^ipp/^(print^|print/[^^/]+^)$ --txt-UUID ^^[0-9a-fA-F]{8,8}-[0-9a-fA-F]{4,4}-[0-9a-fA-F]{4,4}-[0-9a-fA-F]{4,4}-[0-9a-fA-F]{12,12}$ -x ipptool -q "{}" dnssd-value-tests.test ";" && set result=PASS
if "%result%" == "PASS" (
	set /a pass+=1
) else (
	set /a fail+=1
	echo ^<key^>Errors^</key^>^<array^>^<string^> >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo adminurl="{txt_adminurl}" ";" >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo pdl="{txt_pdl}" ";" >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo rp="{txt_rp}" ";" >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo UUID="{txt_uuid}" ";" >>"%PLIST%"
	ippfind "%name%._ipp._tcp.local." -x ipptool -t "{}" dnssd-value-tests.test ";" | findstr /r [TG][EO][DT]: >>"%PLIST%"
	echo ^</string^>^</array^> >>"%PLIST%"
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"

	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo adminurl="{txt_adminurl}" ";"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo pdl="{txt_pdl}" ";"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo rp="{txt_rp}" ";"
	ippfind "%name%._ipp._tcp.local." -x cmd.exe /c echo UUID="{txt_uuid}" ";"
	ippfind "%name%._ipp._tcp.local." -x ipptool -t "{}" dnssd-value-tests.test ";" | findstr /r [TG][EO][DT]:
) else (
	echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
)
echo ^</dict^> >>"%PLIST%"

:: B-5. TLS tests: Performed only if TLS is supported
set /a total+=1
set <NUL /p="B-5. TLS tests: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-5. TLS tests^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

set result=SKIP
ippfind "%name%._ipp._tcp.local." --txt-TLS ^^1\.[0-9] --quiet && set result=PASS
if "%result%" == "PASS" (
	set /a pass+=1
	set HAVE_TLS=1
) else (
	set /a skip+=1
	set HAVE_TLS=0
)

echo %result%
if "%result%" == "SKIP" (
	echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
	echo ^<key^>Skipped^</key^>^<true /^> >>"%PLIST%"
) else (
	echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
)
echo ^</dict^> >>"%PLIST%"

:: B-5.1 HTTP Upgrade test: Printer responds to an IPP Get-Printer-Attributes request after doing an HTTP Upgrade to TLS.
set /a total+=1
set <NUL /p="B-5.1 HTTP Upgrade test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-5.1 HTTP Upgrade test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

if "%HAVE_TLS%" == "1" (
	set result=FAIL
	ippfind "%name%._ipp._tcp.local." -x ipptool -E -q "{}" dnssd-access-tests.test ";" && set result=PASS
	if "%result%" == "PASS" (
		set /a pass+=1
	) else (
		set /a fail+=1
		echo ^<key^>Errors^</key^>^<array^>^<string^> >>"%PLIST%"
		ippfind "%name%._ipp._tcp.local." -x ipptool -E -q "{}" dnssd-access-tests.test ";" >>"%PLIST%"
		echo ^</string^>^</array^> >>"%PLIST%"
	)
) else (
	set /a skip+=1
	set result=SKIP
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"
) else (
	if "%result%" == "SKIP" (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
		echo ^<key^>Skipped^</key^>^<true /^> >>"%PLIST%"
	) else (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
	)
)
echo ^</dict^> >>"%PLIST%"

:: B-5.2 IPPS Browse test: Printer appears in a search for "_ipps._tcp,_print" services.
set /a total+=1
set <NUL /p="B-5.2 IPPS Browse test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-5.2 IPPS Browse test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

if "%HAVE_TLS%" == "1" (
	set result=FAIL
	ippfind _ipps._tcp,_print.local. -T 5 --literal-name "%name%" --quiet && set result=PASS
	if "%result%" == "PASS" (
		set /a pass+=1
	) else (
		set /a fail+=1
	)
) else (
	set /a skip+=1
	set result=SKIP
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"
) else (
	if "%result%" == "SKIP" (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
		echo ^<key^>Skipped^</key^>^<true /^> >>"%PLIST%"
	) else (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
	)
)
echo ^</dict^> >>"%PLIST%"

:: B-5.3 IPPS TXT keys test: The TXT record for IPPS contains all required keys
set /a total+=1
set <NUL /p="B-5.3 IPPS TXT keys test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-5.3 IPPS TXT keys test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

if "%HAVE_TLS%" == "1" (
	set result=FAIL
	ippfind "%name%._ipps._tcp.local." --txt adminurl --txt pdl --txt rp --txt TLS --txt UUID --quiet && set result=PASS
	if "%result%" == "PASS" (
		set /a pass+=1
	) else (
		set /a fail+=1
	        echo ^<key^>Errors^</key^>^<array^>^<string^> >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo adminurl="{txt_adminurl}" ";" >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo pdl="{txt_pdl}" ";" >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo rp="{txt_rp}" ";" >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo TLS="{txt_tls}" ";" >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo UUID="{txt_uuid}" ";" >>"%PLIST%"
	        echo ^</string^>^</array^> >>"%PLIST%"
	)
) else (
	set /a skip+=1
	set result=SKIP
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"

        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo adminurl="{txt_adminurl}" ";"
        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo pdl="{txt_pdl}" ";"
        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo rp="{txt_rp}" ";"
        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo TLS="{txt_tls}" ";"
        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo UUID="{txt_uuid}" ";"
) else (
	if "%result%" == "SKIP" (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
		echo ^<key^>Skipped^</key^>^<true /^> >>"%PLIST%"
	) else (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
	)
)
echo ^</dict^> >>"%PLIST%"

:: B-5.4 IPPS Resolve test: Printer responds to an IPPS Get-Printer-Attributes request using the resolved hostname, port, and resource path.
set /a total+=1
set <NUL /p="B-5.4 IPPS Resolve test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-5.4 IPPS Resolve test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

if "%HAVE_TLS%" == "1" (
	set result=FAIL
	(ippfind "%name%._ipps._tcp.local." --ls && set result=PASS) >nul:
	if "%result%" == "PASS" (
		set /a pass+=1
	) else (
		set /a fail+=1
		echo ^<key^>Errors^</key^>^<array^>^<string^> >>"%PLIST%"
		ippfind "%name%._ipps._tcp.local." --ls >>"%PLIST%"
		echo ^</string^>^</array^> >>"%PLIST%"
	)
) else (
	set /a skip+=1
	set result=SKIP
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"
) else (
	if "%result%" == "SKIP" (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
		echo ^<key^>Skipped^</key^>^<true /^> >>"%PLIST%"
	) else (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
	)
)
echo ^</dict^> >>"%PLIST%"

:: B-5.5 IPPS TXT values test: The TXT record values for IPPS match the reported IPPS attribute values.
set /a total+=1
set <NUL /p="B-5.5 IPPS TXT values test: "
echo ^<dict^>^<key^>Name^</key^>^<string^>B-5.5 IPPS TXT values test^</string^> >>"%PLIST%"
echo ^<key^>FileId^</key^>^<string^>org.pwg.ipp-everywhere.dnssd^</string^> >>"%PLIST%"

if "%HAVE_TLS%" == "1" (
	set result=FAIL
	ippfind "%name%._ipps._tcp.local." --txt-adminurl ^^^(http:^|https:^)// --txt-pdl image/pwg-raster --txt-rp ^^ipp/^(print^|print/[^^/]+^)$ --txt-UUID ^^[0-9a-fA-F]{8,8}-[0-9a-fA-F]{4,4}-[0-9a-fA-F]{4,4}-[0-9a-fA-F]{4,4}-[0-9a-fA-F]{12,12}$ -x ipptool -q "{}" dnssd-value-tests.test ";" && set result=PASS
	if "%result%" == "PASS" (
		set /a pass+=1
	) else (
		set /a fail+=1
	        echo ^<key^>Errors^</key^>^<array^>^<string^> >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo adminurl="{txt_adminurl}" ";" >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo pdl="{txt_pdl}" ";" >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo rp="{txt_rp}" ";" >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo TLS="{txt_tls}" ";" >>"%PLIST%"
	        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo UUID="{txt_uuid}" ";" >>"%PLIST%"
		ippfind "%name%._ipps._tcp.local." -x ipptool -t "{}" dnssd-value-tests.test ";" | findstr /r [TG][EO][DT]: >>"%PLIST%"
	        echo ^</string^>^</array^> >>"%PLIST%"
	)
) else (
	set /a skip+=1
	set result=SKIP
)

echo %result%
if "%result%" == "FAIL" (
	echo ^<key^>Successful^</key^>^<false /^> >>"%PLIST%"

        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo adminurl="{txt_adminurl}" ";"
        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo pdl="{txt_pdl}" ";"
        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo rp="{txt_rp}" ";"
        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo TLS="{txt_tls}" ";"
        ippfind "%name%._ipps._tcp.local." -x cmd.exe /c echo UUID="{txt_uuid}" ";"
	ippfind "%name%._ipp._tcp.local." -x ipptool -t "{}" dnssd-value-tests.test ";" | findstr /r [TG][EO][DT]:
) else (
	if "%result%" == "SKIP" (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
		echo ^<key^>Skipped^</key^>^<true /^> >>"%PLIST%"
	) else (
		echo ^<key^>Successful^</key^>^<true /^> >>"%PLIST%"
	)
)
echo ^</dict^> >>"%PLIST%"

:: Finish up...
echo ^</array^> >>"%PLIST%"
echo ^<key^>Successful^</key^> >>"%PLIST%"
if %fail% gtr 0 (
	echo ^<false /^> >>"%PLIST%"
) else (
	echo ^<true /^> >>"%PLIST%"
)
echo ^</dict^> >>"%PLIST%"
echo ^</plist^> >>"%PLIST%"

set /a score=%pass% + %skip%
set /a score=100 * %score% / %total%
echo Summary: %total% tests, %pass% passed, %fail% failed, %skip% skipped
echo Score: %score%%%
