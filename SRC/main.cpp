#include "MyFrame.h"
#include <wx/wx.h>

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    wxInitAllImageHandlers();
    MyFrame* frame = new MyFrame("Project 11");
    frame->Show(true);
    return true;
}
