//
// IPP Everywhere Tool (ippevetool) IPP attributes page.
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
class IppAttributesPage extends StatefulWidget {
    const IppAttributesPage({super.key, required this.printer});

    final IppPrinter printer;

    @override
    State<IppAttributesPage> createState() => _IppAttributesPageState();
}


// State for the home page...
class _IppAttributesPageState extends State<IppAttributesPage> {
    TextEditingController searchController = TextEditingController();
    String search = "";

    @override
    Widget build(BuildContext context) {
        return CupertinoPageScaffold(
            navigationBar: CupertinoNavigationBar(
                middle: CupertinoSearchTextField(
                    controller: searchController,
                    placeholder: "Search IPP Attributes",
                    onChanged: (String value) async {
                        setState(() {
                            search = value;
                        });
                    },
                ),
            ),
            child: SingleChildScrollView(
                child: CupertinoListSection(
                    header: const Text("IPP Attributes"),
                    children: _buildList(),
                ),
            ),
        );
    }

    // Build the filtered rows of IPP attributes
    List<CupertinoListTile> _buildList() {
        var list = <CupertinoListTile>[ ];

        widget.printer.attributes.forEach((key,value){
            // Filter using search string
            if (key != "group-tag" && (search == "" || key.contains(search) || "$value".contains(search))) {
                list.add(CupertinoListTile(
                    title: Text(key),
                    subtitle: Text("$value"),
                    leadingSize: 0,
                    leadingToTitle: 0,
                    onTap: () => tapValue(context, "$value"),
                ));
            }
        });

      return (list);
    }
}
