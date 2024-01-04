import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:nsd/nsd.dart';
import 'package:url_launcher/url_launcher.dart';

void main() {
  runApp(const IppEveToolApp());
}


void _tapValue(BuildContext context, String value) {
  if (value.startsWith("http://") || value.startsWith("https://")) {
    // Open URL...
    launchUrl(Uri.parse(value));
  }

  // Copy to clipboard and let the user know what happened...
  Clipboard.setData(ClipboardData(text: value));
  ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text('Copied "$value" to clipboard.'), duration: const Duration(seconds: 1)));
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
        setState((){ printers.add(service); });
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
      body: Column(children: [
              Row(children: [
                Image.network("http://www.pwg.org/ipp/ipp-everywhere.png",
                  width: 160,
                ),
                const Text(
                  "idle, accepting jobs\n'printer state message'\nmedia-empty,  toner-low, cover-open",
                  textScaleFactor: 1.5,
                ),
              ]),
              const Row(children: [
                Text("     TXT Record:", textAlign: TextAlign.left, style: TextStyle(fontWeight: FontWeight.bold)),
              ]),
              Expanded(child: SingleChildScrollView(child: DataTable(columns: const [
                  DataColumn(label: SizedBox(width: 100,child: Text("Key"),)),
                  DataColumn(label: Expanded(child: Text("Value"))),
                ],
                rows: _buildTxtRows(widget.printer.txt),
              ))),
            ]),
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
        items: const [
          BottomNavigationBarItem(
            icon: Icon(Icons.home),
            label: "Overview",
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.list),
            label: "IPP",
            tooltip: "IPP Attributes",
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.print),
            label: "Print",
            tooltip: "Print File",
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.run_circle),
            label: "Self-Cert",
            tooltip: "Self-Certification",
          ),
        ],
        currentIndex: 0,
        selectedItemColor: const Color(0xff4b5aa8),
        unselectedItemColor: const Color(0xff94a4ff),
      ),
    );
  }

  List<DataRow> _buildTxtRows(Map<String, Uint8List?>? txt) {
    var list = <DataRow>[ ];

    if (txt != null) {
      txt.forEach((key,value){
        var svalue = "<null>";
        if (value != null) {
          svalue = const Utf8Decoder().convert(value);
        }

        list.add(DataRow(cells: [
          DataCell(Text(key)),
          DataCell(Expanded(child: Text(svalue, softWrap: true)), onTap:(){ _tapValue(context, svalue); }),
        ]));
      });
    }

    return (list);
  }
}
