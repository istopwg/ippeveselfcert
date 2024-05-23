//
// IPP Everywhere Tool (ippevetool) main entry and window.
//
// Copyright © 2022-2024 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

import 'dart:core';
import 'dart:collection';
import 'dart:io';
import 'package:flutter/cupertino.dart';
import 'package:nsd/nsd.dart';
import "IppPrinter.dart";
import "IppPrinterPage.dart";


void main() {
    HttpOverrides.global = IppHttpOverrides();

    runApp(const IppEveToolApp());
}


// Allow all certificates, even self-signed which are almost always used for printers and other IoT devices...
class IppHttpOverrides extends HttpOverrides{
    @override
    HttpClient createHttpClient(SecurityContext? context) {
        return super.createHttpClient(context)
            ..badCertificateCallback = (X509Certificate cert, String host, int port) => true;
    }
}


// Main application class...
class IppEveToolApp extends StatelessWidget {
    const IppEveToolApp({super.key});

    static const version = "2.0.0";

    @override
    Widget build(BuildContext context) {
        return const CupertinoApp(
            title: "IPP Everywhere™ Tool v$version",
            home: IppEveToolHomePage(title: "IPP Everywhere™ Tool v$version"),
            debugShowCheckedModeBanner: false,
        );
    }
}


// Home page/screen/window/whatever where printers are discovered and shown...
class IppEveToolHomePage extends StatefulWidget {
    const IppEveToolHomePage({super.key, required this.title});

    final String title;

    @override
    State<IppEveToolHomePage> createState() => _IppEveToolHomePageState();
}


// State for the home page...
class _IppEveToolHomePageState extends State<IppEveToolHomePage> {
    final printers = SplayTreeMap<String, IppPrinter>((a, b) => a.compareTo(b));

    Future<void> _startDiscovery() async {
//         List<String> uris = [
//             "ipp://9F706F000000.local/ipp/print",
//             "ipps://EPSONFC82D0.local/ipp/print",
//             "ipp://HPFC15B43483FE.local/ipp/print",
//             "ipps://HP3024A9692825.local/ipp/print",
//             "ipp://NPI5656DB.local/ipp/print",
//             "ipps://ET788C775031DD.local:443/ipp/print",
//             "ipp://et0021b7017d7b.local/ipp/print",
//             "ipps://rollo-08-b0-2b.local/ipp/print",
//             "ipp://rollo-e5-47-f8.local/ipp/print",
//         ];
// 
//         for (String uri in uris) {
//             print("Trying to add $uri...");
// 
//             final printer = await ippPrinterWithURI(uri);
// 
//             setState((){
//                 print("Adding '${printer.dnssdName}' from $uri...");
//                 printers[printer.dnssdName] = printer;
//             });
//         }

        // Look for IPPS printers...
        final ippsBrowser = await startDiscovery("_ipps._tcp.", autoResolve: true);
        ippsBrowser.addServiceListener((service, status) async {
            // print("${service.name} => ${status}");
            if (status == ServiceStatus.found) {
                // print("Trying to add '${service.name}'...");
                final printer = await ippPrinterWithService(service);

                setState((){
                    // print("Adding ${printer.uri} from '${service.name}'...");
                    printers[printer.dnssdName] = printer;
                });
            }
        });

    }


    // This method initializes the state object and starts DNS-SD discovery...
    @override
    void initState() {
        super.initState();

        _startDiscovery();
    }


    // This method builds the UI...
    @override
    Widget build(BuildContext context) {
        return CupertinoPageScaffold(
            navigationBar: CupertinoNavigationBar(
                middle: Text(widget.title),
            ),
            child: SingleChildScrollView(
                child: _buildList(context),
            ),
        );
    }


    // This method builds the printer list or spinner that goes on the home page...
    _buildList(BuildContext context) {
        if (printers.isEmpty) {
            return const CupertinoActivityIndicator();
        }

        var tiles = <CupertinoListTile>[];

        printers.forEach((dnssdName, printer) {
            tiles.add(CupertinoListTile(
                leading: printer.getImage(),
                title: Text(dnssdName),
                subtitle: Text(printer.location),
                trailing: const CupertinoListTileChevron(),
                onTap: () => Navigator.of(context).push(CupertinoPageRoute<void>(
                    builder: (BuildContext context) {
                        return IppPrinterPage(printer: printer);
                    },
                )),
            ));
        });

        return CupertinoListSection(
            header: const Text('Printers'),
            children: tiles,
        );
    }
}
