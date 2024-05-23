//
// IPP Everywhere Tool (ippevetool) support functions.
//
// Copyright © 2024 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

import 'dart:async';
import 'dart:io';
import 'dart:convert';
import 'dart:developer' as developer;
import 'package:propertylistserialization/propertylistserialization.dart';


// Get the printer attributes as Map<String,dynamic> value
Future<Map<String,dynamic>> ipptoolGetAttributes({required String printerUri}) async {
    var process = await Process.start("ipptool", ["-j", printerUri, "get-printer-attributes.test"]);
    var json = "";
    var error = "";
    await process.stdout
        .transform(utf8.decoder)
        .forEach((text) {
            json = json + text;
        });
    await process.stderr
        .transform(utf8.decoder)
        .forEach((text) {
            error = error + text.replaceAll("\n", " ");
        });
    //var result = process.exitCode;

    //print("result=$result\n");
    //print("json=$json\n");
    //print("error=$error\n");
  
    if (json == "") {
      json = "[{},{\"ipptool-error\":\"$error\"}]";
    }

    const JsonDecoder decoder = JsonDecoder();
    var groups = decoder.convert(json);
    if (groups.length > 1) {
        return (groups[1]);
    } else {
        return (groups[0]);
    }
}


// Run an ipptool test file and return the results as an XML plist file
Future<Map<String,dynamic>> ipptoolRunTest({required String printerUri, required String testfile, String? datafile, Map<String, String>? vars}) async {
    List<String> args = [printerUri, "-X"];   // Command-line arguments

    if (datafile != null) {
        args.add("-f");
        args.add(datafile);
    }

    if (vars != null) {
        vars.forEach((key, value) {
            args.add("-D$key=$value");
        });
    }

    args.add(testfile);

    developer.log("Starting ipptool $args", name:"ipptool");

    var process = await Process.start("ipptool", args);
    var plist = "";
    process.stderr.pipe(stderr);
    await process.stdout
        .transform(utf8.decoder)
        .forEach((text) {
            plist = plist + text;
        });
//    var result = process.exitCode;

//    print("result=$result\n");
//    print("plist=$plist\n");

    try {
        return (PropertyListSerialization.propertyListWithString(plist) as Map<String,Object>);
    } on PropertyListReadStreamException catch (e) {
        return ({"Successful":false, "Tests":[{"statusCode":"$e"}]});
    }
}
