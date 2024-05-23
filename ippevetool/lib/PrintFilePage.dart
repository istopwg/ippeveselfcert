//
// IPP Everywhere Tool (ippevetool) Print File page.
//
// Copyright © 2024 by the IEEE-ISTO Printer Working Group.
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
import 'package:file_picker/file_picker.dart';


// Print File page
class PrintFilePage extends StatefulWidget {
    const PrintFilePage({super.key, required this.printer});

    final IppPrinter printer;

    @override
    State<PrintFilePage> createState() => _PrintFilePageState();
}


// State for the home page...
class _PrintFilePageState extends State<PrintFilePage> {
    String filename = "";
    String format = "";
    String jobName = "Untitled";
    TextEditingController jobNameController = TextEditingController(text:"Untitled");
    int copies = 1;
    List<int> finishings = [];
    String media = "";
    String printColorMode = "";
    String sides = "";

    Map<String,String> finishingsMap = {
        "10": "Fold",
        "100": "Z fold",
        "101": "Engineering Z fold",
        "11": "Cut",
        "12": "Bale",
        "13": "Booklet maker",
        "14": "Jog offset",
        "15": "Coat",
        "16": "Laminate",
        "20": "Staple top left",
        "21": "Staple bottom left",
        "22": "Staple top right",
        "23": "Staple bottom right",
        "24": "Edge stitch left",
        "25": "Edge stitch top",
        "26": "Edge stitch right",
        "27": "Edge stitch bottom",
        "28": "2 staples on left",
        "29": "2 staples on top",
        "3": "None",
        "30": "2 staples on right",
        "31": "2 staples on bottom",
        "32": "3 staples on left",
        "33": "3 staples on top",
        "34": "3 staples on right",
        "35": "3 staples on bottom",
        "4": "Staple",
        "5": "Punch",
        "50": "Bind left",
        "51": "Bind top",
        "52": "Bind right",
        "53": "Bind bottom",
        "6": "Cover",
        "60": "Cut after every page",
        "61": "Cut after every document",
        "62": "Cut after every set",
        "63": "Cut after job",
        "7": "Bind",
        "70": "Punch top left",
        "71": "Punch bottom left",
        "72": "Punch top right",
        "73": "Punch bottom right",
        "74": "2-hole punch left",
        "75": "2-hole punch top",
        "76": "2-hole punch right",
        "77": "2-hole punch bottom",
        "78": "3-hole punch left",
        "79": "3-hole punch top",
        "8": "Saddle stitch",
        "80": "3-hole punch right",
        "81": "3-hole punch bottom",
        "82": "4-hole punch left",
        "83": "4-hole punch top",
        "84": "4-hole punch right",
        "85": "4-hole punch bottom",
        "86": "Multi-hole punch left",
        "87": "Multi-hole punch top",
        "88": "Multi-hole punch right",
        "89": "Multi-hole punch bottom",
        "9": "Edge stitch",
        "90": "Accordion fold",
        "91": "Double gate fold",
        "92": "Gate fold",
        "93": "Half fold",
        "94": "Half Z fold",
        "95": "Left gate fold",
        "96": "Letter fold",
        "97": "Parallel fold",
        "98": "Poster fold",
        "99": "Right gate fold",
    };
    Map<String,String> formatMap = {
        "application/octet-stream": "Auto-Detect",
        "application/pclm": "PCLm",
        "application/pdf": "PDF",
        "application/postscript": "PostScript",
        "application/vnd.hp-pcl": "HP PCL",
        "image/jpeg": "JPEG",
        "image/png": "PNG",
        "image/pwg-raster": "PWG Raster",
        "image/urf": "Apple Raster",
        "text/html": "HTML",
        "text/plain": "Plain Text",
    };
    Map<String,String> printColorModeMap = {
        "": "Not supported",
        "auto": "Automatic",
        "auto-monochrome": "Auto monochrome",
        "bi-level": "Text",
        "color": "Color",
        "highlight": "Highlight",
        "monochrome": "Monochrome",
        "process-bi-level": "Process text",
        "process-monochrome": "Process monochrome",
    };
    Map<String,String> sidesMap = {
        "": "Not supported",
        "one-sided": "Off",
        "two-sided-long-edge": "On (portrait)",
        "two-sided-short-edge": "On (landscape)",
    };


    @override
    void initState() {
        super.initState();

        format         = widget.printer.attributes["document-format-default"] ?? "application/octet-stream";
        copies         = widget.printer.attributes["copies-default"] ?? 1;
        media          = widget.printer.attributes["media-default"] ?? "na_letter_8.5x11in";
        printColorMode = widget.printer.attributes["print-color-mode-default"] ?? "";
        sides          = widget.printer.attributes["sides-default"] ?? "";
    }

