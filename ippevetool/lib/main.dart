import 'dart:async';
import 'package:flutter/material.dart';
import 'package:nsd/nsd.dart';


void main() {
  runApp(const IppEveToolApp());
}


class IppEveToolApp extends StatelessWidget {
  const IppEveToolApp({super.key});

  // This widget is the root of your application.
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
    final discovery = await startDiscovery("_ipp._tcp.", autoResolve: false);
    discovery.addServiceListener((service, status) {
      print("${service.name} => ${status}");
      if (status == ServiceStatus.found) {
        printers.add(service);
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
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
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
//    print(printer.name);
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
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.printer.name ?? "Unknown"),
      ),
      body: ListView(children: [
              Row(children: [
                Image(
                  image: AssetImage("../libcups/tools/printer.png"),
                  width: 160,
                ),
                Text(
                  "idle, accepting jobs\n'printer state message'\nmedia-empty,  toner-low, cover-open",
                  textScaleFactor: 1.5,
                ),
              ]),
              DataTable(columns: [
                  DataColumn(label: Expanded(child: Text("Key"))),
                  DataColumn(label: Expanded(child: Text("Value"))),
                ],
                rows: [
                DataRow(cells: [
                  DataCell(Text("txtvers")),
                  DataCell(Text("1")),
                ]),
                DataRow(cells: [
                  DataCell(Text("qtotal")),
                  DataCell(Text("1")),
                ]),
                DataRow(cells: [
                  DataCell(Text("pdl")),
                  DataCell(Text("application/vnd.hp-PCL,application/vnd.hp-PCLXL,application/postscript,application/pdf,image/jpeg,application/PCLm,image/urf")),
                ]),
                DataRow(cells: [
                  DataCell(Text("rp")),
                  DataCell(Text("ipp/print")),
                ]),
                DataRow(cells: [
                  DataCell(Text("PaperMax")),
                  DataCell(Text("legal-A4")),
                ]),
                DataRow(cells: [
                  DataCell(Text("kind")),
                  DataCell(Text("document,envelope,photo,postcard")),
                ]),
                DataRow(cells: [
                  DataCell(Text("URF")),
                  DataCell(Text("CP1,MT1-2-8-9-10-11,PQ3-4-5,RS300-600,SRGB24,OB10,W8,DEVW8,DEVRGB24,ADOBERGB24,DM3,IS19-1-2,V1.4")),
                ]),
                DataRow(cells: [
                  DataCell(Text("ty")),
                  DataCell(Text("HP Officejet Pro X576dw MFP")),
                ]),
                DataRow(cells: [
                  DataCell(Text("product")),
                  DataCell(Text("(HP Officejet Pro X576dw MFP)")),
                ]),
                DataRow(cells: [
                  DataCell(Text("usb_MFG")),
                  DataCell(Text("HP")),
                ]),
                DataRow(cells: [
                  DataCell(Text("usb_MDL")),
                  DataCell(Text("HP Officejet Pro X576dw MFP")),
                ]),
                DataRow(cells: [
                  DataCell(Text("priority")),
                  DataCell(Text("20")),
                ]),
                DataRow(cells: [
                  DataCell(Text("adminurl")),
                  DataCell(Text("http://HPFC15B43483FE.local./#hId-pgAirPrint")),
                ]),
              ]),
            ]),
//          ListView(// DNS-SD Values
//            children: [
//            ],
//          ),
//          ListView(// IPP Attributes
//            children: [
//            ],
//          ),
//          ListView(// Print File
//            children: [
//            ],
//          ),
//          ListView(// Self-Certification
//            children: [
//            ],
//          ),
//        ],
//      ),
      bottomNavigationBar: BottomNavigationBar(
        items: [
          BottomNavigationBarItem(
            icon: Icon(Icons.home),
            label: "Overview",
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.explore),
            label: "DNS-SD Values",
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.list),
            label: "IPP Attributes",
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.print),
            label: "Print File",
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.run_circle),
            label: "Self-Certification",
          ),
        ],
      ),
    );
  }
}
