//
// IPP Everywhere Tool (ippevetool) IppPrinter class.
//
// Copyright © 2024 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//
// ignore_for_file: file_names
//

import 'dart:core';
import 'dart:collection';
import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';
import 'ipptool.dart';
import 'package:nsd/nsd.dart';
import 'package:flutter/cupertino.dart';


class IppPrinter {
    final String uri;

    final SplayTreeMap<String,dynamic> attributes;
    final String dnssdName;
    final List<String> documentFormatSupported;
    final String geoLocation;
    final String icon;
    final bool isAcceptingJobs;
    final String location;
    final String makeAndModel;
    final String moreInfo;
    final String name;
    final String state;
    final List<String> stateReasons;
    final SplayTreeMap<String,String> txt;
    final String uuid;

    const IppPrinter(this.uri, this.attributes, this.dnssdName, this.documentFormatSupported, this.geoLocation, this.icon, this.isAcceptingJobs, this.location, this.makeAndModel, this.moreInfo, this.name, this.state, this.stateReasons, this.txt, this.uuid);

    Image getImage() {
        if (icon != "") {
            return Image.network(icon);
        } else {
            return Image.asset("images/missing-icon.png");
        }
    }
}


Future<IppPrinter> ippPrinterWithService(Service service) async {
    SplayTreeMap<String,dynamic> attributes = SplayTreeMap<String,dynamic>((a, b) => a.compareTo(b));
    String dnssdName = service.name ?? "";
    List<String> documentFormatSupported = [];
    String geoLocation = "";
    String icon = "";
    bool isAcceptingJobs = false;
    String location = "";
    String moreInfo = "";
    String makeAndModel = "Unknown";
    String name = "Unknown";
    String state = "Unknown";
    List<String> stateReasons = [];
    SplayTreeMap<String,String> txt = SplayTreeMap<String,String>((a, b) => a.compareTo(b));
    String uuid = "";

    // Printer URI
    if (service.host == null || service.port == null || service.txt == null) {
        throw const SocketException("Bad DNS-SD service.");
    }

    // Have a hostname, port, and TXT record...
    var rp = "/ipp/print";

    if (service.txt!["rp"] != null) {
        // Use the "rp" value from the TXT record...
        Uint8List rpraw = service.txt!["rp"]!;
        rp = const Utf8Decoder().convert(rpraw);
        if (rp[0] != '/') {
            // Prefix with a leading slash...
            rp = "/$rp";
        }
    }

    var uri = "ipps://${service.host}:${service.port}$rp";

    // Convert TXT records...
    service.txt!.forEach((key,value){
        if (value != null) {
            txt[key] = const Utf8Decoder().convert(value);
        } else {
            txt[key] = "";
        }
    });

    // Get the attributes as a JSON object
    var attrs = await ipptoolGetAttributes(printerUri: uri);

    attributes = SplayTreeMap<String, dynamic>.from(attrs, (k1, k2) => k1.compareTo(k2));

    attrs.forEach((key,value){
        //print("'$key' = '$value'\n");

        if (key == "document-format-supported") {
            if (value is String) {
                documentFormatSupported.add(value);
            } else if (value is List) {
                for (var v in value) {
                    if (v is String) {
                        documentFormatSupported.add(v);
                    }
                }
            }
        } else if (key == "printer-dns-sd-name") {
            dnssdName = value;
        } else if (key == "printer-geo-location" && value is String) {
            geoLocation = value;
        } else if (key == "printer-icons") {
            icon = value.last;
        } else if (key == "printer-is-accepting-jobs") {
            isAcceptingJobs = value;
        } else if (key == "printer-location") {
            location = value;
        } else if (key == "printer-make-and-model") {
            makeAndModel = value;
        } else if (key == "printer-more-info") {
            moreInfo = value;
        } else if (key == "printer-name") {
            name = value;
        } else if (key == "printer-state") {
            if (value == 3) {
                state = "Idle";
            } else if (value == 4) {
                state = "Processing";
            } else {
                state = "Stopped";
            }
        } else if (key == "printer-state-reasons") {
            if (value is String && value != "none") {
                stateReasons.add(value);
            } else if (value is List) {
                for (var v in value) {
                    if (v is String && v != "none") {
                        stateReasons.add(v);
                    }
                }
            }
        } else if (key == "printer-uuid") {
            uuid = value;
        }
    });

    if (dnssdName == "") {
        dnssdName = name;
    }

    return IppPrinter(uri, attributes, dnssdName, documentFormatSupported, geoLocation, icon, isAcceptingJobs, location, makeAndModel, moreInfo, name, state, stateReasons, txt, uuid);
}


