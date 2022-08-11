//
// Selfcert main entry for the IPP Everywhere Printer Self-Certification
// application.
//
// Copyright © 2019-2022 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.	See the file "LICENSE" for more
// information.
//

#include "SelfCertApp.h"
#include "selfcert.h"


//
// 'main()' - Main entry.
//

int					// O - Exit status
main(int  argc,				// I - Number of command-line arguments
     char *argv[])			// I - Command-line arguments
{
  SelfCertApp	*app = new SelfCertApp();
					// Self-certification application


  app->show(argc, argv);
  Fl::run();

  return (0);
}
