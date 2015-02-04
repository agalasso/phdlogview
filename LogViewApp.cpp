#include "logviewapp.h"
#include "LogViewFrame.h"

#include <wx/cmdline.h>

wxConfigBase *Config = 0;

LogViewApp::LogViewApp()
    :
    m_frame(0)
{
    SetVendorName("adgsoftware");
    SetAppName("phdlogview");
    Config = wxConfig::Get();
}

bool LogViewApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    wxLog::SetActiveTarget(new wxLogStderr());

    m_frame = new LogViewFrame();
    m_frame->Show();

    if (!m_openFile.IsEmpty())
        m_frame->OpenLog(m_openFile);

    return true;
}

int LogViewApp::OnExit()
{
    return wxApp::OnExit();
}

void LogViewApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.AddParam("filename", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
}

bool LogViewApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (parser.GetParamCount() > 1)
        return false;
    if (parser.GetParamCount() == 1)
        m_openFile = parser.GetParam(0);
    return true;
}

wxIMPLEMENT_APP(LogViewApp);