    @override
    Widget build(BuildContext context) {
        return CupertinoPageScaffold(
            navigationBar: CupertinoNavigationBar(
                middle: const Text("Print a File"),
                trailing: CupertinoButton(
                    onPressed: (){
                        printFile();
                    },
                    padding: EdgeInsets.zero,
                    child: const Text("Print"),
                ),
            ),
            child: SingleChildScrollView(
                child: Column(children: <Widget>[
                    CupertinoListSection(
                        header: const Text("File Information"),
                        children: <Widget>[
                            CupertinoListTile(
                                title: const Text("Filename"),
                                trailing: Row(
                                    children: [
                                        Text(style:const TextStyle(color:CupertinoColors.activeBlue), "$filename  "),
                                        const CupertinoListTileChevron(),
                                    ],
                                ),
                                onTap: () async {
                                    FilePickerResult? result = await FilePicker.platform.pickFiles();

                                    if (result != null) {
                                        setState((){
                                            filename = result.files.single.path!;
                                        });
                                    }
                                },
                            ),
                            CupertinoListTile(
                                title: const Text("Format"),
                                trailing: Row(
                                    children: [
                                        Text(style:const TextStyle(color:CupertinoColors.activeBlue), formatMap[format.toLowerCase()] ?? format),
                                        const Text("  "),
                                        const CupertinoListTileChevron(),
                                    ],
                                ),
                                onTap: () {
                                    Navigator.of(context).push(
                                        CupertinoPageRoute<void>(
                                            builder: (BuildContext context) {
                                                return PickerPage(
                                                    title: "Format",
                                                    defaultValue: format,
                                                    values: makeGenericMap("document-format", formatMap),
                                                    callback: (String value){
                                                        setState(() {
                                                            format = value;
                                                        });
                                                    },
                                                );
                                            },
                                        )
                                    );
                                },
                            ),
                        ],
                    ),
                    CupertinoListSection(
                        header: const Text("Job Ticket"),
                        children: <Widget>[
                            CupertinoListTile(
                                title: const Text("Title"),
                                trailing: ConstrainedBox(
                                    constraints: const BoxConstraints(maxWidth: 300),
                                        child: CupertinoTextField(
                                        controller: jobNameController,
                                        maxLength: 127,
                                        onChanged: (String value) async {
                                            setState(() {
                                                jobName = value;
                                            });
                                        },
                                    ),
                                ),
                            ),
                            CupertinoListTile(
                                title: const Text("Copies"),
                                trailing: Row(
                                    children: <Widget>[
                                        GestureDetector(
                                            child: const Icon(CupertinoIcons.minus_circled),
                                            onTap: () async {
                                                setState(() {
                                                    if (copies > 1) {
                                                        copies --;
                                                    }
                                                });
                                            },
                                        ),
                                        Text(style:const TextStyle(color:CupertinoColors.activeBlue), "  $copies  "),
                                        GestureDetector(
                                            child: const Icon(CupertinoIcons.add_circled),
                                            onTap: () async {
                                                setState(() {
                                                    if (copies < 999) {
                                                        copies ++;
                                                    }
                                                });
                                            },
                                        ),
                                    ],
                                ),
                            ),
                            CupertinoListTile(
                                title: const Text("Media"),
                                trailing: Row(
                                    children: [
                                        Text(style:const TextStyle(color:CupertinoColors.activeBlue), localizeMedia(media)),
                                        const Text("  "),
                                        const CupertinoListTileChevron(),
                                    ],
                                ),
                                onTap: () => Navigator.of(context).push(CupertinoPageRoute<void>(
                                    builder: (BuildContext context) {
                                        return PickerPage(title:"Media", defaultValue:media, values:makeMediaMap(), callback:(String value){
                                            setState(() {
                                                media = value;
                                            });
                                        });
                                    },
                                )),
                            ),
                            CupertinoListTile(
                                title: const Text("2-Sided Printing"),
                                trailing: Row(
                                    children: [
                                        Text(style:const TextStyle(color:CupertinoColors.activeBlue), sidesMap[sides] ?? sides),
                                        const Text("  "),
                                        const CupertinoListTileChevron(),
                                    ],
                                ),
                                onTap: () => Navigator.of(context).push(CupertinoPageRoute<void>(
                                    builder: (BuildContext context) {
                                        return PickerPage(title:"2-Sided Printing", defaultValue:sides, values:makeGenericMap("sides", sidesMap), callback:(String value){
                                            setState(() {
                                                sides = value;
                                            });
                                        });
                                    },
                                )),
                            ),
                            CupertinoListTile(
                                title: const Text("Print Mode"),
                                trailing: Row(
                                    children: [
                                        Text(style:const TextStyle(color:CupertinoColors.activeBlue), printColorModeMap[printColorMode] ?? printColorMode),
                                        const Text("  "),
                                        const CupertinoListTileChevron(),
                                    ],
                                ),
                                onTap: () => Navigator.of(context).push(CupertinoPageRoute<void>(
                                    builder: (BuildContext context) {
                                        return PickerPage(title:"Print Mode", defaultValue:printColorMode, values:makeGenericMap("print-color-mode", printColorModeMap), callback:(String value){
                                            setState(() {
                                                printColorMode = value;
                                            });
                                        });
                                    },
                                )),
                            ),
                            CupertinoListTile(
                                title: const Text("Finishings"),
                                trailing: Row(
                                    children: [
                                        Text(style:const TextStyle(color:CupertinoColors.activeBlue), localizeFinishings(finishings)),
                                        const Text("  "),
                                        const CupertinoListTileChevron(),
                                    ],
                                ),
                                onTap: () => Navigator.of(context).push(CupertinoPageRoute<void>(
                                    builder: (BuildContext context) {
                                        return FinishingsPage(title:"Finishings", defaultValue:finishings, values:makeGenericMap("finishings", finishingsMap), callback:(List<int> value){
                                            setState(() {
                                                finishings = value;
                                            });
                                        });
                                    },
                                )),
                            ),
                        ],
                    ),
                ]),
            ),
        );
    }

