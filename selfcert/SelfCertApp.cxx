// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "SelfCertApp.h"

void SelfCertApp::cb_browser_i(Fl_Browser*, void*) {
  test(browser->text(browser->value()));
}
void SelfCertApp::cb_browser(Fl_Browser* o, void* v) {
  ((SelfCertApp*)(o->parent()->parent()->user_data()))->cb_browser_i(o,v);
}

void SelfCertApp::cb_resultsSubmit_i(Fl_Button*, void*) {
  submissionForm->hotspot(window->x() + window->w() / 2, window->y() + window->h() / 2);
  submissionForm->show();
}
void SelfCertApp::cb_resultsSubmit(Fl_Button* o, void* v) {
  ((SelfCertApp*)(o->parent()->parent()->user_data()))->cb_resultsSubmit_i(o,v);
}

void SelfCertApp::cb_Cancel_i(Fl_Button*, void*) {
  submissionForm->hide();
}
void SelfCertApp::cb_Cancel(Fl_Button* o, void* v) {
  ((SelfCertApp*)(o->parent()->user_data()))->cb_Cancel_i(o,v);
}

/**
 Constructor
*/
SelfCertApp::SelfCertApp() {
  { window = new Fl_Double_Window(800, 600, "IPP Everywhere\342\204\242 Printer Self-Certification Tool v2.0");
    window->color((Fl_Color)55);
    window->user_data((void*)(this));
    window->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));
    { tile = new Fl_Tile(0, 0, 800, 555);
      { browser = new Fl_Browser(0, 0, 800, 100);
        browser->type(1);
        browser->box(FL_DOWN_BOX);
        browser->callback((Fl_Callback*)cb_browser);
      } // Fl_Browser* browser
      { output = new Fl_Text_Display(0, 100, 800, 455);
        output->box(FL_DOWN_BOX);
        output->color((Fl_Color)34);
        output->textfont(4);
        output->textcolor(FL_BACKGROUND2_COLOR);
        Fl_Group::current()->resizable(output);
      } // Fl_Text_Display* output
      tile->end();
    } // Fl_Tile* tile
    { results = new Fl_Group(0, 555, 800, 45);
      { resultsSubmit = new Fl_Button(650, 565, 65, 25, "Submit");
        resultsSubmit->down_box(FL_DOWN_BOX);
        resultsSubmit->callback((Fl_Callback*)cb_resultsSubmit);
        resultsSubmit->deactivate();
      } // Fl_Button* resultsSubmit
      { resultsCancel = new Fl_Button(725, 565, 65, 25, "Cancel");
        resultsCancel->down_box(FL_DOWN_BOX);
      } // Fl_Button* resultsCancel
      results->end();
    } // Fl_Group* results
    window->end();
  } // Fl_Double_Window* window
  { submissionForm = new Fl_Double_Window(400, 445, "Submission Form");
    submissionForm->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(10, 30, 380, 145, "Questions:");
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { usedPWGTools = new Fl_Check_Button(20, 45, 360, 20, "Did you use the PWG-supplied self-certification tools?");
        usedPWGTools->down_box(FL_DOWN_BOX);
      } // Fl_Check_Button* usedPWGTools
      { usedProductionReadyCode = new Fl_Check_Button(20, 70, 360, 20, "Did you use production-ready code?");
        usedProductionReadyCode->down_box(FL_DOWN_BOX);
      } // Fl_Check_Button* usedProductionReadyCode
      { printedCorrectly = new Fl_Check_Button(20, 95, 360, 20, "Did all output print correctly?");
        printedCorrectly->down_box(FL_DOWN_BOX);
      } // Fl_Check_Button* printedCorrectly
      { isPrintServer = new Fl_Check_Button(20, 120, 360, 20, "Is this a print server?");
        isPrintServer->down_box(FL_DOWN_BOX);
      } // Fl_Check_Button* isPrintServer
      { partOfFirmwareUpdate = new Fl_Check_Button(20, 145, 360, 20, "Is IPP Everywhere support part of a firmware update?");
        partOfFirmwareUpdate->down_box(FL_DOWN_BOX);
      } // Fl_Check_Button* partOfFirmwareUpdate
      o->end();
    } // Fl_Group* o
    { productFamilyName = new Fl_Input(10, 195, 380, 25, "Product Family Name:");
      productFamilyName->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Input* productFamilyName
    { productFamilyURL = new Fl_Input(10, 245, 380, 25, "Product Family Web Page (URL)");
      productFamilyURL->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Input* productFamilyURL
    { modelNames = new Fl_Input(10, 295, 380, 100, "Model Names:");
      modelNames->type(4);
      modelNames->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Input* modelNames
    { generateJSON = new Fl_Button(165, 410, 150, 25, "Generate JSON File");
      generateJSON->deactivate();
    } // Fl_Button* generateJSON
    { Fl_Button* o = new Fl_Button(325, 410, 65, 25, "Cancel");
      o->callback((Fl_Callback*)cb_Cancel);
    } // Fl_Button* o
    submissionForm->set_modal();
    submissionForm->end();
  } // Fl_Double_Window* submissionForm
  browser->add("Test Printer");

  buffer = new Fl_Text_Buffer();
  output->buffer(buffer);
}

SelfCertApp::~SelfCertApp() {
  delete window;
}

void SelfCertApp::show(int argc, char *argv[]) {
  window->show(argc, argv);
}
