import 'package:flutter/material.dart';
import 'dart:async';
import 'package:flutter_nsd/flutter_nsd.dart';


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

class IppEveHomePage extends StatefulWidget {
  const IppEveHomePage({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  State<IppEveHomePage> createState() => _IppEveHomePageState();
}

class _IppEveHomePageState extends State<IppEveHomePage> {
  final flutterNsd = FlutterNsd();
  final printers = <NsdServiceInfo>[];

  Future<void> startDiscovery() async {
    await flutterNsd.discoverServices("_ipp._tcp.");
  }

  @override
  void initState() {
    super.initState();

    flutterNsd.stream.listen(
      (NsdServiceInfo printer) {
        setState(() {
          printers.add(printer);
        });
      },
      onError: (e) {
      },
    );

    startDiscovery();
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

  _onPrinterTap(BuildContext context, NsdServiceInfo printer) {
    print(printer.name);
  }
}