    // Localize a finishings value
    String localizeFinishings(List<int> finishings) {
        String text = "";

        for (int fin in finishings) {
            text += finishingsMap["$fin"] ?? "$fin";
            text += " ";
        }

        if (text == "") {
            text = "None";
        }

        return text;
    }


    // Localize a media size name
    String localizeMedia(String keyword) {
        switch (keyword) {
            case "na_letter_8.5x11in" :
                return "US Letter";
            case "na_legal_8.5x14in" :
                return "US Legal";
            case "na_number-10_4.125x9.5in" :
                return "US #10 Envelope";
            case "na_tabloid_11x17in" :
                return "US Tabloid";
            case "iso_a4_210x297mm" :
                return "ISO A4";
            case "iso_a3_210x297mm" :
                return "ISO A3";
            case "iso_dl_110x220mm" :
                return "ISO DL Envelope";
        }

        // Return dimensional size from the end...
        return keyword.split("_")[2];
    }


    // Create localized list of keyword choices
    Map<String,String> makeGenericMap(String attrName, Map<String,String> map) {
        var strings = <String,String>{};
        var xxxSupported = widget.printer.attributes["$attrName-supported"];

        if (xxxSupported is List) {
            for (var keyword in xxxSupported) {
                strings["$keyword"] = map["$keyword".toLowerCase()] ?? "$keyword";
            }
        } else if (xxxSupported != null) {
            strings["$xxxSupported"] = map["$xxxSupported".toLowerCase()] ?? "$xxxSupported";
        }

        return (strings);
    }


    // Create localized list of media sizes
    Map<String,String> makeMediaMap() {
        var media = <String,String>{};

        for (String keyword in widget.printer.attributes["media-supported"]) {
            media[keyword] = localizeMedia(keyword);
        }

        return (media);
    }


    // Actually print a file
    Future<void> printFile() async {
        int jobId = 42;

        showCupertinoModalPopup<void>(
            context: context,
            builder: (BuildContext context) => CupertinoAlertDialog(
                title: const Text('File Printed'),
                content: Text("Job created successfully: job-id=$jobId"),
                actions: <CupertinoDialogAction>[
                    CupertinoDialogAction(
                        isDefaultAction: true,
                        onPressed: () {
                            Navigator.pop(context);
                            Navigator.pop(context);
                        },
                        child: const Text('OK'),
                    ),
                    CupertinoDialogAction(
                        onPressed: () {
                            Navigator.pop(context);
                        },
                        child: const Text('Keep Printing'),
                    ),
                ],
            ),
        );
        return;
    }
}


// List picker page
class FinishingsPage extends StatefulWidget {
    const FinishingsPage({super.key, required this.title, required this.defaultValue, required this.values, required this.callback});

    final String title;
    final List<int> defaultValue;
    final Map<String,String> values;
    final void Function(List<int> value) callback;

    @override
    State<FinishingsPage> createState() => _FinishingsPageState();
}


// State for the home page...
class _FinishingsPageState extends State<FinishingsPage> {
    List<int> currentValue = [];

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
            int keyvalue = int.parse(key);

            tiles.add(CupertinoListTile(
                title: Text(value),
                trailing: currentValue.contains(keyvalue) ? const Icon(CupertinoIcons.checkmark) : null,
                onTap: (){
                    setState((){
                        if (keyvalue == 3) {
                            // Just "None"
                            currentValue = [ 3 ];
                        } else if (currentValue.contains(keyvalue)) {
                            currentValue.remove(keyvalue);
                        } else {
                            currentValue.add(keyvalue);
                            currentValue.remove(3);
                        }

                        if (currentValue.isEmpty) {
                            currentValue = [ 3 ];
                        }
                    });
                },
            ));
        });

        return (tiles);
    }
}
