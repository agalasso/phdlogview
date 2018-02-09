/*
 * This file is part of phdlogview
 *
 * Copyright (C) 2016 Andy Galasso
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, visit the http://fsf.org website.
 */

#include "LogViewApp.h"
#include "LogViewFrame.h"

#include <gsl/gsl_errno.h>
#include <wx/cmdline.h>

wxConfigBase *Config = 0;

static void gsl_error_handler(const char *reason, const char *file, int line, int gsl_errno)
{
#if defined(_MSC_VER)
    DebugBreak();
#endif
}

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

    gsl_set_error_handler(&gsl_error_handler);

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
