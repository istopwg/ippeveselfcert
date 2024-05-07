//
// IPP Everywhere Tool (ippevetool) main UI.
//
// Copyright © 2022-2024 by the ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

import 'dart:async';
import 'dart:collection';
import 'dart:convert';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:nsd/nsd.dart';
import 'package:url_launcher/url_launcher.dart';
//import 'package:xml/xml.dart';
import 'ipptool.dart';


// Main entry - run the main application window...
void main() {
    runApp(const IppEveToolApp());
}


void _showAlert(BuildContext context, String message) {
    // set up the button
    Widget okButton = TextButton(
        child: const Text("OK"),
        onPressed: () { Navigator.of(context).pop(); },
    );

    // set up the AlertDialog
    AlertDialog alert = AlertDialog(
        title: const Text("Alert"),
        content: Text(message),
        actions: [
            okButton,
        ],
    );

    // show the dialog
    showDialog(
        context: context,
        builder: (BuildContext context) {
            return alert;
        },
    );
}


// Tap handler - copy to the clipboard and open URLs...
void _tapValue(BuildContext context, String value) {
    if (value.startsWith("http://") || value.startsWith("https://")) {
        // Open URL...
        launchUrl(Uri.parse(value));
    }

    // Copy to clipboard and let the user know what happened...
    Clipboard.setData(ClipboardData(text: value));
    ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content:
            Text('Copied "$value" to clipboard.'),
            duration: const Duration(seconds: 1),
        )
    );
}


// Main application window...
class IppEveToolApp extends StatelessWidget {
    const IppEveToolApp({super.key});

    @override
    Widget build(BuildContext context) {
        return MaterialApp(
            title: 'IPP Everywhere™ Tool',
            theme: ThemeData(
                // Theme colors are based on the PWG logo colors, which are also used on the PWG web page...
                primarySwatch: const MaterialColor(0xff4b5aa8, {
                    50:  Color(0xff181814),
                    100: Color(0xff262d54),
                    200: Color(0xff2B397F),
                    300: Color(0xff394ba8),
                    400: Color(0xff445299),
                    500: Color(0xff4b5aa8),
                    600: Color(0xff626CA8),
                    700: Color(0xff94a4ff),
                    800: Color(0xffb8c3ff),
                    900: Color(0xffccccc0),
                }),
            ),
            debugShowCheckedModeBanner: false,
            home: const IppEveHomePage(title: 'IPP Everywhere™ Tool'),
        );
    }
}

// Home page with list of printers...
class IppEveHomePage extends StatefulWidget {
    const IppEveHomePage({super.key, required this.title});

    final String title;

    @override
    State<IppEveHomePage> createState() => _IppEveHomePageState();
}

class _IppEveHomePageState extends State<IppEveHomePage> {
    final printers = <Service>[];

    Future<void> _startDiscovery() async {
        final discovery = await startDiscovery("_ipp._tcp.", autoResolve: true);
        discovery.addServiceListener((service, status) {
            // print("${service.name} => ${status}");
            if (status == ServiceStatus.found) {
                setState((){
                    printers.add(service);
                    printers.sort((a, b) => a.name!.compareTo(b.name!));
                });
            }
        });
        return;
    }

    @override
    void initState() {
        super.initState();

        _startDiscovery();
    }

    @override
    Widget build(BuildContext context) {
        return Scaffold(
            appBar: AppBar(
                // Here we take the value from the IppEveHomePage object that was created by
                // the App.build method, and use it to set our appbar title.
                title: Text(widget.title),
            ),
            body: _buildList(context),
        );
    }

    _buildList(BuildContext context) {
        if (printers.isEmpty) {
            return const Center(
                child: CircularProgressIndicator(),
            );
        } else {
            return ListView.builder(
                itemBuilder: (context_, index) => GestureDetector(
                    onTap: () => _onPrinterTap(context, printers[index]),
                    child: ListTile(
                        leading: const Icon(Icons.print),
                        title: Text(printers[index].name ?? "Invalid printer name"),
                    ),
                ),
                itemCount: printers.length,
            );
        }
    }

    _onPrinterTap(BuildContext context, Service printer) {
        Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => IppEveDetailsPage(printer: printer,)),
        );
    }
}

// Details page for printers...
class IppEveDetailsPage extends StatefulWidget {
    const IppEveDetailsPage({super.key, required this.printer});

    final Service printer;

    @override
    State<IppEveDetailsPage> createState() => _IppEveDetailsPageState();
}

class _IppEveDetailsPageState extends State<IppEveDetailsPage> {
    Map<String,dynamic> printerAttrs = {};
    var printerIcon = Image.asset("images/default-icon.png", width:160);
    var printerState = "Unknown";
    var printerUri = "unknown";
    TextEditingController txtController = TextEditingController();
    String txtSearch = "";
    TextEditingController ippController = TextEditingController();
    String ippSearch = "";

    @override
    void initState() {
        super.initState();

        _getAttrList(widget.printer);
    }

