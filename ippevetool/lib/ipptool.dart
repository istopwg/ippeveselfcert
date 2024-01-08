//
// ipptool support function.
//
// Copyright © 2024 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

import 'dart:async';
import 'dart:io';
import 'dart:convert';
import 'package:xml/xml.dart';


Future<XmlDocument> ipptool({required String testfile, String? datafile, Map<String, String>? vars}) async {
    List<String> args = [testfile];     // Command-line arguments

    if (datafile != null) {
        args.add("-f");
        args.add(datafile);
    }

    if (vars != null) {
        vars.forEach((key, value) {
            args.add("-D$key=$value");
        });
    }

    var process = await Process.start("ipptool", args);
    var plist = "";
    process.stderr.pipe(stderr);
    await process.stdout
        .transform(utf8.decoder)
        .forEach((text) {
            plist = plist + text;
        });
    var result = process.exitCode;

    print("result=$result\n");
    print("plist=$plist\n");
  
    return (XmlDocument.parse(plist));
}
