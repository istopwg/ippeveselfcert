//
// IPP Everywhere Tool (ippevetool) utility functions.
//
// Copyright © 2022-2024 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

import 'dart:core';
import 'package:flutter/cupertino.dart';
import 'package:flutter/services.dart';
import 'package:url_launcher/url_launcher.dart';


// List picker page
class PickerPage extends StatefulWidget {
    const PickerPage({super.key, required this.title, required this.defaultValue, required this.values, required this.callback});

    final String title;
    final String defaultValue;
    final Map<String,String> values;
    final void Function(String value) callback;

    @override
    State<PickerPage> createState() => _PickerPageState();
}


// State for the home page...
class _PickerPageState extends State<PickerPage> {
    String currentValue = "";

    @override
    void initState() {
        super.initState();

        currentValue = widget.defaultValue;
    }

    @override
    Widget build(BuildContext context) {
        return CupertinoPageScaffold(
            navigationBar: CupertinoNavigationBar(
                middle: Text(widget.title),
                trailing: CupertinoButton(
                    onPressed: (){
                        widget.callback(currentValue);
                        Navigator.pop(context);
                    },
                    padding: EdgeInsets.zero,
                    child: const Text("Choose"),
                ),
            ),
            child: SingleChildScrollView(
                child: CupertinoListSection(
                    header: Text(widget.title),
                    children: _buildListTiles(),
                ),
            ),
        );
    }

    // Build the list tiles for the picker...
    List<CupertinoListTile> _buildListTiles() {
        var tiles = <CupertinoListTile>[];

        widget.values.forEach((key,value){
            tiles.add(CupertinoListTile(
                title: Text(value),
                trailing: key == currentValue ? const Icon(CupertinoIcons.checkmark) : null,
                onTap: (){
                    setState((){
                        currentValue = key;
                    });
                },
            ));
        });

        return (tiles);
    }
}


// Show an alert dialog with an OK button
void showAlert(BuildContext context, String message) {
    showCupertinoModalPopup<void>(
        context: context,
        builder: (BuildContext context) => CupertinoAlertDialog(
            title: const Text('Alert'),
            content: Text(message),
            actions: <CupertinoDialogAction>[
                CupertinoDialogAction(
                    isDefaultAction: true,
                    onPressed: () {
                        Navigator.pop(context);
                    },
                    child: const Text('OK'),
                ),
            ],
        ),
    );
}


// Show a popup message that disappears after a short interval
void showPopup(BuildContext context, String message, int duration) {
    final overlayEntry = OverlayEntry(
        builder: (context) => Positioned(
            bottom: 8.0,
            left: 8.0,
            right: 8.0,
            child: CupertinoPopupSurface(
                child: Padding(
                    padding: const EdgeInsets.symmetric(
                        horizontal: 8.0,
                        vertical: 8.0,
                    ),
                    child: Text(message, style: const TextStyle(fontSize: 14.0, color: CupertinoColors.secondaryLabel), textAlign: TextAlign.center),
                ),
            ),
        ),
    );

    Future.delayed(
        Duration(milliseconds: duration),
        overlayEntry.remove,
    );

    Overlay.of(context).insert(overlayEntry);
}


// Tap handler - copy to the clipboard and open URLs...
void tapValue(BuildContext context, String value) {
    if (value.startsWith("http://") || value.startsWith("https://")) {
        // Open URL...
        launchUrl(Uri.parse(value));
    }

    // Copy to clipboard and let the user know what happened...
    Clipboard.setData(ClipboardData(text: value));
    showPopup(context, 'Copied "$value" to clipboard.', 1500);
}

