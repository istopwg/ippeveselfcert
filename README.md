IPP Everywhere™ v2.0 Printer Self-Certification Tools
=====================================================

> Note: The current v1.1 self-certification tools are provided in the
> "v1.1-updates" branch of this repository.

![Version](https://img.shields.io/github/v/release/istopwg/ippeveselfcert?include_prereleases)
![Apache 2.0](https://img.shields.io/github/license/istopwg/ippeveselfcert)
![Build and Test](https://github.com/istopwg/ippeveselfcert/workflows/Build%20and%20Test/badge.svg)
[![ippeveselfcert](https://snapcraft.io/ippeveselfcert/badge.svg)](https://snapcraft.io/ippeveselfcert)
[![Coverity Scan Status](https://img.shields.io/coverity/scan/25371.svg)](https://scan.coverity.com/projects/istopwg-ippeveselfcert)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/istopwg/ippeveselfcert.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/istopwg/ippeveselfcert/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/istopwg/ippeveselfcert.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/istopwg/ippeveselfcert/context:cpp)

The IPP Everywhere™ Printer self-certification tools are used to test the
conformance of printers to PWG Candidate Standard 5100.14-20xx: IPP Everywhere™
v2.0. The testing and submission procedures are defined in PWG Candidate
Standard 5100.20-20xx: IPP Everywhere™ v2.0 Printer Self-Certification Manual.

The [IPP Everywhere™ home page](http://www.pwg.org/ipp/everywhere.html) provides
access to all information relevant to IPP Everywhere™.  The
"ippeveselfcert@pwg.org" mailing list is used to discuss IPP Everywhere Printer
Self-Certification.  You can subscribe at
<https://www.pwg.org/mailman/listinfo/ippeveselfcert>.

Issues found in the tools should be reported using the Github issues page at
<https://github.com/istopwg/ippeveselfcert>.

> Note: Tests are intended to be run on an isolated network, or at least when no
> other users are printing using the target printer.  Otherwise the test scripts
> will fail randomly.


Compiling
---------

Please see the file "BUILD.md" for instructions on compiling the software.

> Note: Self-certification results submitted to the PWG IPP Everywhere™ portal
> MUST be generated using the tools provides on the PWG web site.


Legal Stuff
-----------

Copyright © 2014-2022 by the IEEE-ISTO Printer Working Group.
Copyright © 2007-2019 by Apple Inc.
Copyright © 1997-2007 by Easy Software Products.

This software is provided under the terms of the Apache License, Version 2.0.
A copy of this license can be found in the file `LICENSE`.  Additional legal
information is provided in the file `NOTICE`.

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.  See the License for the
specific language governing permissions and limitations under the License.
