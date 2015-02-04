#ifndef LOGVIEWAPP_INCLUDED
#define LOGVIEWAPP_INCLUDED

#include <wx/app.h>
#include <wx/config.h>

class LogViewFrame;

extern wxConfigBase *Config;

class LogViewApp : public wxApp
{
    LogViewFrame *m_frame;
    wxString m_openFile;

public:
    LogViewApp();

private:
    bool OnInit();
    int OnExit();
    void OnInitCmdLine(wxCmdLineParser& parser);
    bool OnCmdLineParsed(wxCmdLineParser& parser);
};

wxDECLARE_APP(LogViewApp);

#endif
