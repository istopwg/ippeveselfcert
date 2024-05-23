//
// IPP Everywhere Tool (ippevetool) TXT record page.
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
import "util.dart";


// Home page/screen/window/whatever where printers are discovered and shown...
class TxtRecordPage extends StatefulWidget {
    const TxtRecordPage({super.key, required this.printer});

    final IppPrinter printer;

    @override
    State<TxtRecordPage> createState() => _TxtRecordPageState();
}


// State for the home page...
class _TxtRecordPageState extends State<TxtRecordPage> {
    TextEditingController searchController = TextEditingController();
    String search = "";

    @override
    Widget build(BuildContext context) {
        return CupertinoPageScaffold(
            navigationBar: CupertinoNavigationBar(
                middle: CupertinoSearchTextField(
                    controller: searchController,
                    placeholder: "Search TXT Record",
                    onChanged: (String value) async {
                        setState(() {
                            search = value;
                        });
                    },
                ),
            ),
            child: SingleChildScrollView(
                child: CupertinoListSection(
                    header: const Text("TXT Key/Value Pairs"),
                    children: _buildList(),
                ),
            ),
        );
    }

    // Build the filtered rows of TXT record keys/values
    List<CupertinoListTile> _buildList() {
        var list = <CupertinoListTile>[ ];

        widget.printer.txt.forEach((key,value){
            // Filter using search string
            if (search == "" || key.contains(search) || value.contains(search)) {
                list.add(CupertinoListTile(
                    title: Text(key),
                    subtitle: Text(value),
                    leadingSize: 0,
                    leadingToTitle: 0,
                    onTap: () => tapValue(context, value),
                ));
            }
        });

      return (list);
    }
}
