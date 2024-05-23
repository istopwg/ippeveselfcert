//
// IPP Everywhere Tool (ippevetool) printer window.
//
// Copyright © 2022-2024 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//
// ignore_for_file: file_names
//

import 'dart:core';
import 'package:flutter/cupertino.dart';
import "IppPrinter.dart";
import "IppAttributesPage.dart";
import "PrintFilePage.dart";
import "TxtRecordPage.dart";
import "ipptool.dart";
import "util.dart";


// IPP Printer page...
class IppPrinterPage extends StatefulWidget {
    const IppPrinterPage({super.key, required this.printer});

    final IppPrinter printer;

    @override
    State<IppPrinterPage> createState() => _IppPrinterPageState();
}


// State for the home page...
class _IppPrinterPageState extends State<IppPrinterPage> {
    @override
    Widget build(BuildContext context) {
        return CupertinoPageScaffold(
            navigationBar: CupertinoNavigationBar(
                middle: Text(widget.printer.dnssdName),
            ),
            child: SingleChildScrollView(
                child: CupertinoListSection(
                    header: const Text("Printer Information"),
                    children: [
                        CupertinoListTile(
                            subtitle: Text(widget.printer.dnssdName),
                            title: const Text("DNS-SD Name"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => tapValue(context, widget.printer.dnssdName),
                        ),
                        CupertinoListTile(
                            subtitle: Text(widget.printer.moreInfo),
                            title: const Text("Embedded Web Page"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => tapValue(context, widget.printer.moreInfo),
                        ),
                        CupertinoListTile(
                            subtitle: Text(widget.printer.geoLocation),
                            title: const Text("Geolocation"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => tapValue(context, widget.printer.geoLocation),
                        ),
                        CupertinoListTile(
                            subtitle: Text(widget.printer.location),
                            title: const Text("Location"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => tapValue(context, widget.printer.location),
                        ),
                        CupertinoListTile(
                            subtitle: Text(widget.printer.makeAndModel),
                            title: const Text("Make and Model"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => tapValue(context, widget.printer.makeAndModel),
                        ),
                        CupertinoListTile(
                            subtitle: Text("${widget.printer.state} ${widget.printer.stateReasons}"),
                            title: const Text("State"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => tapValue(context, "${widget.printer.state} ${widget.printer.stateReasons}"),
                        ),
                        CupertinoListTile(
                            subtitle: Text(widget.printer.uri),
                            title: const Text("URI"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => tapValue(context, widget.printer.uri),
                        ),
                        CupertinoListTile(
                            subtitle: Text(widget.printer.uuid),
                            title: const Text("UUID"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => tapValue(context, widget.printer.uuid),
                        ),
                        CupertinoListTile(
                            title: Text("View ${widget.printer.txt.length} TXT Key/Value Pairs"),
                            trailing: const CupertinoListTileChevron(),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => Navigator.of(context).push(CupertinoPageRoute<void>(
                                builder: (BuildContext context) {
                                    return TxtRecordPage(printer: widget.printer);
                                },
                            )),
                        ),
                        CupertinoListTile(
                            title: Text("View ${widget.printer.attributes.length} IPP Attributes"),
                            trailing: const CupertinoListTileChevron(),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => Navigator.of(context).push(CupertinoPageRoute<void>(
                                builder: (BuildContext context) {
                                    return IppAttributesPage(printer: widget.printer);
                                },
                            )),
                        ),
                        CupertinoListTile(
                            title: const Text("Identify Printer"),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => identifyPrinter(),
                        ),
                        CupertinoListTile(
                            title: const Text("Print a File"),
                            trailing: const CupertinoListTileChevron(),
                            leadingSize: 0,
                            leadingToTitle: 0,
                            onTap: () => Navigator.of(context).push(CupertinoPageRoute<void>(
                                builder: (BuildContext context) {
                                    return PrintFilePage(printer: widget.printer);
                                },
                            )),
                        ),
                        const CupertinoListTile(
                            title: Text("Run a Test"),
                            trailing: CupertinoListTileChevron(),
                            leadingSize: 0,
                            leadingToTitle: 0,
                        ),
                        const CupertinoListTile(
                            title: Text("Run Self-Certification Tests"),
                            trailing: CupertinoListTileChevron(),
                            leadingSize: 0,
                            leadingToTitle: 0,
                        ),
                    ],
                ),
            ),
        );
    }

    Future<void> identifyPrinter() async {
        var plist = await ipptoolRunTest(printerUri:widget.printer.uri, testfile:"identify-printer-default.test");

        // Do something with results...
        if (!plist["Successful"]) {
            var statusCode = plist["Tests"][0]["StatusCode"];

            setState(() {
                showAlert(context, "Identify-Printer request failed: $statusCode");
            });
        }
    }
}