Future<IppPrinter> ippPrinterWithURI(String uri) async {
    SplayTreeMap<String,dynamic> attributes = SplayTreeMap<String,dynamic>((a, b) => a.compareTo(b));
    String dnssdName = "";
    List<String> documentFormatSupported = [];
    String geoLocation = "";
    String icon = "";
    bool isAcceptingJobs = false;
    String location = "";
    String moreInfo = "";
    String makeAndModel = "Unknown";
    String name = "Unknown";
    String state = "Unknown";
    List<String> stateReasons = [];
    SplayTreeMap<String,String> txt = SplayTreeMap<String,String>((a, b) => a.compareTo(b));
    String uuid = "";

    // Add standard TXT records...
    txt["priority"] = "50";
    txt["qtotal"]   = "1";
    txt["txtvers"]  = "1";

    // Extract information from URI
    Uri parsedUri = Uri.parse(uri);
    txt["rp"] = parsedUri.path;
    if (parsedUri.isScheme("ipps")) {
        txt["TLS"] = "1.3";
    }

    // Get the attributes as a JSON object
    var attrs = await ipptoolGetAttributes(printerUri: uri);

    attributes = SplayTreeMap<String, dynamic>.from(attrs, (k1, k2) => k1.compareTo(k2));

    attrs.forEach((key,value){
        //print("'$key' = '$value'\n");

        if (key == "color-supported") {
            if (value == true) {
                txt["Color"] = "Y";
            } else {
                txt["Color"] = "N";
            }
        } else if (key == "document-format-supported") {
            if (value is String) {
                documentFormatSupported.add(value);
            } else if (value is List) {
                for (var v in value) {
                    if (v is String) {
                        documentFormatSupported.add(v);
                    }
                }
            }
            txt["pdl"] = "$value";
        } else if (key == "printer-dns-sd-name") {
            dnssdName = value;
        } else if (key == "printer-geo-location" && value is String) {
            geoLocation = value;
        } else if (key == "printer-icons") {
            icon = value.last;
        } else if (key == "printer-is-accepting-jobs") {
            isAcceptingJobs = value;
        } else if (key == "printer-location") {
            location = value;
            txt["note"] = value;
        } else if (key == "printer-make-and-model") {
            makeAndModel = value;
            txt["product"] = "($value)";
            txt["ty"] = value;
        } else if (key == "printer-more-info") {
            moreInfo = value;
            txt["adminurl"] = value;
        } else if (key == "printer-name") {
            name = value;
        } else if (key == "printer-state") {
            if (value == 3) {
                state = "Idle";
            } else if (value == 4) {
                state = "Processing";
            } else {
                state = "Stopped";
            }
        } else if (key == "printer-state-reasons") {
            if (value is String && value != "none") {
                stateReasons.add(value);
            } else if (value is List) {
                for (var v in value) {
                    if (v is String && v != "none") {
                        stateReasons.add(v);
                    }
                }
            }
        } else if (key == "printer-uuid") {
            uuid = value;
            txt["UUID"] = value.substring(9);
        } else if (key == "sides-supported") {
            if (value == "one-sided") {
                txt["Duplex"] = "N";
            } else {
                txt["Duplex"] = "Y";
            }
        } else if (key == "urf-supported") {
            txt["URF"] = "$value";
        }
    });

    if (dnssdName == "") {
        dnssdName = name;
    }

    return IppPrinter(uri, attributes, dnssdName, documentFormatSupported, geoLocation, icon, isAcceptingJobs, location, makeAndModel, moreInfo, name, state, stateReasons, txt, uuid);
}