    @override
    Widget build(BuildContext context) {
        return Scaffold(
            appBar: AppBar(
                title: Text(widget.printer.name ?? "Unknown"),
                actions: <Widget>[
                    TextButton(
                        onPressed: () {
                            _showAlert(context, "Print File UI");
                        },
                        child: const Text('Print File'),
                    ),
                    TextButton(
                        onPressed: () {
                            _showAlert(context, "Run Test UI");
                        },
                        child: const Text('Run Test'),
                    ),
                    TextButton(
                        onPressed: () {
                            _showAlert(context, "Run Self-Cert UI");
                        },
                        child: const Text('Run Self-Cert'),
                    ),
                ],
            ),
            body: SingleChildScrollView(child: Column(children: [
                Row(children: [
                    const Spacer(),
                    printerIcon,
                    const Spacer(),
                ]),
                Row(children: [
                    const Spacer(),
                    const Text("State: ", style: TextStyle(fontWeight: FontWeight.bold)),
                    Text(printerState),
                    const Spacer(),
                ]),
                Row(children: [
                    const Spacer(),
                    const Text("URI: ", style: TextStyle(fontWeight: FontWeight.bold)),
                    GestureDetector(
                        child: Text(printerUri, style: const TextStyle(color: Color(0xff4b5aa8))),
                        onTap: (){ _tapValue(context, printerUri); },
                    ),
                    const Spacer(),
                ]),

                // Divider between sections...
                const Divider(color: Color(0xff4b5aa8), height: 20.0, thickness: 2.0),

                Row(children: [
                    const Spacer(),
                    const Text("TXT Record: ", style: TextStyle(fontWeight: FontWeight.bold)),
                    SizedBox(
                        width: 400,
                        child: CupertinoSearchTextField(
                            controller: txtController,
                            onChanged: (String value) async {
                                setState(() {
                                    txtSearch = value;
                                });
                            },
                        ),
                    ),
                    const Spacer(),
                ]),
                DataTable(
                    columns: const [
                        DataColumn(label: SizedBox(width: 100,child: Text("Key"),)),
                        DataColumn(label: Expanded(child: Text("Value"))),
                    ],
                    rows: _buildTxtRows(widget.printer.txt, txtSearch),
                ),

                // Divider between sections...
                const Divider(color: Color(0xff4b5aa8), height: 20.0, thickness: 2.0),

                // IPP Attributes
                Row(children: [
                    const Spacer(),
                    const Text("IPP Attributes: ", style: TextStyle(fontWeight: FontWeight.bold)),
                    SizedBox(
                        width: 400,
                        child: CupertinoSearchTextField(
                            controller: ippController,
                            onChanged: (String value) async {
                                setState(() {
                                    ippSearch = value;
                                });
                            },
                        ),
                    ),
                    const Spacer(),
                ]),
                DataTable(
                    columns: const [
                        DataColumn(label: SizedBox(width: 100,child: Text("Name"),)),
                        DataColumn(label: Expanded(child: Text("Value"))),
                    ],
                    rows: _buildAttrRows(printerAttrs, ippSearch),
                ),
            ])),
        );
    }


    // Build the filtered rows of attributes
    List<DataRow> _buildAttrRows(Map<String,dynamic> attrs, String search) {
        var list = <DataRow>[];
        var sortedAttrs = SplayTreeMap<String, dynamic>.from(attrs, (k1, k2) => k1.compareTo(k2));

        sortedAttrs.forEach((key,value){
            if (key != "group-tag" && (search == "" || key.contains(search) || "$value".contains(search))) {
                list.add(DataRow(cells: [
                    DataCell(Text(key)),
                    DataCell(Expanded(child: Text("$value", softWrap: true)), onTap:(){ _tapValue(context, "$value"); }),
                ]));
            }
        });

        return (list);
    }


    // Build the filtered rows of TXT record keys/values
    List<DataRow> _buildTxtRows(Map<String, Uint8List?>? txt, String search) {
      var list = <DataRow>[ ];

      if (txt != null) {
        var sortedTxt = SplayTreeMap<String, Uint8List?>.from(txt, (k1, k2) => k1.compareTo(k2));
        sortedTxt.forEach((key,value){
          var svalue = "<null>";
          if (value != null) {
            svalue = const Utf8Decoder().convert(value);
          }

          // Filter using search string
          if (search == "" || key.contains(search) || svalue.contains(search)) {
            list.add(DataRow(cells: [
              DataCell(Text(key)),
              DataCell(Expanded(child: Text(svalue, softWrap: true)), onTap:(){ _tapValue(context, svalue); }),
            ]));
          }
        });
      }

      return (list);
    }


    // Get a list of attribute name and value data rows
    Future<void> _getAttrList(Service printer) async {
      if (printer.host != null && printer.port != null && printer.txt != null) {
        // Have a hostname, port, and TXT record...
        var rp = "/ipp/print";

        if (printer.txt!["rp"] != null) {
          // Use the "rp" value from the TXT record...
          Uint8List rpraw = printer.txt!["rp"]!;
          rp = const Utf8Decoder().convert(rpraw);
          if (rp[0] != '/') {
            rp = "/$rp";
          }
        }

        // Build the printer URI...
        var uri = "ipp://${printer.host}:${printer.port}$rp";
        //print("printer URI = $uri\n");

        // Get the attributes as a JSON object
        ipptoolGetAttributes(printerUri: uri).then((attrs){
          var icon = "";
          var state = "Unknown";
          var stateReasons = "";

          attrs.forEach((key,value){
              //print("'$key' = '$value'\n");

              if (key == "printer-icons") {
                  icon = value.last;
              } else if (key == "printer-state") {
                  if (value == 3) {
                      state = "Idle";
                  } else if (value == 4) {
                      state = "Processing";
                  } else {
                      state = "Stopped";
                  }
               } else if (key == "printer-state-reasons" && value != "none") {
                   stateReasons = ", $value";
               }
          });

          setState((){
              printerAttrs = attrs;
              if (icon != "") {
                  printerIcon = Image.network(icon, width: 160);
              } else {
                  printerIcon = Image.asset("images/missing-icon.png", width: 160);
              }
              printerState = "$state$stateReasons";
              printerUri   = uri;
          });
        });
      }
    }
}
